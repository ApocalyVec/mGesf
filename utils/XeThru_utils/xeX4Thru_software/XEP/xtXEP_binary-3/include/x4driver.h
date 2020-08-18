/**
 * @file
 * @brief Platform independent driver to interface with x4 radar.
 *
 */

#ifndef X4DRIVER_H
#define X4DRIVER_H

#include <stdint.h>

typedef float float32_t;



/**
 * Function pointer allowing access to x4 enable pin.
 */
typedef uint32_t (*PinSetEnableFunc)(void* user_reference, uint8_t value);

/**
 * Function pointer allowing write access to spi bus.
 */
typedef uint32_t (*SpiWriteFunc)(void* user_reference, uint8_t* data, uint32_t length);

/**
 * Function pointer allowing read to spi bus.
 */
typedef uint32_t (*SpiReadFunc)(void* user_reference, uint8_t* data, uint32_t length);

/**
 * Function pointer allowing read/write access to spi bus.
 */
typedef uint32_t (*SpiWriteReadFunc)(void* user_reference, uint8_t* wdata, uint32_t wlength, uint8_t* rdata, uint32_t rlength);

/**
 * Function pointer allowing locking of lock struct.
 */
typedef uint32_t (*LockFunc)(void* lock, uint32_t timeout);

/**
 * Function pointer allowing unlocking of lock struct.
 */
typedef void (*UnLockFunc)(void* lock);

/**
 * Function pointer allowing triggering of sweep pin.
 */
typedef uint32_t (*TriggerSweepFunc)(void* user_reference);

/**
 * Function pointer allowing waiting for a number of us.
 */
typedef void (*WaitUsFunc)(uint32_t us);

/**
 * Function pointer allowing notification when data is ready.
 */
typedef void (*NotifyDataReadyFunc)(void* user_reference);

/**
 * Function pointer allowing configuration of a timer struct.
 */
typedef uint32_t (*TimerConfigure)(void* timer , uint32_t val);

/**
 * Function pointer allowing enabling /disabling of the data_ready ISR.
 */
typedef void (*EnableDataReadyISRFunc)(void* user_reference,uint32_t enable);

/**
 * Error return codes
 */
typedef enum {
    XEP_ERROR_X4DRIVER_UNINITIALIZED                           = -1,
    XEP_ERROR_X4DRIVER_OK                                      = 0,
    XEP_ERROR_X4DRIVER_NOK                                     = 1,
    XEP_ERROR_X4DRIVER_NOT_SUPPORTED                           = 2,
    XEP_ERROR_X4DRIVER_BUSY                                    = 3,
    XEP_ERROR_X4DRIVER_PIF_TIMEOUT                             = 4,
    XEP_ERROR_X4DRIVER_OSC_LOCK_FAIL                           = 5,
    XEP_ERROR_X4DRIVER_LDO_ENABLE_FAIL                         = 6,
    XEP_ERROR_X4DRIVER_8051_VERIFY_FAIL                        = 7,
    XEP_ERROR_X4DRIVER_SRAM_FIFO_TIMEOUT_FAIL                  = 8,
    XEP_ERROR_X4DRIVER_FRAME_READY_TIMEOUT                     = 9,
    XEP_ERROR_X4DRIVER_FRAME_LENGTH_TO_LOW                     = 10,
    XEP_ERROR_X4DRIVER_BUFFER_TO_SMALL                         = 11,
    XEP_ERROR_X4DRIVER_INVALID_DACSTEP_INPUT                   = 12,
    XEP_ERROR_X4DRIVER_TIMER_PERIOD_CHANGE_FAIL                = 13,
    XEP_ERROR_X4DRIVER_UNPACK_FRAME_TO_LARGE_COUNTER           = 14,
    XEP_ERROR_X4DRIVER_DOWNCONVERTION_REGION_NOT_SUPPORTED     = 15,
    XEP_ERROR_X4DRIVER_ERROR_FRAME_AREA_TOO_LARGE              = 16,
    XEP_ERROR_X4DRIVER_ERROR_FRAME_AREA_END_OUT_OF_SCOPE       = 17,
    XEP_ERROR_X4DRIVER_PRF_DIV_TOO_SMALL                       = 18,
    XEP_ERROR_X4DRIVER_MAILBOX_ERROR                           = 19,
    XEP_ERROR_X4DRIVER_INVALID_TX_CENTER_FREQUENCY             = 20,
    XEP_ERROR_X4DRIVER_INVALID_TX_POWER_SETTING                = 21,
    XEP_ERROR_X4DRIVER_INVALID_CONFIGURATION_NOISELESS_GHOST   = 22,
    XEP_ERROR_X4DRIVER_INVALID_CONFIGURATION_RX_MFRAMES_COARSE = 23,
    XEP_ERROR_X4DRIVER_INVALID_CONFIGURATION_MAX_COUNTER       = 24,
    XEP_ERROR_X4DRIVER_INVALID_CONFIGURATION_RARE_DROP_OF_BITS = 25,
    XEP_ERROR_X4DRIVER_INVALID_CONFIGURATION_TRX_BACKEND_CLK   = 26,
    XEP_ERROR_X4DRIVER_COMMON_PLL_LOCK_FAIL                    = 27,
    XEP_ERROR_X4DRIVER_TX_PLL_LOCK_FAIL                        = 28,
    XEP_ERROR_X4DRIVER_RX_PLL_LOCK_FAIL                        = 29,
} xtx4driver_errors_t;


