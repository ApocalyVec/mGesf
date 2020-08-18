#include "xt_config.h"
#include <asf.h>
#include <string.h>

#define CONFIG_SIZE         512
#define CONFIG_MAGIC        "$XTHWCFG"
#define CONFIG_VERSION      ((1<<16)|(0<<0))

#define SECTION_INFO            0x00
#define SECTION_SYSTEM          0x10
#define SECTION_SDRAM           0x11
#define SECTION_COMM_USB        0x20
#define SECTION_COMM_UART       0x21
#define SECTION_COMM_USRT       0x22
#define SECTION_COMM_SPI        0x23
#define SECTION_COMM_I2C        0x24
#define SECTION_IO              0x40
#define SECTION_LED             0x41
#define SECTION_RADAR           0x80
#define SECTION_END             0xFF

uint8_t raw_config_buffer[CONFIG_SIZE]; 

typedef struct {
    char magic_key[8];
    uint32_t version;
    uint32_t checksum;
} xt_config_header_t;

typedef struct {
    uint8_t id;
    uint8_t length;
    uint16_t offset;
} xt_sat_entry_t;

xt_info_config_t info_config = {
                                .item_number = {'0','0','0','0','0','0','-','0','0','0',}, // Item number is unknown
                                .res1 = {0xFF, 0xFF},
                                .serial_number = {'0','0','0','0','0','0','0','0','0','0','0','0',},
                                };

xt_io_config_t io_config = {    .IO1 = 0x1C, // PA28
                                .IO2 = 0x19, // PA25
                                .IO3 = 0x1E, // PA30
                                .IO4 = 0x1F, // PA31
                                .IO5 = 0x1A, // PA26
                                .IO6 = 0x1B, // PA27
                                .res1 = {0xFF, 0xFF},
                                .ModeSel1 = 0x2D, // PB13
                                .ModeSel2 = 0x22, // PB2
                                .res2 = 0xFF,
                                .VBUS = 0x02, // PA2
                                };

xt_communication_config_t communication_config[CONFIG_INTF_COUNT] = {
                            // USB
                            {   .module = (uint32_t)USBHS,

                                .pin1 = 0xFE, // HSDM, special
                                .pin2 = 0xFE, // HSDP, special
                                .pin3 = 0x02, // VBUS = PA2
                                .pin4 = 0xFF,
                                
                                .settings = 1, // Primary
                                .res1 = {0xFF, 0xFF, 0xFF},
                            },
                            // UART
                            {   .module = (uint32_t)USART0,

                                .pin1 = 0x20, // RX = PB0
                                .pin2 = 0x21, // TX = PB1
                                .pin3 = 0xFF,
                                .pin4 = 0xFF,

                                .settings = (3<<4), // Secondary, ModeSel = 3
                                .res1 = {0xFF, 0xFF, 0xFF},
                            },
                            // USRT
                            {   .module = (uint32_t)USART0,

                                .pin1 = 0x20, // RX = PB0
                                .pin2 = 0x21, // TX = PB1
                                .pin3 = 0x2D, // SCLK = PB13
                                .pin4 = 0xFF,
                                
                                .settings = (2<<4), // Secondary, ModeSel = 2
                                .res1 = {0xFF, 0xFF, 0xFF},
                            },
                            // SPI
                            {   .module = (uint32_t)USART0,

                                .pin1 = 0x20, // MOSI = PB0
                                .pin2 = 0x21, // MISO = PB1
                                .pin3 = 0x2D, // SCLK = PB13
                                .pin4 = 0x22, // nSS = PB2

                                .settings = (1<<4), // Secondary, ModeSel = 1
                                .res1 = {0xFF, 0xFF, 0xFF},
                            },
                            // I2C
                            {   .module = (uint32_t)0xFFFFFFFF, // No I2C

                                .pin1 = 0xFF, 
                                .pin2 = 0xFF, 
                                .pin3 = 0xFF, 
                                .pin4 = 0xFF, 

                                .settings = 0xFF, 
                                .res1 = {0xFF, 0xFF, 0xFF},
                            },};

xt_led_config_t led_config = {  .red = 0x78, // PD24
                                .green = 0x79, // PD25
                                .blue = 0x7A, // PD26
                                .mode = 0, 
                                };

xt_system_config_t system_config = {    .FWS = 5, // For 2.7V operation, S70 Datasheet Table 54-54          
                                        .oscillator_selection = 0xFF, // Check GPNVM bit 4
                                        .vmin = 29, // 2.9V min
                                        .vmax = 31, // 3.1V max
                                        };

xt_radar_config_t radar_config = {  .external_load_caps = 1, // Mounted
                                    .center_frequency = 3, // EU = 7.29GHz
                                    .tx_power = 2, // Medium
                                    .prf_div = 16,
                                    .frame_area_offset = 0.18 // Antenna-specific, measured
                                    };

xt_sdram_config_t sdram_config = {  .chip = 0, // W9864G6JT-6I
                                    .rows = 8, 
                                    .cols = 12,
                                    .banks = 4,
                                    .data_bus_width = 16,
                                    .cas_latency = 3,
                                    .write_recovery_delay = 2,
                                    .row_cycle_delay_and_row_refresh_cycle = 12,
                                    .row_precharge_delay = 3,
                                    .row_to_column_delay = 3,
                                    .active_to_precharge_delay = 7,
                                    .exit_self_refresh_to_active_delay = 12,
                                    .load_mode_register_command_to_active_or_refresh_command = 3,
                                    .supports_unaligned_access = 1,
                                    .res1 = {0xFF, 0xFF},

                                    .low_power_config = 0,

                                    .refresh_timer_count = 15625,
                                    };

