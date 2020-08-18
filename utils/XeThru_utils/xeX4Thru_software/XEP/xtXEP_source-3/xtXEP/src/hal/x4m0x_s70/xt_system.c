/**
 * @file
 *
 * @brief System level hardware functionality
 *
 */

#include "chip.h"
#include "board.h"
#include "xep_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sams70.h"
#include "xt_system.h"
#include "xt_config.h"

#define CERT_MODE_GPNVM_BIT 3
#define EXT_CLOCK_GPNVM_BIT 4

#define RESET_REASON_GPBR_REG   4
#define RESET_COUNT_GPBR_REG    5
#define CRASH_COUNT_GPBR_REG    6

#define EXT_MEM_DISABLE_GPNVM_BIT 2

#define GPBR_READ(reg) GPBR->SYS_GPBR[reg]
#define GPBR_WRITE(reg,val) GPBR->SYS_GPBR[reg]=val

void xt_task_report_idle(void);
void xt_task_report_active(void* task);

uint32_t cpu_load = 0;

// Symbols defined in linker script
extern int _end_fast_heap_memory;
extern int _start_fast_heap_memory;

int xt_idle_sleep(void)
{
    xt_task_report_idle();

	__DSB();
	__WFI();
	
	xt_task_report_active(NULL);

    return XT_SUCCESS;
}

int xt_deep_sleep(void)
{
    portENTER_CRITICAL();

    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    SUPC->SUPC_CR |= SUPC_CR_KEY_PASSWD | SUPC_CR_VROFF;

    portEXIT_CRITICAL();

    for (;;);
}

int xt_wait(uint32_t ms_to_wait)
{
    vTaskDelay(ms_to_wait / portTICK_PERIOD_MS);

    return XT_SUCCESS;
}

int xt_delay_us(uint32_t us_to_wait)
{
    uint64_t start_time = xt_get_system_timer_us();
    uint64_t delay_until = start_time + us_to_wait;

    // In case of overflow condition
    // make sure the timer overflows before continuing
    if (delay_until < start_time)
        while (start_time <= xt_get_system_timer_us());

    while (delay_until > xt_get_system_timer_us());

    return XT_SUCCESS;
}

int xt_software_reset(xt_swreset_reason_t reason)
{
    xt_set_reset_reason(reason);

    RSTC_ProcessorReset();

    // Reset of MCU should be triggered prior to this,
    // any return from this function is an error.
    return XT_ERROR;
}

int xt_use_external_memory(bool use)
{
    if (xt_external_memory_enabled() == use)
        return XT_SUCCESS;

    if (use)
        EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD |
                EEFC_FCR_FCMD_CGPB | EEFC_FCR_FARG(EXT_MEM_DISABLE_GPNVM_BIT) );
    else
        EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD |
                EEFC_FCR_FCMD_SGPB | EEFC_FCR_FARG(EXT_MEM_DISABLE_GPNVM_BIT) );

    uint32_t cmd_status;
    do {
        cmd_status = EFC->EEFC_FSR;
    } while ((cmd_status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY);

    return XT_SUCCESS;
}

bool xt_external_memory_enabled(void)
{
    static bool ext_mem_status_is_read = false;
    static bool ext_mem_enabled = false;

    if (!XT_CONFIG_SDRAM_MOUNTED())
        return false;

    if (!ext_mem_status_is_read)
    {
        ext_mem_status_is_read = true;

        uint32_t cmd_status;
        EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_GGPB);
        do {
            cmd_status = EFC->EEFC_FSR;
        } while ((cmd_status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY);
        ext_mem_enabled = ((EFC->EEFC_FRR & (1<<EXT_MEM_DISABLE_GPNVM_BIT)) == 0);
    }

    return ext_mem_enabled;
}

bool xt_fast_memory_enabled(void)
{
    return ((intptr_t)&_end_fast_heap_memory -
            (intptr_t)&_start_fast_heap_memory) > 0;
}

int xt_get_crash_count(void)
{
    static bool crash_count_incremented = false;
    
    if (!crash_count_incremented)
    {
        xt_swreset_reason_t reason;
        xt_get_reset_reason(&reason);
        
        // If an intentional reset was done, clear counter
        uint32_t count = 0;
        if ((reason != XT_SWRST_HW_PIN) && 
            (reason != XT_SWRST_INTENDED) &&
            (reason != XT_SWRST_HOSTCOMMAND) && 
            (reason != XT_SWRST_BOOTLOADER) && 
            (reason != XT_SWRST_NONE))
        {
            count = GPBR_READ(CRASH_COUNT_GPBR_REG) + 1;
        }
        
        GPBR_WRITE(CRASH_COUNT_GPBR_REG, count);

        crash_count_incremented = true;
    }

    return GPBR_READ(CRASH_COUNT_GPBR_REG);
}