/**
 * Addresses of all the SPI register on X4.
 */
typedef enum {
    ADDR_SPI_FORCE_ZERO_R                      = 0,
    ADDR_SPI_FORCE_ONE_R                       = 1,
    ADDR_SPI_CHIP_ID_DIG_R                     = 2,
    ADDR_SPI_CHIP_ID_SYS_R                     = 3,
    ADDR_SPI_DEBUG_RW                          = 4,
    ADDR_SPI_RADAR_DATA_SPI_RE                 = 5,
    ADDR_SPI_RADAR_DATA_SPI_STATUS_R           = 6,
    ADDR_SPI_SPI_RADAR_DATA_CLEAR_STATUS_WE    = 7,
    ADDR_SPI_SPI_RADAR_DATA0_FIFO_STATUS_R     = 8,
    ADDR_SPI_SPI_RADAR_DATA0_CLEAR_STATUS_WE   = 9,
    ADDR_SPI_SPI_RADAR_DATA1_FIFO_STATUS_R     = 10,
    ADDR_SPI_SPI_RADAR_DATA1_CLEAR_STATUS_WE   = 11,
    ADDR_SPI_RADAR_BIST_CTRL_RW                = 12,
    ADDR_SPI_RADAR_BIST_STATUS_R               = 13,
    ADDR_SPI_FIRMWARE_VERSION_SPI_R            = 14,
    ADDR_SPI_TO_CPU_WRITE_DATA_WE              = 15,
    ADDR_SPI_SPI_MB_FIFO_STATUS_R              = 16,
    ADDR_SPI_FROM_CPU_READ_DATA_RE             = 17,
    ADDR_SPI_SPI_MB_CLEAR_STATUS_WE            = 18,
    ADDR_SPI_TO_MEM_WRITE_DATA_WE              = 19,
    ADDR_SPI_SPI_MEM_FIFO_STATUS_R             = 20,
    ADDR_SPI_FROM_MEM_READ_DATA_RE             = 21,
    ADDR_SPI_SPI_MEM_CLEAR_STATUS_WE           = 22,
    ADDR_SPI_MEM_MODE_RW                       = 23,
    ADDR_SPI_MEM_FIRST_ADDR_MSB_RW             = 24,
    ADDR_SPI_MEM_FIRST_ADDR_LSB_RW             = 25,
    ADDR_SPI_BOOT_FROM_OTP_SPI_RWE             = 26,
    ADDR_SPI_MCU_BIST_CTRL_RW                  = 27,
    ADDR_SPI_MCU_BIST_STATUS_R                 = 28,
    ADDR_SPI_SPI_CONFIG_WE                     = 29,
    ADDR_SPI_CPU_RESET_RW                      = 127,
} xtx4_spi_register_address_t;


/**
 * Addresses of all the PIF register on X4.
 */
