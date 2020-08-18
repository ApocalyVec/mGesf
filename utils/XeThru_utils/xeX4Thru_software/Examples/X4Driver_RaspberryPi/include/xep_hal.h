/**
 * @file
 *
 * @brief Brief description
 *
 * Longer description
 */

#ifndef XT_XEP_HAL_H
#define XT_XEP_HAL_H

#include <stdint.h>
#include <stdbool.h>

typedef struct xtProtocol xtProtocol;

//==============================================================================
// Common defines for HAL_API


typedef enum {
    XT_SUCCESS = 0,            ///< Return value of a function that executed successfully
    XT_NOT_IMPLEMENTED = 900,  ///< Return value of a function that is not implemented
    XT_ERROR  = 999,           ///< Return value of a function where an unspecified error occured
}xt_error_codes_t;

//==============================================================================
// Macros and typedefs for Selftest

#define XT_SELFTEST_DATA_LENGTH     256

//==============================================================================
// Macros and typedefs for System

/**
 * Operation mode
 */
 typedef enum
{
    XT_OPMODE_FACTORY       = 0,    ///< Factory test mode
    XT_OPMODE_SYNC_SERIAL   = 1,    ///< USART communication mode
    XT_OPMODE_SPI           = 2,    ///< SPI communication mode
    XT_OPMODE_SERIAL        = 3,    ///< UART communication mode
    XT_OPMODE_CERTIFICATION = 32,   ///< Certification mode
    XT_OPMODE_UNINITIALIZED = 0xFF  ///< Mode is not initialized
} xt_opmode_t;

/**
 * Cause of a software-triggered reset
 */
typedef enum
{
    XT_SWRST_NONE           = 0x00, ///< No cause of reset
    XT_SWRST_INTENDED       = 0x01, ///< Reset was intentionally triggered
    XT_SWRST_BOOTLOADER     = 0x02, ///< Enter bootloader mode
    XT_SWRST_SYSTEM_MONITOR = 0x10, ///< System monitor detected an unrecoverable error
    XT_SWRST_MALLOC_FAILED  = 0x11, ///< Failed to allocate memory
    XT_SWRST_STACK_OVERFLOW = 0x12, ///< Overflow in stack
    XT_SWRST_HARDFAULT      = 0x13, ///< MCU HardFault triggered
    XT_SWRST_MEMMANAGE      = 0x14, ///< MCU MemManage fault triggered
    XT_SWRST_BUSFAULT       = 0x15, ///< MCU BusFault triggered
    XT_SWRST_USAGEFAULT     = 0x16, ///< MCU UsageFault triggered
    XT_SWRST_DEBUGMON       = 0x17, ///< MCU DebugMon fault triggered
    XT_SWRST_NMI            = 0x18, ///< Non-Maskable Interrupt triggered
    XT_SWRST_WDT            = 0x19, ///< Watchdog Timer triggered
    XT_SWRST_HW_PIN         = 0x1A, ///< HW reset pin triggered
    XT_SWRST_HARD_WDT       = 0x1B, ///< Hard Watchdog Timer triggered
    XT_SWRST_ASSERT         = 0x1C, ///< An assert was triggered.
    XT_SWRST_HOSTCOMMAND    = 0x30, ///< Host requested reset
    XT_SWRST_APPUNLOAD      = 0x40, ///< Application was unloaded
    XT_SWRST_UNDEFINED      = 0x50  ///< Unknown cause of software reset
} xt_swreset_reason_t;

//==============================================================================
// Macroes and typedefs for GPIO

/**
 * ID of all IO pins that may be available.
 *
 * Some board implementations may not implement all IO pins.
 * If an IO pin is not implemented, an error code shall be returned.
 */