int xt_reset_crash_count(void)
{
    GPBR_WRITE(CRASH_COUNT_GPBR_REG, 0);

    return XT_SUCCESS;
}

int xt_get_reset_count(void)
{
    static bool reset_count_incremented = false;
    
    if (!reset_count_incremented)
    {
        uint32_t count = GPBR_READ(RESET_COUNT_GPBR_REG) + 1;

        // If coming from the bootloader, reset counter
        xt_swreset_reason_t reason;
        xt_get_reset_reason(&reason);
        if (reason == XT_SWRST_BOOTLOADER)
            count = 1;

        GPBR_WRITE(RESET_COUNT_GPBR_REG, count);

        reset_count_incremented = true;
    }

    return GPBR_READ(RESET_COUNT_GPBR_REG);
}

int xt_set_reset_reason(xt_swreset_reason_t reason)
{
    GPBR_WRITE(RESET_REASON_GPBR_REG, reason);

    return XT_SUCCESS;
}

int xt_get_reset_reason(xt_swreset_reason_t * reason)
{
    static bool reset_reason_is_read = false;
    static xt_swreset_reason_t reset_reason = XT_SWRST_NONE;

    if (!reset_reason_is_read)
    {
        uint32_t reset_type = (RSTC_GetStatus()&RSTC_SR_RSTTYP_Msk);
        
        if (reset_type == RSTC_SR_RSTTYP_SOFT_RST) //SOFT_RST
            reset_reason = GPBR_READ(RESET_REASON_GPBR_REG);
        else if (reset_type == RSTC_SR_RSTTYP_WDT_RST) // WDT_RST
            reset_reason = XT_SWRST_HARD_WDT;
        else if (reset_type == RSTC_SR_RSTTYP_USER_RST) // NRST pin
            reset_reason = XT_SWRST_HW_PIN;

        xt_set_reset_reason(XT_SWRST_NONE);

        reset_reason_is_read = true;
    }

    *reason = reset_reason;

    return XT_SUCCESS;
}

int xt_use_external_clock(bool use)
{
    // When only internal oscillator is available, ignore request
    if (XT_CONFIG_SYSTEM_INT_ONLY_OSC())
        return XT_SUCCESS;

    if (use)
    {
        EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD |
                    EEFC_FCR_FCMD_SGPB | EEFC_FCR_FARG(EXT_CLOCK_GPNVM_BIT));
    } else
    {
        EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD |
                    EEFC_FCR_FCMD_CGPB | EEFC_FCR_FARG(EXT_CLOCK_GPNVM_BIT));
    }

    return XT_SUCCESS;
}

bool xt_using_external_clock(void)
{
    // Check HW config, 0xFF means the GPNVM bit must be checked
    if (XT_CONFIG_SYSTEM_OSC_SELECTION() != 0xFF)
        return XT_CONFIG_SYSTEM_BYPASS_OSC() != 0;

    uint32_t cmd_status;
    EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_GGPB);
    do {
        cmd_status = EFC->EEFC_FSR;
    } while ((cmd_status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY);
    return ((EFC->EEFC_FRR & (1<<EXT_CLOCK_GPNVM_BIT)) != 0);
}