typedef enum {
    ADDR_PIF_SP_RW                              = 1,
    ADDR_PIF_DPL_RW                             = 2,
    ADDR_PIF_DPH_RW                             = 3,
    ADDR_PIF_DPL1_RW                            = 4,
    ADDR_PIF_DPH1_RW                            = 5,
    ADDR_PIF_PCON_RW                            = 7,
    ADDR_PIF_TCON_RW                            = 8,
    ADDR_PIF_TMOD_RW                            = 9,
    ADDR_PIF_TL0_RW                             = 10,
    ADDR_PIF_TH0_RW                             = 12,
    ADDR_PIF_DPS_RW                             = 18,
    ADDR_PIF_DPC_RW                             = 19,
    ADDR_PIF_IEN2_RW                            = 26,
    ADDR_PIF_P2_RW                              = 32,
    ADDR_PIF_IEN0_RW                            = 40,
    ADDR_PIF_IP0_RW                             = 41,
    ADDR_PIF_IEN1_RW                            = 56,
    ADDR_PIF_IP1_RW                             = 57,
    ADDR_PIF_IRCON2_RW                          = 63,
    ADDR_PIF_IRCON_RW                           = 64,
    ADDR_PIF_T2CON_RW                           = 72,
    ADDR_PIF_PSW_RW                             = 80,
    ADDR_PIF_ACC_RW                             = 96,
    ADDR_PIF_B_RW                               = 112,
    ADDR_PIF_SRST_RW                            = 119,
    ADDR_PIF_FIRMWARE_VERSION_RW                = 0,
    ADDR_PIF_GPIO_OUT_RW                        = 6,
    ADDR_PIF_GPIO_IN_R                          = 11,
    ADDR_PIF_GPIO_OE_RW                         = 13,
    ADDR_PIF_RX_MFRAMES_RW                      = 14,
    ADDR_PIF_SMPL_MODE_RW                       = 15,
    ADDR_PIF_RX_DOWNCONVERSION_COEFF_I1_WE      = 16,
    ADDR_PIF_RX_DOWNCONVERSION_COEFF_I2_WE      = 17,
    ADDR_PIF_RX_DOWNCONVERSION_COEFF_Q1_WE      = 20,
    ADDR_PIF_RX_DOWNCONVERSION_COEFF_Q2_WE      = 21,
    ADDR_PIF_RX_RAM_WRITE_OFFSET_MSB_RW         = 22,
    ADDR_PIF_RX_RAM_LINE_FIRST_MSB_RW           = 23,
    ADDR_PIF_RX_RAM_LINE_LAST_MSB_RW            = 24,
    ADDR_PIF_RX_RAM_LSBS_RW                     = 25,
    ADDR_PIF_RX_COUNTER_NUM_BYTES_RW            = 27,
    ADDR_PIF_RX_COUNTER_LSB_RW                  = 28,
    ADDR_PIF_FETCH_RADAR_DATA_SPI_W             = 29,
    ADDR_PIF_FETCH_RADAR_DATA_PIF_W             = 30,
    ADDR_PIF_RADAR_DATA_PIF_RE                  = 31,
    ADDR_PIF_RADAR_DATA_PIF_STATUS_R            = 33,
    ADDR_PIF_PIF_RADAR_DATA_CLEAR_STATUS_WE     = 34,
    ADDR_PIF_PIF_RADAR_DATA0_FIFO_STATUS_R      = 35,
    ADDR_PIF_PIF_RADAR_DATA0_CLEAR_STATUS_WE    = 36,
    ADDR_PIF_PIF_RADAR_DATA1_FIFO_STATUS_R      = 37,
    ADDR_PIF_PIF_RADAR_DATA1_CLEAR_STATUS_WE    = 38,
    ADDR_PIF_RAM_SELECT_RW                      = 39,
    ADDR_PIF_RADAR_READOUT_IDLE_R               = 42,
    ADDR_PIF_RX_RESET_COUNTERS_W                = 43,
    ADDR_PIF_TRX_CLOCKS_PER_PULSE_RW            = 44,
    ADDR_PIF_RX_MFRAMES_COARSE_RW               = 45,
    ADDR_PIF_TRX_PULSES_PER_STEP_MSB_RW         = 46,
    ADDR_PIF_TRX_PULSES_PER_STEP_LSB_RW         = 47,
    ADDR_PIF_TRX_DAC_MAX_H_RW                   = 48,
    ADDR_PIF_TRX_DAC_MAX_L_RW                   = 49,
    ADDR_PIF_TRX_DAC_MIN_H_RW                   = 50,
    ADDR_PIF_TRX_DAC_MIN_L_RW                   = 51,
    ADDR_PIF_TRX_DAC_STEP_RW                    = 52,
    ADDR_PIF_TRX_ITERATIONS_RW                  = 53,
    ADDR_PIF_TRX_START_W                        = 54,
    ADDR_PIF_TRX_CTRL_DONE_R                    = 55,
    ADDR_PIF_TRX_BACKEND_DONE_R                 = 58,
    ADDR_PIF_TRX_CTRL_MODE_RW                   = 59,
    ADDR_PIF_TRX_LFSR_TAPS_0_RW                 = 60,
    ADDR_PIF_TRX_LFSR_TAPS_1_RW                 = 61,
    ADDR_PIF_TRX_LFSR_TAPS_2_RW                 = 62,
    ADDR_PIF_TRX_LFSR_RESET_WS                  = 65,
    ADDR_PIF_RX_WAIT_RW                         = 66,
    ADDR_PIF_TX_WAIT_RW                         = 67,
    ADDR_PIF_TRX_DAC_OVERRIDE_H_RW              = 68,
    ADDR_PIF_TRX_DAC_OVERRIDE_L_RW              = 69,
    ADDR_PIF_TRX_DAC_OVERRIDE_LOAD_WS           = 70,
    ADDR_PIF_CPU_SPI_MASTER_CLK_CTRL_RW         = 71,
    ADDR_PIF_MCLK_TRX_BACKEND_CLK_CTRL_RW       = 73,
    ADDR_PIF_OSC_CTRL_RW                        = 74,
    ADDR_PIF_IO_CTRL_1_RW                       = 75,
    ADDR_PIF_IO_CTRL_2_RW                       = 76,
    ADDR_PIF_IO_CTRL_3_RW                       = 77,
    ADDR_PIF_IO_CTRL_4_RW                       = 78,
    ADDR_PIF_IO_CTRL_5_RW                       = 79,
    ADDR_PIF_IO_CTRL_6_RW                       = 81,
    ADDR_PIF_PIF_MB_FIFO_STATUS_R               = 82,
    ADDR_PIF_TO_CPU_READ_DATA_RE                = 83,
    ADDR_PIF_FROM_CPU_WRITE_DATA_WE             = 84,
    ADDR_PIF_PIF_MB_CLEAR_STATUS_WE             = 85,
    ADDR_PIF_PIF_MEM_FIFO_STATUS_R              = 86,
    ADDR_PIF_PIF_MEM_CLEAR_STATUS_WE            = 87,
    ADDR_PIF_SPI_MASTER_SEND_RW                 = 88,
    ADDR_PIF_SPI_MASTER_RADAR_BURST_KICK_W      = 89,
    ADDR_PIF_SPI_MASTER_IDLE_R                  = 90,
    ADDR_PIF_SPI_MASTER_MODE_RW                 = 91,
    ADDR_PIF_SPI_MASTER_RADAR_BURST_SIZE_LSB_RW = 92,
    ADDR_PIF_OTP_CTRL_RW                        = 93,
    ADDR_PIF_BOOT_FROM_OTP_PIF_RWE              = 94,
    ADDR_PIF_RX_PLL_CTRL_1_RW                   = 95,
    ADDR_PIF_RX_PLL_CTRL_2_RW                   = 97,
    ADDR_PIF_RX_PLL_SKEW_CTRL_RW                = 98,
    ADDR_PIF_RX_PLL_SKEWCALIN_RW                = 99,
    ADDR_PIF_RX_PLL_STATUS_R                    = 100,
    ADDR_PIF_TX_PLL_CTRL_1_RW                   = 101,
    ADDR_PIF_TX_PLL_CTRL_2_RW                   = 102,
    ADDR_PIF_TX_PLL_SKEW_CTRL_RW                = 103,
    ADDR_PIF_TX_PLL_SKEWCALIN_RW                = 104,
    ADDR_PIF_TX_PLL_STATUS_R                    = 105,
    ADDR_PIF_COMMON_PLL_CTRL_1_RW               = 106,
    ADDR_PIF_COMMON_PLL_CTRL_2_RW               = 107,
    ADDR_PIF_COMMON_PLL_CTRL_3_RW               = 108,
    ADDR_PIF_COMMON_PLL_CTRL_4_RW               = 109,
    ADDR_PIF_COMMON_PLL_FRAC_2_RW               = 110,
    ADDR_PIF_COMMON_PLL_FRAC_1_RW               = 111,
    ADDR_PIF_COMMON_PLL_FRAC_0_RW               = 113,
    ADDR_PIF_COMMON_PLL_STATUS_R                = 114,
    ADDR_PIF_CLKOUT_SEL_RW                      = 115,
    ADDR_PIF_APC_DVDD_TESTMODE_RW               = 116,
    ADDR_PIF_MISC_CTRL                          = 117,
    ADDR_PIF_DVDD_RX_CTRL_RW                    = 118,
    ADDR_PIF_DVDD_TX_CTRL_RW                    = 120,
    ADDR_PIF_DVDD_TESTMODE_RW                   = 121,
    ADDR_PIF_AVDD_RX_CTRL_RW                    = 122,
    ADDR_PIF_AVDD_TX_CTRL_RW                    = 123,
    ADDR_PIF_AVDD_TESTMODE_RW                   = 124,
    ADDR_PIF_LDO_STATUS_1_R                     = 125,
    ADDR_PIF_LDO_STATUS_2_R                     = 126,
    ADDR_PIF_SPI_CONFIG_PIF_RWE                 = 127,
} xtx4_pif_register_address_t;