typedef enum {
    XTIO_XETHRU_IO1 = 1,  ///< ID for IO pin IO1 on the XeThru 16 pin connector

    XTIO_XETHRU_IO2 = 2,  ///< ID for IO pin IO2 on the XeThru 16 pin connector

    XTIO_XETHRU_IO3 = 3,  ///< ID for IO pin IO3 on the XeThru 16 pin connector

    XTIO_XETHRU_IO4 = 4,  ///< ID for IO pin IO4 on the XeThru 16 pin connector

    XTIO_XETHRU_IO5 = 5,  ///< ID for IO pin IO5 on the XeThru 16 pin connector

    XTIO_XETHRU_IO6 = 6,  ///< ID for IO pin IO6 on the XeThru 16 pin connector

    XTIO_XETHRU_IO7 = 7,  ///< ID for IO pin IO7 on the XeThru 16 pin connector.
                          ///< This pin can also function as WAKEUP and should
                          ///< normally not be used as an IO pin

    XTIO_XETHRU_IO8 = 8,  ///< ID for IO pin IO8 on the XeThru 16 pin connector.
                          ///< This pin can also function as SWCLK and should
                          ///< normally not be used as an IO pin

    XTIO_XETHRU_IO9 = 9,  ///< ID for IO pin IO9 on the XeThru 16 pin connector.
                          ///< This pin can also function as SWDIO and should
                          ///< normally not be used as an IO pin

    XTIO_X4_IO1 = 41,     ///< ID for IO pin X4_IO1 on tin the X4 Radar IC
                          ///< This pin can also have functions other than GPIO
                          ///< during communication with X4

    XTIO_X4_IO2 = 42,     ///< ID for IO pin X4_IO2 on tin the X4 Radar IC
                          ///< This pin can also have functions other than GPIO
                          ///< during communication with X4

    XTIO_X4_IO3 = 43,     ///< ID for IO pin X4_IO3 on tin the X4 Radar IC
                          ///< This pin can also have functions other than GPIO
                          ///< during communication with X4

    XTIO_X4_IO4 = 44,     ///< ID for IO pin X4_IO4 on tin the X4 Radar IC
                          ///< This pin can also have functions other than GPIO
                          ///< during communication with X4

    XTIO_X4_ENABLE = 49,  ///< ID for IO pin X4_ENABLE on tin the X4 Radar IC

    XTIO_MODE_SEL1 = 70, ///< ID for IO pin for mode selection
    
    XTIO_MODE_SEL2 = 71, ///< ID for IO pin for mode selection

    XTIO_SERIAL_SCLK = 80,  ///< ID for IO pin on the XeThru 16 pin connector
                            ///< This pin can also function as clock in SPI mode

    XTIO_SERIAL_nSS = 81,   ///< ID for IO pin on the XeThru 16 pin connector
                            ///< This pin can also function as slave select in SPI mode

    XTIO_SERIAL_RX = 82,    ///< ID for IO pin on the XeThru 3 pin connector
                            ///< This pin normally is used for serial communication
                            
    XTIO_SERIAL_TX = 83,    ///< ID for IO pin on the XeThru 4 pin connector
                            ///< This pin normally is used for serial communication

    XTIO_USB_VBUS = 90,   ///< ID for IO pin sensing USB VBUS
} xtio_pin_id_t;

/// Normal GPIO mode - this is default mode
#define XTIO_GPIO_MODE               0x0001
/// Pull up enabled
#define XTIO_PULL_UP                 0x0010
/// Pull down enabled
#define XTIO_PULL_DOWN               0x0020
/// No pull ups enabled
#define XTIO_FLOATING                0x0030
// Open-drain output driver
#define XTIO_OPEN_DRAIN              0x0040
/// Enable interrupt for IO pin
#define XTIO_INTERRUPT_MODE          0x0002

/**
 * Trigger mode for interrupt on IO pins
 */
typedef enum {
    XTIO_INTERRUPT_NONE = 0,         ///< No interrupt will trigger
    XTIO_INTERRUPT_RISING_EDGE = 1,  ///< Interrupt triggers on rising edge
    XTIO_INTERRUPT_FALLING_EDGE = 2, ///< Interrupt triggers on fallin edge
} xtio_interrupt_modes;

/**
 * IO pin direction "macroes"
 */
typedef enum {
    XTIO_INPUT,         ///< IO pin as input
    XTIO_OUTPUT,        ///< IO pin as output
} xtio_pin_direction_t;

/**
 * IOPORT levels
 */
typedef enum {
	XTIO_PIN_LEVEL_LOW = 0,  ///< IOPORT pin value low
	XTIO_PIN_LEVEL_HIGH = 1, ///< IOPORT pin value high
}xtio_pin_level_t;