int xt_get_certification_submode(void)
{
    int submode = 0;

    if ((XT_CONFIG_IO1() != 0xFF) &&
        (XT_CONFIG_IO2() != 0xFF) && 
        (XT_CONFIG_IO3() != 0xFF) &&
        (XT_CONFIG_IO4() != 0xFF) &&
        (XT_CONFIG_IO5() != 0xFF) &&
        (XT_CONFIG_IO6() != 0xFF))
    {
        // Set input pins
        xtio_set_direction(XTIO_XETHRU_IO1, XTIO_INPUT, XTIO_PIN_LEVEL_LOW);
        xtio_set_direction(XTIO_XETHRU_IO3, XTIO_INPUT, XTIO_PIN_LEVEL_LOW);
        xtio_set_direction(XTIO_XETHRU_IO5, XTIO_INPUT, XTIO_PIN_LEVEL_LOW);
        xtio_set_pin_mode(XTIO_XETHRU_IO1, XTIO_PULL_UP);
        xtio_set_pin_mode(XTIO_XETHRU_IO3, XTIO_PULL_UP);
        xtio_set_pin_mode(XTIO_XETHRU_IO5, XTIO_PULL_UP);

        // Set driving pins
        xtio_set_direction(XTIO_XETHRU_IO2, XTIO_OUTPUT, XTIO_PIN_LEVEL_LOW);
        xtio_set_direction(XTIO_XETHRU_IO4, XTIO_OUTPUT, XTIO_PIN_LEVEL_LOW);
        xtio_set_direction(XTIO_XETHRU_IO6, XTIO_OUTPUT, XTIO_PIN_LEVEL_LOW);

        xt_delay_us(10);

        // Read sub-mode by checking level
        xtio_pin_level_t pin_level;
        xtio_get_level(XTIO_XETHRU_IO1, &pin_level);
        if (pin_level == XTIO_PIN_LEVEL_LOW)
            submode |= 1<<0;
        xtio_get_level(XTIO_XETHRU_IO3, &pin_level);
        if (pin_level == XTIO_PIN_LEVEL_LOW)
            submode |= 1<<1;
        xtio_get_level(XTIO_XETHRU_IO5, &pin_level);
        if (pin_level == XTIO_PIN_LEVEL_LOW)
            submode |= 1<<2;
    } else
    {
        // Set input pins
        xtio_set_direction(XTIO_SERIAL_TX, XTIO_INPUT, XTIO_PIN_LEVEL_LOW);
        xtio_set_direction(XTIO_SERIAL_RX, XTIO_INPUT, XTIO_PIN_LEVEL_LOW);
        xtio_set_direction(XTIO_SERIAL_SCLK, XTIO_INPUT, XTIO_PIN_LEVEL_LOW);
        xtio_set_pin_mode(XTIO_SERIAL_TX, XTIO_PULL_UP);
        xtio_set_pin_mode(XTIO_SERIAL_RX, XTIO_PULL_UP);
        xtio_set_pin_mode(XTIO_SERIAL_SCLK, XTIO_PULL_UP);

        xt_delay_us(10);

        // Read sub-mode by checking level
        xtio_pin_level_t pin_level;
        xtio_get_level(XTIO_SERIAL_TX, &pin_level);
        if (pin_level == XTIO_PIN_LEVEL_LOW)
            submode |= 1<<0;
        xtio_get_level(XTIO_SERIAL_RX, &pin_level);
        if (pin_level == XTIO_PIN_LEVEL_LOW)
            submode |= 1<<1;
        xtio_get_level(XTIO_SERIAL_SCLK, &pin_level);
        if (pin_level == XTIO_PIN_LEVEL_LOW)
            submode |= 1<<2;
    }
    
    return submode;
}

int xt_enable_certification_mode(bool cert_mode)
{
    if (cert_mode)
    {
        EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD |
                    EEFC_FCR_FCMD_SGPB | EEFC_FCR_FARG(CERT_MODE_GPNVM_BIT));
    } else
    {
        EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD |
                    EEFC_FCR_FCMD_CGPB | EEFC_FCR_FARG(CERT_MODE_GPNVM_BIT));
    }

    return XT_SUCCESS;
}

bool xt_is_in_certification_mode(void)
{
	uint32_t cmd_status;
	
    EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_GGPB);
    do {
        cmd_status = EFC->EEFC_FSR;
    } while ((cmd_status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY);
    return ((EFC->EEFC_FRR & (1<<CERT_MODE_GPNVM_BIT)) != 0);
}

xt_opmode_t xt_get_operation_mode(void)
{
    static xt_opmode_t operation_mode = XT_OPMODE_UNINITIALIZED;

    if (operation_mode == XT_OPMODE_UNINITIALIZED)
    {
        if (xt_is_in_certification_mode())
        {
            operation_mode = XT_OPMODE_CERTIFICATION;
        } else
        {
            xtio_set_direction(XTIO_MODE_SEL1, XTIO_INPUT, XTIO_PIN_LEVEL_LOW);
            xtio_set_pin_mode(XTIO_MODE_SEL1, XTIO_PULL_UP);
            xtio_set_direction(XTIO_MODE_SEL2, XTIO_INPUT, XTIO_PIN_LEVEL_LOW);
            xtio_set_pin_mode(XTIO_MODE_SEL2, XTIO_PULL_UP);
            
            for (volatile int i = 0; i < 1000; i++);

            xtio_pin_level_t level1, level2;
            xtio_get_level(XTIO_MODE_SEL1, &level1);
            xtio_get_level(XTIO_MODE_SEL2, &level2);
            operation_mode = (level1<<0) | (level2<<1);
        }
    }
   
	return operation_mode;
}