/**
 * Addresses of all the XIF register on X4.
 */
typedef enum {
    ADDR_XIF_DEBUG_RW                           = 0,
    ADDR_XIF_SAMPLER_PRESET_MSB_RW              = 1,
    ADDR_XIF_SAMPLER_PRESET_LSB_RW              = 2,
    ADDR_XIF_DAC_TRIM_RW                        = 3,
    ADDR_XIF_PREAMP_TRIM_RW                     = 4,
    ADDR_XIF_RX_FE_ANATESTREQ_RW                = 5,
    ADDR_XIF_LNA_ANATESTREQ_RW                  = 6,
    ADDR_XIF_DAC_ANATESTREQ_RW                  = 7,
    ADDR_XIF_VREF_TRIM_RW                       = 8,
    ADDR_XIF_IREF_TRIM_RW                       = 9,
    ADDR_XIF__APC_TEMP_TRIM_RW                  = 10,
} xtx4_xif_register_address_t;


/**
 * DAC steps for X4.
 */
typedef enum {
    DAC_STEP_1                                  = 0,
    DAC_STEP_2                                  = 1,
    DAC_STEP_4                                  = 2,
    DAC_STEP_8                                  = 3,
} xtx4_dac_step_t;


/**
 * Sweep trigger modes for X4.
 */