//==============================================================================
// Macroes and typedefs for LED

/**
 * ID of all LEDs that may be available.
 *
 * Some board implementations may not implement all LEDs.
 * If a LED is not implemented, an error code shall be returned.
 */
typedef enum {
    XTIO_LED_RED    = 1,      ///< ID for red LED
    XTIO_LED_GREEN  = 2,      ///< ID for green LED
    XTIO_LED_BLUE   = 3,      ///< ID for blue LED
    XTIO_LED_YELLOW = 4,      ///< ID for yellow LED
    XTIO_LED_WHITE  = 5,      ///< ID for white LED
} xtio_led_id_t;

/**
 * Mode for LEDs that may be available.
 *
 * Some board implementations may not implement all LED modes.
 * If a LED mode is not implemented, an error code shall be returned.
 */
typedef enum {
    XTIO_LED_ONOFF,    ///< LED is ON of OFF
    XTIO_LED_PWM,      ///< LED is controlled by PWM
} xtio_led_mode_t;

/**
 * States for LEDs that may be available.
 *
 * Some board implementations may not implement all LED states.
 * If a LED state is not implemented, an error code shall be returned.
 *
 * In addition, PWM controlled LEDs may also be available. Brightness of light
 * is then controlled by a value between 0.0 and 1.0.
 */
typedef enum {
    XTIO_LED_OFF = 0,  ///< LED is OFF
    XTIO_LED_ON  = 1,  ///< LED is OFF
} xtio_led_state_t;

/**
 * Error codes for Xethru IO driver
 */
typedef enum {
    /// Executed successfully
    XTIO_SUCCESS = XT_SUCCESS,
    /// Using Pin ID not implemented for this HW board
    XTIO_WRONG_PIN_ID = 1,
    /// IO function not available for this pin
    XTIO_IO_FUNCTION_NOT_AVAILABLE = 2,
    /// Interrupt for this IO pin is not available
    XTIO_INTERRUPT_NOT_AVAILABLE = 3,
    /// IO mode not supported
    XTIO_MODE_NOT_SUPPORTED = 4,
    /// LED is not present on this board
    XTIO_WRONG_LED_ID = 10,
    /// LED mode not available
    XTIO_LED_MODE_NOT_AVAILABLE = 11,
} xtio_error_codes_t;


//==============================================================================
// Typedefs related to HW and/or ASF

/**
 * @brief 32-bit floating-point type definition.
 */
typedef float float32_t;

//==============================================================================
// Prototypes of all API functions in XEP HAL

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================
// Selftest

/**
 * Struct with results specific for a test
 */
typedef struct
{
    int id;
    uint8_t passed;
    uint8_t data[XT_SELFTEST_DATA_LENGTH];
} xt_test_result_t;

/**
 * @brief Performs the specified selftest
 * 
 * @return Status of execution
 */
int xt_run_selftest(
    uint8_t testcode,               ///< Test to run as defined in @ref xt_selftest_code_t
    xt_test_result_t* test_result   ///< Pointer to struct that will hold result of test
);

/**
 * @brief Access function for X4Driver to mark a faulty pin
 * 
 * @return Status of execution
 */
int xt_selftest_x4_connectivity_mark_faulty_pin(
    int pin                         ///< Index of faulty pin
);

/**
 * @brief Access function for X4Driver to get the pin combinations to test
 * 
 * @return Status of execution
 */
int xt_selftest_x4_connectivity_get_pin_table(
    int* count,                     ///< Pointer to holder for the number of pins
    const int** x4_pins,            ///< Pointer to holder for the X4 pin array
    const int** mcu_pins            ///< Pointer to holder for the MCU pin array
);

//==============================================================================
// System

/**
 * @brief Causes the calling task to wait and yields to another task
 *
 * @return Status of execution
 */
int xt_wait(
    uint32_t ms_to_wait     ///< Milliseconds to wait
);

/**
 * @brief Delays the execution for the specified number of microseconds
 *
 * @return Status of execution
 */
int xt_delay_us(
    uint32_t us_to_wait     ///< Microseconds to delay
);

/**
 * @brief Triggers a software reset of MCU
 *
 * @return Status of execution
 */