typedef struct {
    uint8_t id;
    void* config;
    uint32_t config_size;
} xt_config_binding_t;

xt_config_binding_t config_bindings[] = {   {SECTION_INFO, &info_config, sizeof(info_config)},
                                            {SECTION_IO, &io_config, sizeof(io_config)},
                                            {SECTION_LED, &led_config, sizeof(led_config)},
                                            {SECTION_COMM_USB, &communication_config[CONFIG_USB_INDEX], sizeof(communication_config[CONFIG_USB_INDEX])},
                                            {SECTION_COMM_UART, &communication_config[CONFIG_UART_INDEX], sizeof(communication_config[CONFIG_UART_INDEX])},
                                            {SECTION_COMM_USRT, &communication_config[CONFIG_USRT_INDEX], sizeof(communication_config[CONFIG_USRT_INDEX])},
                                            {SECTION_COMM_SPI, &communication_config[CONFIG_SPI_INDEX], sizeof(communication_config[CONFIG_SPI_INDEX])},
                                            {SECTION_COMM_I2C, &communication_config[CONFIG_I2C_INDEX], sizeof(communication_config[CONFIG_I2C_INDEX])},
                                            {SECTION_SYSTEM, &system_config, sizeof(system_config)},
                                            {SECTION_SDRAM, &sdram_config, sizeof(sdram_config)},
                                            {SECTION_RADAR, &radar_config, sizeof(radar_config)},
                                        };

void xt_config_build(void);
int xt_config_find_section(uint8_t section_id, uint8_t* length, uint16_t* offset);

uint32_t xtio_flash_read_hw_data(uint8_t* buffer, uint32_t max_size, uint32_t* size);

__attribute__ ((section (".ramfunc")))
uint32_t xtio_flash_read_hw_data(uint8_t* buffer, uint32_t max_size, uint32_t* size)
{
    irqflags_t flags = cpu_irq_save();

    // Start user signature read
    EFC->EEFC_FCR = EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_STUS;
    int timeout = 100;
    while ((EFC->EEFC_FSR & EEFC_FSR_FRDY) && (timeout > 0))
    {
        timeout--;
        for (volatile int i = 0; i < 1000; i++);
    } 
    if (timeout < 0)
    {
        cpu_irq_restore(flags);
        return 0;
    }
    
    uint32_t i = 0;
    uint8_t* aligned_addr = (uint8_t*)0x20000;
    while ((i < max_size) && (i < IFLASH_PAGE_SIZE))
    {
        buffer[i] = aligned_addr[i];
        i++;
    }
    *size = i;
    
    // Stop user signature read, no need for timeout since nothing will work if this fails.. 
    EFC->EEFC_FCR = EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_SPUS;
    while ((EFC->EEFC_FSR & EEFC_FSR_FRDY) == 0);
    
    cpu_irq_restore(flags);
    
    return 1;
}

int xt_config_init(void)
{
    // Read configuration page
    uint32_t size = 0;
    xtio_flash_read_hw_data(raw_config_buffer, CONFIG_SIZE, &size);
    if (size != CONFIG_SIZE)
        return 0;

    xt_config_header_t* config_header = (xt_config_header_t*) raw_config_buffer;

    // Check magic key
    if (strcmp(config_header->magic_key, CONFIG_MAGIC) != 0)
        return 0;
    
    // Calculate checksum, 32bit xor-sum of entire page should be 0
    uint32_t checksum = 0;
    for (uint32_t i = 0; i < CONFIG_SIZE/sizeof(uint32_t); i++)
        checksum ^= ((uint32_t*)raw_config_buffer)[i];
    if (checksum != 0)
        return 0;
    
    // Check version
    if (config_header->version != CONFIG_VERSION)
        return 0;
    
    // Build configuration information structures to override defaults
    xt_config_build();

    return 1;
}

void xt_config_build(void)
{
    // Go through all bindings of ID and config structures
    for (uint32_t i = 0; i < sizeof(config_bindings)/sizeof(xt_config_binding_t); i++)
    {
        uint8_t length = 0;
        uint16_t offset = 0;
        
        // Search for section, copy if found, otherwise using defaults
        if (xt_config_find_section(config_bindings[i].id, &length, &offset))
        {
            // Make sure to not write outside of expected structure
            if (config_bindings[i].config_size < length)
                length = config_bindings[i].config_size;
            
            memcpy(config_bindings[i].config, &raw_config_buffer[offset], length);
        }
    }
}

int xt_config_find_section(uint8_t section_id, uint8_t* length, uint16_t* offset)
{
    // Traverse Section Allocation Table to find requested ID
    xt_sat_entry_t* sat_entry = (xt_sat_entry_t*) &raw_config_buffer[sizeof(xt_config_header_t)];
    while (sat_entry->id != SECTION_END)
    {
        if (sat_entry->id == section_id)
        {
            *length = sat_entry->length;
            *offset = sat_entry->offset;
            return 1;
        }
        sat_entry = (xt_sat_entry_t*)((uint32_t)sat_entry + sizeof(xt_sat_entry_t));
    }

    *length = 0;
    *offset = 0;

    return 0;
}