typedef enum {
    SWEEP_TRIGGER_MANUAL                        = 0,
    SWEEP_TRIGGER_X4                            = 1,
    SWEEP_TRIGGER_MCU                           = 2,
} xtx4_sweep_trigger_control_mode_t;


/**
 * TX center frequencies for X4.
 */
typedef enum {
    TX_CENTER_FREQUENCY_MIN                     = 3,
    TX_CENTER_FREQUENCY_EU_7_290GHz             = 3,
    TX_CENTER_FREQUENCY_KCC_8_748GHz            = 4,
    TX_CENTER_FREQUENCY_MAX                     = 4,
} xtx4_tx_center_frequency_t;

/**
 * TX power values for X4.
 */
typedef enum {
    TX_POWER_OFF                                = 0,
    TX_POWER_LOW                                = 1,
    TX_POWER_MEDIUM                             = 2,
    TX_POWER_HIGH                               = 3,
} xtx4_tx_power_t;


/**
 * X4driver actions for timer driven operations.
 */
typedef enum {
    X4DRIVER_ACTION_NONE                            = 0,
    X4DRIVER_ACTION_CHECK_FRAME_READY               = 1<<0,
} xtx4_x4driver_action_t;


/**
 * X4driver ldo enums.
 */
typedef enum {
    X4DRIVER_LDO_DVDD_RX                            = 1<<0,
    X4DRIVER_LDO_DVDD_TX                            = 1<<1,
    X4DRIVER_LDO_AVDD_RX                            = 1<<2,
    X4DRIVER_LDO_AVDD_TX                            = 1<<3,
    X4DRIVER_LDO_ALL                                = 0x0000000f,
} xtx4_x4driver_ldo_t;

/**
 * Collection of callbacks, used to configure interact with the X4 chip.
 */
typedef struct
{
    PinSetEnableFunc pin_set_enable;
    SpiWriteFunc spi_write;
    SpiReadFunc spi_read;
    SpiWriteReadFunc spi_write_read;
    WaitUsFunc wait_us;
    NotifyDataReadyFunc notify_data_ready;
    TriggerSweepFunc trigger_sweep;
    EnableDataReadyISRFunc enable_data_ready_isr;
} X4DriverCallbacks_t;





/**
 * Abstract lock struct to allow for platform independent configuration of Locks.
 */
typedef struct
{
    LockFunc lock;
    UnLockFunc unlock;
    void * object;
    uint32_t timeout;
} X4DriverLock_t;


/**
 * Abstract timer struct to allow for platform independent configuration of Timers.
 */
typedef struct
{
    TimerConfigure configure;
    float32_t configured_frequency;
    void * object;
} X4DriverTimer_t;

/**
 * Contains all of the private variables for the X4Driver.
 */
typedef struct
{
    void* user_reference;
    uint32_t initialized;
    X4DriverLock_t lock;
    X4DriverTimer_t sweep_timer;
    X4DriverTimer_t action_timer;
    X4DriverCallbacks_t callbacks;
	uint32_t zero_frame_counter;
    uint32_t frame_counter;
    uint32_t frame_length;
    uint32_t frame_read_size;
    uint32_t frame_is_ready_strategy;
    float32_t configured_fps;
    uint32_t sweep_trigger_strategy;
    uint32_t next_action;
    xtx4_sweep_trigger_control_mode_t trigger_mode;
    uint32_t dac_max;
    uint32_t dac_min;
    xtx4_dac_step_t dac_step;
    uint32_t iterations;
    uint32_t pulses_per_step;
    float32_t normalization_offset;
    float32_t normalization_nfactor;
    float32_t filter_normalization_factor;
    uint32_t bytes_per_counter;
    xtx4_tx_center_frequency_t center_frequency;
    uint8_t downconversion_enabled;
    uint8_t iq_separate;
    uint8_t rx_wait_offset;
    uint8_t rx_wait;
    uint32_t required_bins_active;
    uint32_t rx_mframes;
    uint32_t rx_mframes_coarse;
    float32_t rx_wait_offset_m;
    uint32_t frame_area_offset_bins;
    float32_t frame_area_offset_meters;
    uint8_t* spi_buffer;
    uint32_t spi_buffer_size;
    float32_t frame_area_start;
    float32_t frame_area_end;
    float32_t frame_area_start_requested;
    float32_t frame_area_end_requested;
    uint32_t frame_area_start_ram_line;
    uint32_t frame_area_start_bin_requested;
    uint32_t frame_area_start_ram_line_bin;
    uint32_t frame_area_start_bin_offset;
    uint32_t frame_area_end_ram_line;
    uint32_t frame_area_end_bin_requested;
    uint32_t frame_area_end_ram_line_bin;
    uint32_t frame_area_end_bin_offset;
    uint8_t ram_select;
    uint8_t external_load_caps;
    
/**
 *  Set of coefficients for down conversion for I and Q.
    The coefficients cannot be read back from X4.
    Should be written with most significant coefficient first.
 */
    const int8_t *downconversion_coeff_i1;
    const int8_t *downconversion_coeff_i2;
    const int8_t *downconversion_coeff_q1;
    const int8_t *downconversion_coeff_q2;
    
    int8_t downconversion_coeff_custom_q1[32];
    int8_t downconversion_coeff_custom_i1[32];
    int8_t downconversion_coeff_custom_q2[32];
    int8_t downconversion_coeff_custom_i2[32];

} X4Driver_t;

