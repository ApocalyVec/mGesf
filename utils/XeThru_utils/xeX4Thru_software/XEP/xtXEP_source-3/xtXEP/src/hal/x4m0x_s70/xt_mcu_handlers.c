/**
 * @file
 *
 * @brief Interrupt handlers for common MCU interrupts.
 */


#include "board.h"
#include "xep_hal.h"

/* The fault handler implementation calls a function called
prvGetRegistersFromStack(). */
void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress );

//#define INVESTIGATE_SYSTEM_ERROR

void NMI_Handler(void)
{
#ifdef INVESTIGATE_SYSTEM_ERROR
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler_nmi_address_const                         \n"
        " bx r2                                                     \n"
        " handler_nmi_address_const: .word prvGetRegistersFromStack \n"
    );
#endif

    xt_trigger_crash(XT_SWRST_NMI, "NMI Handler", true);
}

void HardFault_Handler(void)
{
#ifdef INVESTIGATE_SYSTEM_ERROR
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler_hardfault_address_const                   \n"
        " bx r2                                                     \n"
        " handler_hardfault_address_const: .word prvGetRegistersFromStack \n"
    );
#endif

    xt_trigger_crash(XT_SWRST_HARDFAULT, "HardFault Handler", true);
}

void MemManage_Handler(void)
{
#ifdef INVESTIGATE_SYSTEM_ERROR
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler_memmanage_address_const                   \n"
        " bx r2                                                     \n"
        " handler_memmanage_address_const: .word prvGetRegistersFromStack \n"
    );
#endif

    xt_trigger_crash(XT_SWRST_MEMMANAGE, "MemManage Handler", true);
}

void BusFault_Handler(void)
{
#ifdef INVESTIGATE_SYSTEM_ERROR
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler_busfault_address_const                    \n"
        " bx r2                                                     \n"
        " handler_busfault_address_const: .word prvGetRegistersFromStack \n"
    );
#endif

    xt_trigger_crash(XT_SWRST_BUSFAULT, "BusFault Handler", true);
}

void UsageFault_Handler(void)
{
#ifdef INVESTIGATE_SYSTEM_ERROR
	__asm volatile
	(
	" tst lr, #4                                                \n"
	" ite eq                                                    \n"
	" mrseq r0, msp                                             \n"
	" mrsne r0, psp                                             \n"
	" ldr r1, [r0, #24]                                         \n"
	" ldr r2, handler_usagefault_address_const                    \n"
	" bx r2                                                     \n"
	" handler_usagefault_address_const: .word prvGetRegistersFromStack \n"
	);
#endif

    xt_trigger_crash(XT_SWRST_USAGEFAULT, "UsageFault Handler", true);
}

/*
void SVC_Handler(void)
{
	//## nva_set_reset_reason(XT_SWRST_BUSFAULT);

	__asm volatile
	(
	" tst lr, #4                                                \n"
	" ite eq                                                    \n"
	" mrseq r0, msp                                             \n"
	" mrsne r0, psp                                             \n"
	" ldr r1, [r0, #24]                                         \n"
	" ldr r2, handler_svc_address_const                    \n"
	" bx r2                                                     \n"
	" handler_svc_address_const: .word prvGetRegistersFromStack \n"
	);
}
*/


void DebugMon_Handler(void)
{
#ifdef INVESTIGATE_SYSTEM_ERROR
	__asm volatile
	(
	" tst lr, #4                                                \n"
	" ite eq                                                    \n"
	" mrseq r0, msp                                             \n"
	" mrsne r0, psp                                             \n"
	" ldr r1, [r0, #24]                                         \n"
	" ldr r2, handler_debugmon_address_const                    \n"
	" bx r2                                                     \n"
	" handler_debugmon_address_const: .word prvGetRegistersFromStack \n"
	);
#endif

    xt_trigger_crash(XT_SWRST_DEBUGMON, "DebugMon Handler", true);
}

/*
void PendSV_Handler(void)
{
	//## nva_set_reset_reason(XT_SWRST_BUSFAULT);

	__asm volatile
	(
	" tst lr, #4                                                \n"
	" ite eq                                                    \n"
	" mrseq r0, msp                                             \n"
	" mrsne r0, psp                                             \n"
	" ldr r1, [r0, #24]                                         \n"
	" ldr r2, handler_pendsv_address_const                    \n"
	" bx r2                                                     \n"
	" handler_pendsv_address_const: .word prvGetRegistersFromStack \n"
	);
}
*/

/*
void SysTick_Handler(void)
{
	//## nva_set_reset_reason(XT_SWRST_BUSFAULT);

	__asm volatile
	(
	" tst lr, #4                                                \n"
	" ite eq                                                    \n"
	" mrseq r0, msp                                             \n"
	" mrsne r0, psp                                             \n"
	" ldr r1, [r0, #24]                                         \n"
	" ldr r2, handler_systick_address_const                    \n"
	" bx r2                                                     \n"
	" handler_systick_address_const: .word prvGetRegistersFromStack \n"
	);
}
*/

__attribute__ ((optimize("-O0"))) void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
/* These are volatile to try and prevent the compiler/linker optimising them
away as the variables never actually get used.  If the debugger won't show the
values of the variables, make them global my moving their declaration outside
of this function. */
volatile uint32_t r0;
volatile uint32_t r1;
volatile uint32_t r2;
volatile uint32_t r3;
volatile uint32_t r12;
volatile uint32_t lr; /* Link register. */
volatile uint32_t pc; /* Program counter. */
volatile uint32_t psr;/* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    /* When the following line is hit, the variables contain the register values. */
	while(1)
	{
		//## ioport_toggle_pin_level(redpin);
	}
}
