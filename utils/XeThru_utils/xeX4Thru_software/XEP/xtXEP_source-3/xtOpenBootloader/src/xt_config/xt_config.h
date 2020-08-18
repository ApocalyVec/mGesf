#ifndef XT_CONFIG_H_
#define XT_CONFIG_H_

#include <stdint.h>

// Info
typedef struct {
    char item_number[10];
    uint8_t res1[2];
    char serial_number[12];
} xt_info_config_t;

extern xt_info_config_t info_config;
#define XT_CONFIG_ITEM_NUMBER()     info_config.item_number
#define XT_CONFIG_SERIAL_NUMBER()   info_config.serial_number

// IO Configuration
typedef struct {
    uint8_t IO1;
    uint8_t IO2;
    uint8_t IO3;
    uint8_t IO4;

    uint8_t IO5;
    uint8_t IO6;
    uint8_t res1[2];
    
    uint8_t ModeSel1;
    uint8_t ModeSel2;
    uint8_t res2;
    uint8_t VBUS;
} xt_io_config_t;

extern xt_io_config_t io_config;
#define XT_CONFIG_IO1()         io_config.IO1
#define XT_CONFIG_IO2()         io_config.IO2
#define XT_CONFIG_IO3()         io_config.IO3
#define XT_CONFIG_IO4()         io_config.IO4
#define XT_CONFIG_IO5()         io_config.IO5
#define XT_CONFIG_IO6()         io_config.IO6
#define XT_CONFIG_MODESEL1()    io_config.ModeSel1
#define XT_CONFIG_MODESEL2()    io_config.ModeSel2
#define XT_CONFIG_VBUS()        io_config.VBUS

// LED Configuration
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t mode;
} xt_led_config_t;

extern xt_led_config_t led_config;
#define XT_CONFIG_LED_RED()         led_config.red
#define XT_CONFIG_LED_GREEN()       led_config.green
#define XT_CONFIG_LED_BLUE()        led_config.blue
#define XT_CONFIG_LED_MODE()        led_config.mode

// System Configuration
typedef struct {
    uint8_t FWS;    
    uint8_t oscillator_selection;
    uint8_t vmin;
    uint8_t vmax;
} xt_system_config_t;

extern xt_system_config_t system_config;
#define XT_CONFIG_SYSTEM_FWS()              system_config.FWS
#define XT_CONFIG_SYSTEM_OSC_SELECTION()    system_config.oscillator_selection
#define XT_CONFIG_SYSTEM_BYPASS_OSC()       (system_config.oscillator_selection==0x1)
#define XT_CONFIG_SYSTEM_INT_ONLY_OSC()     (system_config.oscillator_selection==0x2)
#define XT_CONFIG_SYSTEM_VMIN()             system_config.vmin
#define XT_CONFIG_SYSTEM_VMAX()             system_config.vmax

// SDRAM Configuration
typedef struct {
    uint8_t chip;
    uint8_t rows;
    uint8_t cols;
    uint8_t banks;

    uint8_t data_bus_width;
    uint8_t cas_latency;
    uint8_t write_recovery_delay;
    uint8_t row_cycle_delay_and_row_refresh_cycle;

    uint8_t row_precharge_delay;
    uint8_t row_to_column_delay;
    uint8_t active_to_precharge_delay;
    uint8_t exit_self_refresh_to_active_delay;

    uint8_t load_mode_register_command_to_active_or_refresh_command;
    uint8_t supports_unaligned_access;
    uint8_t res1[2];

    uint32_t low_power_config;

    uint32_t refresh_timer_count;
} xt_sdram_config_t;
#define XT_CONFIG_SDRAM_MOUNTED()       (sdram_config.chip != 0xFF)

extern xt_sdram_config_t sdram_config;

// Radar Configuration
typedef struct {
    uint8_t external_load_caps;
    uint8_t center_frequency;
    uint8_t tx_power;
    uint8_t prf_div;

    float frame_area_offset;
} xt_radar_config_t;

extern xt_radar_config_t radar_config;
#define XT_CONFIG_RADAR_EXT_LOAD_CAPS()     radar_config.external_load_caps
#define XT_CONFIG_RADAR_CENTER_FREQ()       radar_config.center_frequency
#define XT_CONFIG_RADAR_TX_POWER()          radar_config.tx_power
#define XT_CONFIG_RADAR_PRF_DIV()           radar_config.prf_div
#define XT_CONFIG_RADAR_FRAME_AREA_OFFSET() radar_config.frame_area_offset

// Communication Configuration
typedef struct {
    uint32_t module;

    uint8_t pin1;
    uint8_t pin2;
    uint8_t pin3;
    uint8_t pin4;

    uint8_t settings;
    uint8_t res1[3];
} xt_communication_config_t;

#define CONFIG_USB_INDEX    0
#define CONFIG_UART_INDEX   1
#define CONFIG_USRT_INDEX   2
#define CONFIG_SPI_INDEX    3
#define CONFIG_I2C_INDEX    4
#define CONFIG_INTF_COUNT   5
extern xt_communication_config_t communication_config[CONFIG_INTF_COUNT];

int xt_config_init(void);

#endif