#define X4DRIVER_MAX_ALLOWED_ZERO_FRAMES 100

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Read back embedded 8051 firmware.
 * Assumes Enable has been set.
 * @return Status of execution as defined in x4driver.h.
 */
int x4driver_verify_firmware(X4Driver_t* x4driver, const uint8_t * buffer, uint32_t size);


/**
 * @brief Gets size of X4Driver struct.
 * @return size of x4driver struct.
 */
int x4driver_get_instance_size(void);


/**
 * @brief Creates x4driver instance. Initiates data structures with predefined values.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_create(X4Driver_t** x4driver, void* instance_memory, X4DriverCallbacks_t* x4driver_callbacks,X4DriverLock_t *lock,X4DriverTimer_t *timer,X4DriverTimer_t *timer_action, void* user_reference);



/**
 * @brief Set frame area zone Assume air as transmitter medium.
 * Start and end in meter.
 * x4driver_set_frame_area will configure chip to convert specified zone by adjusting rxwait and rxframes.
 * Will then select the part of the frame to read.
 * @return Status of execution as defined in x4driver.h.
 */
int x4driver_set_frame_area(X4Driver_t* x4driver, float32_t start, float32_t end);


/**
 * @brief Calibrated frame offset in meters.
 * Rx_wait is adjusted to get a as close to 0 m in rx_wait resolution.
 * @return Status of execution as defined in x4driver.h,
 */
int x4driver_set_frame_area_offset(X4Driver_t* x4driver, float32_t offset_in_meters);


/**
 * @brief Sets frame rate for frame streaming.
 * @return Status of execution as defined in x4driver.h.
 */
int x4driver_set_fps(X4Driver_t* x4driver, float32_t fps);


/**
 * @brief Reads frame and normalizes that frame.
 * @return Status of execution as defined in x4driver.h,
 */
int x4driver_read_frame_normalized(X4Driver_t* x4driver, uint32_t* frame_counter, float32_t* data, uint32_t length);


/**
 * @brief Gets current frame length.
 * @return Status of execution as defined in x4driver.h,
 */
int x4driver_get_frame_length(X4Driver_t* x4driver, uint32_t * cycles);


/**
 * @brief Set enable for X4
 *
 * @return Status of execution
 */
int x4driver_set_enable(X4Driver_t* x4driver, uint8_t value);


/**
 * @brief Set SPI register on radar chip.
 *
 * @return Status of execution
 */
int x4driver_set_spi_register(X4Driver_t* x4driver,uint8_t address, uint8_t value);


/**
 * @brief Get SPI register on radar chip.
 *
 * @return Status of execution
 */
int x4driver_get_spi_register(X4Driver_t* x4driver,uint8_t address, uint8_t * value);

/**
 * @brief Writes data to SPI register on radar chip.
 *
 * @return Status of execution
 */
int x4driver_write_to_spi_register(X4Driver_t* x4driver, uint8_t address, const uint8_t* values, uint32_t length);

/**
 * @brief Reads data from SPI register on radar chip.
 *
 * @return Status of execution
 */
int x4driver_read_from_spi_register(X4Driver_t* x4driver, uint8_t address, uint8_t * values, uint32_t length);

/**
 * @brief Uploads default 8051 firmware to X4.
 * Assumes Enable has been set.
 * @return Status of execution as defined in x4driver.h.
 */
int x4driver_upload_firmware_default(X4Driver_t* x4driver);


/**
 * @brief Upload custom 8051 firmware to X4.
 * Assumes Enable has been set.
 * @return Status of execution as defined in x4driver.h.
 */
int x4driver_upload_firmware_custom(X4Driver_t* x4driver, const uint8_t * buffer,uint32_t lenght);