int xt_software_reset(
    xt_swreset_reason_t reason  ///< Reset reason as defined in
                                ///< @ref xt_swreset_reason_t
);

/**
 * @brief Get reset count. Must be called at least once every reset to update counter.
 *
 * @return Reset count
 */
int xt_get_reset_count(void);

/**
 * @brief Gets the cause of the previous software reset
 *
 * @return Status of execution
 */
int xt_get_reset_reason(
    xt_swreset_reason_t * reason    ///< Reset reason as defined in
                                    ///< @ref xt_swreset_reason_t
);

/**
 * @brief Get crash count. 
 *
 * @return Crash count
 */
int xt_get_crash_count(void);

/**
 * @brief Resets crash count. 
 *
 * @return Status of execution
 */
int xt_reset_crash_count(void);

/**
 * @brief Sets the cause of the coming software reset
 *
 * @return Status of execution
 */
int xt_set_reset_reason(
    xt_swreset_reason_t reason  ///< Reset reason as defined in
                                ///< @ref xt_swreset_reason_t
);

/**
 * Struct with radar configuration
 */
typedef struct
{
    uint8_t external_load_caps;
    uint8_t center_frequency;
    uint8_t tx_power;
    uint8_t prf_div;
    float32_t frame_area_offset;
} xt_radar_t;

/**
 * @brief Gets default radar configuration
 * 
 * @return Status of execution
 */
int xt_get_default_radar_config(
    xt_radar_t * config  ///< Default radar configuration
);

/**
 * @brief Instructs the firmware whether to use either a crystal oscillator or external clock. 
 *
 * @return Status of execution
 */
int xt_use_external_clock(bool use);

/**
 * @brief Checks whether the crystal oscillator or external clock should be used. 
 *
 * @return True for external clock, false for crystal oscillator. 
 */
bool xt_using_external_clock(void);

/**
 * @brief Reset the watchdog timer
 *
 * @return Status of execution as defined in @ref xt_error_codes_t
 */
int xt_feed_watchdog(void);

/**
 * @brief Triggers a crashdump and reset of module. 
 *
 * @return Status of execution as defined in @ref xt_error_codes_t
 */
int xt_trigger_crash(xt_swreset_reason_t crash_reason, const char* crash_info, bool full_dump);

/**
 * @brief Gets the operation mode of the module
 *
 * @return Operation mode as defined in @ref xt_opmode_t
 */
xt_opmode_t xt_get_operation_mode(void);

/**
 * @brief Gets the system timer in microseconds
 *
 * @return System timer in microseconds
 */
uint64_t xt_get_system_timer_us(void);

/**
 * @brief Gets the program counter of the specified task
 *
 * @return Pointer to instruction
 */
void* xt_get_program_counter_from_task_handle(void* handle);

/**
 * @brief Gets the stack start pointer and depth for specified task
 *
 * @return Status of execution as defined in @ref xt_error_codes_t
 */
int xt_get_stack_area_from_task_handle(void* handle, uint32_t stack_size, void** stack_pointer, uint32_t* depth);

/**
 * @brief Gets the current average CPU load
 *
 * @return Current average CPU load, 0 means no load, 1000 means full load
 */
uint32_t xt_get_cpu_load(void);

//==============================================================================
// flash/nv-ram


/**
 * @brief Stores data in flash.
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
uint32_t xtio_flash_store(uint32_t destination_address, uint8_t * buffer, uint32_t size);

/**
 * @brief Gets the flash page size. This is the programming size. 
 *
 * @return Number of bytes in page
 */
uint32_t xtio_get_page_size(void);

/**
 * @brief Gets the flash block size. This is the erase size. 
 *
 * @return Number of bytes in block
 */
uint32_t xtio_get_block_size(void);

/**
 * @brief Gets the size of the entire flash. 
 *
 * @return Number of bytes in flash
 */
uint32_t xtio_get_flash_size(void);

//==============================================================================
// GPIO