uint64_t xt_get_system_timer_us(void)
{
    uint64_t ostick = xTaskGetTickCount();
    uint64_t period = SysTick->LOAD-1;
    uint64_t systickval = SysTick->LOAD - SysTick->VAL;
    uint64_t systick_us = systickval * 1000 / period;
    uint64_t us = ostick * 1000 + systick_us;

    return us;
}

void * get_pc (void);
void * get_pc (void) { return __builtin_return_address(0); }
void* xt_get_program_counter_from_task_handle(void* handle)
{
    if ((((uint32_t)handle) % 4) != 0)
        return 0;

    // First value in FreeRTOS TCB (task handle) is pointer to task top of stack
    uint32_t* stack_pointer = (uint32_t*)*((uint32_t*)handle);
	
    if (xTaskGetCurrentTaskHandle() == handle)
        return (void*) get_pc();
	
    // On task switch, FreeRTOS will push R4-R11, R14 (LR) and S16-S31 if in FPU context (LR & 0x10) to stack.
    // The SysTick interrupt that caused the task switch will push R0-R3, R12, LR and PC to stack. 
    // This means that PC is at index 31.
    if ((stack_pointer[8] & 0x10) == 0)
		return (void*)stack_pointer[31];
	return (void*)stack_pointer[15];
}

int xt_get_stack_area_from_task_handle(void* handle, uint32_t stack_size, void** stack_pointer, uint32_t* depth)
{	
    uint32_t stack_base = (uint32_t)((uint32_t*)handle)[12]; // Directly accessing stack pointer in TCB
    uint32_t stack_end = stack_base + stack_size*sizeof(uint32_t);
    
    // First value in FreeRTOS TCB (task handle) is pointer to task top of stack
    *stack_pointer = (uint32_t*)*((uint32_t*)handle);
	
    *depth = stack_end - ((uint32_t)*stack_pointer);
	
    return XT_SUCCESS;
}

int xt_feed_watchdog(void)
{
    WDT_Restart(WDT);

    // Reset Reinforced WDT
	RSWDT->RSWDT_CR = 0xC4000001;

	return XT_SUCCESS;
}

uint32_t xt_get_cpu_load(void)
{
	return cpu_load;
}

int (*crashdump_callback)(xt_swreset_reason_t,const char*,bool) = NULL;
int xt_register_crashdump_callback(int (*callback)(xt_swreset_reason_t,const char*,bool))
{
    crashdump_callback = callback;

    return XT_SUCCESS;
}

int xt_trigger_crash(xt_swreset_reason_t crash_reason, const char* crash_info, bool full_dump)
{
    if (crashdump_callback != NULL)
        crashdump_callback(crash_reason, crash_info, full_dump);
    xt_software_reset(crash_reason);
	
	return XT_SUCCESS;
}

void WDT_Handler(void)
{
    xt_trigger_crash(XT_SWRST_WDT, "Watchdog timeout", true);
}

inline void xt_task_report_idle(void)
{
	PIOA->PIO_CODR = (1<<29);
}

inline void xt_task_report_active(void* task)
{
	PIOA->PIO_SODR = (1<<29);
}

void TC1_Handler(void)
{	
	uint32_t timer_value = TC0->TC_CHANNEL[0].TC_CV;
	
	cpu_load = timer_value/64;
	if (cpu_load > 1000)
		cpu_load = 1000;
	TC0->TC_BCR = (1<<0); // Resets and triggers all channels
	
    int sr = TC0->TC_CHANNEL[1].TC_SR;
    UNUSED(sr);
}

extern const DeviceVectors exception_table;
bool xt_has_bootloader(void)
{
	// If we have a bootloader, we're linked in a little bit higher.
	return (intptr_t)&exception_table > 0x400000;
}

const char *xt_bootloader_string(void)
{
	if (!xt_has_bootloader()) {
		return "";
	}
	// This should be either "XETHRU BOOTLOADER" or "XETHRU OPENBOOTLOADER"
	for (const char *ptr = (const char*)0x400000;
	    (intptr_t)ptr < 0x410000; ++ptr) {
		if (ptr[0] == 'X' && ptr[1] == 'E' && ptr[2] == 'T' &&
		    ptr[3] == 'H' && ptr[4] == 'R' && ptr[5] == 'U')
			return ptr;
	}
	return "unknown";
}