/**
 * @brief Gets PIF register value.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_pif_register(X4Driver_t* x4driver, uint8_t address,uint8_t * value);


/**
 * @brief Gets XIF register value.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_xif_register(X4Driver_t* x4driver, uint8_t address,uint8_t * value);


/**
 * @brief Sets PIF register value.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_pif_register(X4Driver_t* x4driver, uint8_t address, uint8_t value);

/**
 * @brief Sets XIF register value.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_xif_register(X4Driver_t* x4driver, uint8_t address, uint8_t value);


/**
 * @brief setup external osc on X4.
 * requires enable to be set and 8051 SRAM to be program.
 * @return XEP_ERROR_X4DRIVER_OSC_LOCK_FAIL if osc lock fails
 */
int x4driver_init_clock(X4Driver_t* x4driver);


/**
 * @brief Enables dvdd_tx,dvdd_rx, avdd_tx dvdd_rx ldos on X4.
 * requires enable to be set and 8051 SRAM to be program.
 * @return XEP_ERROR_X4DRIVER_LDO_ENABLE_FAIL if one of the ldos fails to enable.
 */
int x4driver_ldo_enable_all(X4Driver_t* x4driver);


/**
 * @brief Allows enabling of dvdd_tx,dvdd_rx, avdd_tx dvdd_rx ldos on X4.
 * requires enable to be set and 8051 SRAM to be program.
 * @return XEP_ERROR_X4DRIVER_LDO_ENABLE_FAIL if one of the ldos fails to enable.
 */
int x4driver_ldo_enable(X4Driver_t* x4driver,uint32_t ldo);


/**
 * @brief Will check if frame is ready.
 * Assumes Enable has been set.
 * Assumes X4 firmware has been programmed.
 * @return Status of execution as defined in x4driver.h.
 */
int x4driver_is_frame_ready(X4Driver_t* x4driver,uint8_t * is_ready);


/**
 * @brief Will trigger a radar sweep.
 * Assumes Enable has been set.
 * Assumes X4 firmware has been programmed.
 * @return Status of execution as defined in x4driver.h.
 */
int x4driver_start_sweep(X4Driver_t* x4driver);


/**
 * @brief Initiates x4driver.
 * Will make sure that enable is set, 8051 SRAM is programmed, ldos are enabled, and that the external oscillator has been enabled.
 * @return XEP_ERROR_X4DRIVER_OK in case of success. XEP_ERROR_X4DRIVER_BUSY,XEP_ERROR_X4DRIVER_PIF_TIMEOUT,XEP_ERROR_X4DRIVER_OSC_LOCK_FAIL or XEP_ERROR_X4DRIVER_LDO_ENABLE_FAIL in case of errors.
 */
int x4driver_init(X4Driver_t* x4driver);


/** @brief Will setup x4 in default configuration.
 * Assumes Enable has been set.
 * Assumes X4 firmware has been programmed.
 * @return Status of execution as defined in x4driver.h.
 */
int x4driver_setup_default(X4Driver_t* x4driver);


/**
 *@brief Set frame length in cycles. Cycles are in rx microframes.
 * @return Status of execution
 */
int x4driver_set_frame_length(X4Driver_t* x4driver,uint8_t cycles);