/**
 * @brief Set direction of specified IO pin
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
int xtio_set_direction(
    xtio_pin_id_t pin_id,   ///< IO Pin ID as defined in @ref xtio_pin_id_t
    int direction,          ///< Input or output direction as defined in
                            ///< @ref xtio_pin_direction_t
    int level               ///< Output pin level as defined in
                            ///< @ref xtio_pin_level_t
);

/**
 * @brief Set the level of specified IO pin
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
int xtio_set_level(
    xtio_pin_id_t pin_id,   ///< IO Pin ID as defined in @ref xtio_pin_id_t
    int level               ///< Output pin level as defined in
                            ///< @ref xtio_pin_level_t
);

/**
 * @brief Toggle the level of specified IO pin
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
int xtio_toggle_level(
    xtio_pin_id_t pin_id    ///< IO Pin ID as defined in @ref xtio_pin_id_t
);

/**
 * @brief Get level of specified IO pin
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
int xtio_get_level(
    xtio_pin_id_t pin_id,   ///< IO Pin ID as defined in @ref xtio_pin_id_t
    xtio_pin_level_t* level ///< Input pin level as defined in
                            ///< @ref xtio_pin_level_t
);

/**
 * @brief Set mode for specified IO pin
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
int xtio_set_pin_mode(
    xtio_pin_id_t pin_id,   ///< IO Pin ID as defined in @ref xtio_pin_id_t
    uint32_t mode           ///< IO mode as defined by macroes
);

/**
 * Register interrupt callback function
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
int xtio_irq_register_callback(
    xtio_pin_id_t pin_id,         ///< IO Pin ID as defined in @ref xtio_pin_id_t
    void (*irq_callback)(void),   ///< Interrupt callback function
    xtio_interrupt_modes mode     ///< Tripper mode for IRQ
);

/**
 * Unregister interrupt handler
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
int xtio_irq_unregister_callback(
    xtio_pin_id_t pin_id          ///< IO Pin ID as defined in @ref xtio_pin_id_t
);


//==============================================================================
// LED

/**
 * @brief Initiate LED. Set MCU pin as output and set initial LED state.
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
*/
int xtio_led_init(
    xtio_led_id_t led_id,   ///< LED ID as defined in @ref xtio_led_id_t
    int mode,               ///< LED mode as defined in @ref xtio_led_mode_t
    float led_state         ///< LED state as defined in @ref xtio_led_state_t
);

/**
 * @brief Set LED state.
 *
 * If LED is set to ON/OFF mode, a value of 0 is OFF and 1 is ON.
 * If PWM mode is implemented, the brightness is controlled by a value between
 * 0.0 and 1.0 - 0.0 is OFF.
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
*/
int xtio_led_set_state(
    xtio_pin_id_t led_id,   ///< LED ID as defined in @ref xtio_led_id_t
    float led_state         ///< LED state as defined in @ref xtio_led_state_t
);

/**
 * @brief Toggle LED state.
 *
 * If LED is set to ON/OFF mode, a value of 0 is OFF and 1 is ON.
 * If LED is in PWM mode, this function will set LED state to either ON or OFF
 * depending on current state.
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
*/
int xtio_led_toggle_state(
    xtio_pin_id_t led_id    ///< LED ID as defined in @ref xtio_led_id_t
);

//==============================================================================
// External RAM

/**
 * @brief Initialize external RAM if available
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
int xtio_extram_init(void);

//==============================================================================
// UART

/**
 * @brief Configures the UART module with the specified baudrate.
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
int xtio_uart_configure(
     int baudrate       ///< Baudrate for UART communication
);


//==============================================================================
// Common serial host communication functions

/// Typedef of callback function for notifying of completed transfer
typedef void (* xtio_host_send_completed_t)(void* arg);

/**
 * Send a data buffer to host through a serial port - USB or UART
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
int xtio_host_send(
    uint8_t * buffer,                       ///< Pointer to buffer with data to send
    uint32_t length,                        ///< Length of data to send
    xtio_host_send_completed_t callback,    ///< Callback when completed
    void* callback_arg                      ///< Argument for callback
);

/**
 * Initiate serial communication - USB if available, otherwise UART
 *
 * @return Status of execution as defined in @ref xtio_error_codes_t
 */
int xtio_serial_com_init(
	xtProtocol * xt_protocol   ///< Pointer to an xtProtocol object used for parsing communication
	);

#ifdef __cplusplus
}
#endif

#endif // XT_XEP_HAL_H