/**
 * @brief Reads raw frame
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_read_frame_bytes(X4Driver_t* x4driver, uint32_t* frame_counter, uint8_t* data, uint32_t length);


/**
 * @brief Gets Iterations.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_iterations(X4Driver_t* x4driver, uint8_t * iterations);


/**
 * @brief Sets Iterations.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_iterations(X4Driver_t* x4driver, uint8_t iterations);


/**
 * @brief Gets pulses per step.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_pulses_per_step(X4Driver_t* x4driver, uint16_t * pps);


/**
 * @brief Sets pulses per step.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_pulses_per_step(X4Driver_t* x4driver, uint16_t  pps);


/**
 * @brief Sets a register segment. Takes mask, will shift data to fit segment.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_pif_segment(X4Driver_t* x4driver,uint8_t segment_address,uint8_t mask , uint8_t segment_value);


/**
 * @brief Gets a register segment. Takes mask, will shift data to fit segment.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_pif_segment(X4Driver_t* x4driver,uint8_t segment_address,uint8_t mask, uint8_t * segment_value);


/*
 * @brief Sets trx_dac_step_clog2 segment of TRX_DAC_STEP register.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_dac_step(X4Driver_t* x4driver, xtx4_dac_step_t  dac_step);


/*
 * @brief Gets trx_dac_step_clog2 segment of TRX_DAC_STEP register.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_dac_step(X4Driver_t* x4driver, xtx4_dac_step_t * dac_step);


/*
 * @brief Gets DAC min.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_dac_min(X4Driver_t* x4driver, uint16_t * dac_min);


/*
 * @brief Sets DAC min.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_dac_min(X4Driver_t* x4driver, uint16_t dac_min);


 /*
 * @brief Gets DAC max.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_dac_max(X4Driver_t* x4driver, uint16_t * dac_max);


/*
 * @brief Sets DAC max.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_dac_max(X4Driver_t* x4driver, uint16_t dac_max);


/*
 * @brief Gets RX wait.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_rx_wait(X4Driver_t* x4driver, uint8_t * rx_wait);

/*
 * @brief Sets RX wait.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_rx_wait(X4Driver_t* x4driver, uint8_t rx_wait);

/*
 * @brief Sets sweep trigger control.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_sweep_trigger_control(X4Driver_t* x4driver, xtx4_sweep_trigger_control_mode_t mode);


/*
 * @brief Gets configured fps.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_fps(X4Driver_t* x4driver, float32_t * fps);


/*
 * @brief Gets calculated fps i.e. for a software timer running timer ticks on ms resolution it will give the configured fps from the timer.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_calculated_fps(X4Driver_t* x4driver, float32_t * fps);


/*
 * @brief Sets down convention
 * requires enable to be set and 8051 SRAM to be programed.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_downconversion(X4Driver_t* x4driver, uint8_t enable);


/*
 * @brief Gets down convention status
 * requires enable to be set and 8051 SRAM to be programed.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_downconversion(X4Driver_t* x4driver, uint8_t * enable);


/*
 * @brief Gets Number of bins
 * requires enable to be set and 8051 SRAM to be programed.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_frame_bin_count(X4Driver_t* x4driver, uint32_t * bins);


/*
 * @brief Sets Pulse Repetition Frequency(PRF) divider
 * requires enable to be set and 8051 SRAM to be programed.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_prf_div(X4Driver_t* x4driver, uint8_t  prf_div);


/*
 * @brief Gets Pulse Repetition Frequency(PRF) divider
 * requires enable to be set and 8051 SRAM to be programed.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_prf_div(X4Driver_t* x4driver, uint8_t * prf_div);


/*
 * @brief Gets the TX center frequency
 * requires enable to be set and 8051 SRAM to be programed.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_tx_center_frequency(X4Driver_t* x4driver, xtx4_tx_center_frequency_t * tx_center_frequency);


/*
 * @brief Sets the TX center frequency
 * requires enable to be set and 8051 SRAM to be programed.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_tx_center_frequency(X4Driver_t* x4driver, xtx4_tx_center_frequency_t tx_center_frequency);


/*
 * @brief Sets the TX output power
 * requires enable to be set and 8051 SRAM to be programed.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_tx_power(X4Driver_t* x4driver, xtx4_tx_power_t tx_power);



/*
 * @brief Gets the TX output power
 * requires enable to be set and 8051 SRAM to be programed.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_tx_power(X4Driver_t* x4driver, xtx4_tx_power_t *tx_power);

/*
 * @brief Gets the length between data bins.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_bin_length(X4Driver_t* x4driver, float32_t * bin_length);


/*
 * @brief Gets sweeep time.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_sweep_time(X4Driver_t* x4driver, float32_t * sweep_time);


/**
 * @brief Get frame area
 * Start and end in meter.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_frame_area(X4Driver_t* x4driver, float32_t *start, float32_t *end);

/*
 * @brief Gets sampler frequency of 23.328 Ghz when RF mode is selected and  23.328 Ghz/8 when down conversion is selected.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_sampler_frequency(X4Driver_t* x4driver, float32_t * frequency);

/*
 * @brief Checks configuration.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_check_configuration(X4Driver_t* x4driver);


/*
 *@brief Gets the filter coefficients for X4.
 *
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_filter_coefficients(X4Driver_t *x4driver, uint8_t *buffer_q1, uint8_t *buffer_i1, uint8_t *buffer_q2, uint8_t *buffer_i2, uint8_t size);

/*
 *@brief Sets the filter coefficients for X4.
 *
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_filter_coefficients(X4Driver_t *x4driver, uint8_t *buffer_q1, uint8_t *buffer_i1, uint8_t *buffer_q2, uint8_t *buffer_i2, uint8_t size);


/*
 *@brief Handles events in x4driver when using timer driven radar triggering.
 *
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_on_action_event(X4Driver_t* x4driver);

/*
 * @brief Sets DAC
 * values automatically.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_dac_auto(X4Driver_t* x4driver,uint32_t daclength);

/*
 * @brief Gets RF sampler frequency of  23.328 Ghz.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_get_sampler_frequency_rf(X4Driver_t* x4driver, float32_t * rf_frequency);

/**
 * @brief Gets calculated frame offset in meters done by x4driver_set_frame_area_offset.
 * @return Status of execution as defined in x4driver.h.
 */
int x4driver_get_frame_area_offset(X4Driver_t* x4driver, float32_t * offset_in_meters);


/*
 * @brief Sets frame trigger period in 1/10000 second increments. 32 bit value.
 * requires enable to be set and 8051 SRAM to be program.
 * @return Status of execution as defined in x4driver.h
 */
int x4driver_set_frame_trigger_period(X4Driver_t* x4driver, uint32_t period);

#ifdef __cplusplus
}
#endif


#endif // X4DRIVER_H

