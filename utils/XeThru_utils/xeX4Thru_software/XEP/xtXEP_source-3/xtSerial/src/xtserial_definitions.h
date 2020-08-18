#ifndef XTSERIAL_DEFINITIONS_H_
#define XTSERIAL_DEFINITIONS_H_

#include <inttypes.h>
#include <stdbool.h>

#include "xtid.h"

/* DEPRECATED! Internal codes not to be used. */
#define DEPRECATED_XTS_SPR_APPDATA_FAST 0x51
/* ------------------------------------------ */

#define XTS_DEF_ENTER_BOOTLOADER_KEY	(0xa2b95ef0)
#define XTS_DEF_PINGVAL					(0xeeaaeaae)
#define XTS_DEF_PONGVAL_READY			(0xaaeeaeea)
#define XTS_DEF_PONGVAL_NOTREADY		(0xaeeaeeaa)
#define XTS_DEF_PONGVAL_SAFEMODE        (0xffeefeef)
#define XTS_DEF_ENTER_BOOTLOADER_KEY     (0xa2b95ef0)
#define XTS_DEF_ENTER_BOOTLOADER_KEY_2   (0xa2b96ef1)

/* **************************************** */



// Primary: Routed to Primary task, typically before app is started.
// Secondary: Routed to Secondary task, typically when app is running.
#define XTS_SPA_VSDM_GET_STATUS			0xa0
#define XTS_SPA_COMMON_GET_SPECTRUM		0x90

#define XTS_USER_CONTENTID_COMFIGMSG        (1)
#define XTS_USER_CONTENTID_RADARSETTINGS    (3)
#define XTS_USER_CONTENTID_SYSTEM_STATUS    (5)

/* **************************************** */

typedef enum serial_protocol_command_en
{
    XTS_SPC_RESERVED            = 0,
    XTS_SPC_PING                = 0x01,
    XTS_SPC_START_BOOTLOADER    = 0x02,
    XTS_SPC_APPCOMMAND          = 0x10,
    XTS_SPC_MOD_SETMODE         = 0x20,
    XTS_SPC_MOD_LOADAPP         = 0x21,
    XTS_SPC_MOD_RESET           = 0x22,
    XTS_SPC_MOD_SETCOM          = 0x23,
    XTS_SPC_MOD_SETLEDCONTROL   = 0x24,
    XTS_SPC_MOD_NOISEMAP        = 0x25,
    XTS_SPC_MOD_GETMODE         = 0x26,    
    XTS_SPC_IOPIN               = 0x40,
    XTS_SPC_OUTPUT              = 0x41,
    XTS_SPC_DEBUG_OUTPUT        = 0x42,
    XTS_SPC_X4DRIVER            = 0x50,
    XTS_SPC_DIR_COMMAND         = 0x90,
    XTS_SPC_HIL                 = 0xa0,
    XTS_SPC_DEBUG_LEVEL         = 0xb0,
    XTS_SPC_BOOTLOADER          = 0xc0
} serial_protocol_command_t;

typedef enum serial_protocol_appcommand_en
{
    XTS_SPCA_RESERVED       = 0,
    XTS_SPCA_SET            = 0x10,
    XTS_SPCA_GET            = 0x11,
    XTS_SPCA_FACTORY_PRESET = 0x12,
    XTS_SPCA_STORE_NOISEMAP = 0x13,
    XTS_SPCA_LOAD_NOISEMAP  = 0x14,
    XTS_SPCA_GET_PARAMETER_FILE = 0x15,
    XTS_SPCA_DELETE_NOISEMAP = 0x16,
    XTS_SPCA_POLL           = 0x20
} serial_protocol_appcommand_t;

typedef enum serial_protocol_bootloader_command_en
{
    XTS_SPCB_GET_DEVICE_INFO = 0x00,
    XTS_SPCB_WRITE_PAGE      = 0x10,
    XTS_SPCB_START_APP       = 0x20
} serial_protocol_bootloader_command_t;

typedef enum serial_protocol_response_en
{
    XTS_SPR_RESERVED            = 0,
    XTS_SPR_PONG                = 0x01,
    XTS_SPR_ACK                 = 0x10,
    XTS_SPR_REPLY               = 0x11,
    XTS_SPR_HIL                 = 0x12,
    XTS_SPR_ERROR               = 0x20,
    XTS_SPR_SYSTEM              = 0x30,
    XTS_SPR_APPDATA             = 0x50,
    /* DEPRECATED. XTS_SPR_APPDATA_FAST       	= 0x51, */
    XTS_SPR_DATA                = 0xa0
} serial_protocol_response_t;

typedef enum serial_protocol_response_errorcode_en
{
    XTS_SPRE_RESERVED           = 0,
    XTS_SPRE_NOT_RECOGNIZED     = XTID_MCP_ERROR_NOT_RECOGNIZED,
    XTS_SPRE_CRC_FAILED         = 0x02,
    XTS_SPRE_BUSY,
    XTS_SPRE_APP_INVALID        = 0x20,
    XTS_SPRE_COMMAND_FAILED     = 0x21,
    XTS_SPRE_RESTARTING         = 0x22,
    XTS_SPRE_USER               = 0xa0
} serial_protocol_response_errorcode_t;

typedef enum serial_protocol_response_systemcode_en
{
    XTS_SPRS_RESERVED           = 0,
    XTS_SPRS_BOOTING            = 0x10,
    XTS_SPRS_READY              = 0x11
} serial_protocol_response_systemcode_t;

typedef enum sensor_direct_command_en
{
    XTS_SDC_RESERVED                    = 0,
    // Messages routed to radar task.
    XTS_SDC_RADAR_FIRST                 = 0x10,
    XTS_SDC_RADLIB_EXECUTE              = 0x10,
    XTS_SDC_RADLIB_SETVARINT            = 0x11,
    XTS_SDC_RADLIB_GETVARINT            = 0x12,
    XTS_SDC_RADLIB_SETVARFLOAT          = 0x13,
    XTS_SDC_RADLIB_GETVARFLOAT          = 0x14,
    XTS_SDC_RADLIB_SETREGFIELD          = 0x15,
    XTS_SDC_RADLIB_GETREGFIELD          = 0x16,
    XTS_SDC_RADLIB_GETFRAMEFLOAT        = 0x17,
    XTS_SDC_RADLIB_GETFRAMERAW          = 0x18,
    XTS_SDC_RADLIB_GETCHIPID            = 0x19,
    XTS_SDC_RADLIB_TIMINGMEASUREMENT    = 0x20,
    XTS_SDC_RADHW_RESET                 = 0x30,
    XTS_SDC_RADHW_CLOCK                 = 0x31,
    XTS_SDC_RADMSC_DOFRAMETOFLASH       = 0x40,
    XTS_SDC_RADAR_LAST                  = 0x40,

    // Messages routed to system task.
    XTS_SDC_SYSTEM_FIRST            = 0x50,
    XTS_SDC_SYSTEM_TEST             = 0x50,
    XTS_SDC_SYSCFG_SETINT           = 0x51,
    XTS_SDC_SYSCFG_SETFLOAT         = 0x52,
    XTS_SDC_SYSFLASH_GETBLOCK       = 0x53,
    XTS_SDC_SYSFLASH_SETBLOCK       = 0x54,
    XTS_SDC_SYSTEM_GET_VERSION      = 0x55,
    XTS_SDC_SYSTEM_GET_BUILD        = 0x56,
    XTS_SDC_SYSTEM_GET_PRODUCT      = 0x57,
    XTS_SDC_SYSTEM_GET_INFO         = 0x58,
    XTS_SDC_SYSTEM_GET_DEBUG_DATA   = 0x59,
    XTS_SDC_GET_PARAMETER_FILE      = 0x60,
    XTS_SDC_GET_CRASH_DUMP_LENGTH   = 0x61,
    XTS_SDC_CLEAR_CRASH_DUMP        = 0x62,
    XTS_SDC_GET_CRASH_DUMP          = 0x63,
    XTS_SDC_SEARCH_FILE_TYPE        = 0x64,
    XTS_SDC_FIND_ALL_FILES          = 0x65,
    XTS_SDC_CREATE_NEW_FILE         = 0x66,
    XTS_SDC_SET_FILE_DATA           = 0x67,
    XTS_SDC_CLOSE_FILE              = 0x68,
    XTS_SDC_GET_FILE_LENGTH         = 0x69,
    XTS_SDC_DELETE_FILE             = 0x70,
    XTS_SDC_GET_FILE_DATA           = 0x71,
    XTS_SDC_OPEN_FILE               = 0x72,
    XTS_SDC_FORMAT_FILESYSTEM       = 0x73,
    XTS_SDC_GET_PROFILEID           = 0x74,
    XTS_SDC_INJECT_FRAME            = 0x75,
    XTS_SDC_PREPARE_INJECT_FRAME    = 0x76,    
    XTS_SDC_SYSTEM_LAST,

    // Messages routed to processing task.
    XTS_SDC_APP_FIRST           = 0x70,
    XTS_SDC_APP_SETFLOAT        = 0x70,
    XTS_SDC_APP_SETINT          = 0x71,
    XTS_SDC_APP_LAST            = 0x71,

    // Messages routed to communication task.
    XTS_SDC_COMM_FIRST          = 0x80,
    XTS_SDC_COMM_SETBAUDRATE    = 0x80,
    XTS_SDC_COMM_LAST           = 0x80

} sensor_direct_command_t;

typedef enum serial_protocol_command_iopin_en
{
    XTS_SPCIOP_RESERVED            = 0,
    XTS_SPCIOP_SETCONTROL          = 0x10,
    XTS_SPCIOP_GETCONTROL          = 0x11,
    XTS_SPCIOP_SETVALUE            = 0x20,
    XTS_SPCIOP_GETVALUE            = 0x21,
} serial_protocol_command_iopin_t;

typedef enum serial_protocol_command_output_en
{
    XTS_SPCO_RESERVED            = 0,
    XTS_SPCO_SETCONTROL          = 0x10,
    XTS_SPCO_GETCONTROL          = 0x11,
} serial_protocol_command_output_t;

typedef enum serial_protocol_command_noisemap_en
{
    XTS_SPCN_RESERVED            = 0,
    XTS_SPCN_SETCONTROL          = 0x10,
    XTS_SPCN_GETCONTROL          = 0x11,
} serial_protocol_command_noisemap_t;

typedef enum serial_protocol_command_x4driver_en
{
    XTS_SPCX_RESERVED            = 0,
    XTS_SPCX_SET                 = 0x10,
    XTS_SPCX_GET                 = 0x11,
    XTS_SPCX_WRITE               = 0x12,
    XTS_SPCX_READ                = 0x13,
    XTS_SPCX_INIT                = 0x20,
} serial_protocol_command_x4driver_t;

typedef enum serial_protocol_command_x4driver_id_en
{
    XTS_SPCXI_RESERVED            = 0,
    XTS_SPCXI_FPS                 = 0x10,
    XTS_SPCXI_PULSESPERSTEP       = 0x11,
    XTS_SPCXI_ITERATIONS          = 0x12,
    XTS_SPCXI_DOWNCONVERSION      = 0x13,
    XTS_SPCXI_FRAMEAREA           = 0x14,
    XTS_SPCXI_DACSTEP             = 0x15,
    XTS_SPCXI_DACMIN              = 0x16,
    XTS_SPCXI_DACMAX              = 0x17,
    XTS_SPCXI_FRAMEAREAOFFSET     = 0x18,
    XTS_SPCXI_ENABLE              = 0x19,
    XTS_SPCXI_TXCENTERFREQUENCY   = 0x20,
    XTS_SPCXI_TXPOWER             = 0x21,
    XTS_SPCXI_SPIREGISTER         = 0x22,
    XTS_SPCXI_PIFREGISTER         = 0x23,
    XTS_SPCXI_XIFREGISTER         = 0x24,
    XTS_SPCXI_PRFDIV              = 0x25,
    XTS_SPCXI_FRAMEBINCOUNT       = 0x26,
} serial_protocol_command_x4driver_id_t;


typedef enum serial_protocol_hil_en
{
    XTS_SPH_RESERVED            = 0,
    XTS_SPH_CMD_CONTINUE        = 0x10,
    XTS_SPH_SEND_FLOAT          = 0x20
} serial_protocol_hil_t;

typedef enum sensor_system_config_reference_en
{
    XTS_SSCR_RESERVED           = 0,
    XTS_SSCR_PULSESPERSTEP      = 0x10,
    XTS_SSCR_ITERATIONS         = 0x11,
    XTS_SSCR_DACSTEP            = 0x12,
    XTS_SSCR_DACMIN             = 0x13,
    XTS_SSCR_DACMAX             = 0x14,
    XTS_SSCR_SAMPLEDELAY        = 0x15,
    XTS_SSCR_PGSELECT           = 0x16,
    XTS_SSCR_MCLKDIV            = 0x17,
    XTS_SSCR_STAGGEREDPRFENABLE = 0x18,
    XTS_SSCR_DACAUTO            = 0x19,
    XTS_SSCR_FPS                = 0x1A,
    XTS_SSCR_FRAMESTITCH        = 0x1B
} sensor_system_config_reference_t;

typedef enum sensor_app_config_reference_en
{
    XTS_SACR_RESERVED           = 0,
    XTS_SACR_OUTPUTBASEBAND     = 0x10,
    XTS_SACR_OUTPUTSTATUS       = 0x11,
    XTS_SACR_OUTPUTSTATUSEXT    = 0x12,
    XTS_SACR_OUTPUT_RF		= 0x13,
    XTS_SACR_OUTPUT_PULSE_DOPPLER = 0x14,
} sensor_app_config_reference_t;

typedef enum serial_protocol_response_datatype_en
{
    XTS_SPRD_NONE               = 0x0,
    XTS_SPRD_BYTE               = 0x10,
    XTS_SPRD_INT                = 0x11,
    XTS_SPRD_FLOAT              = 0x12,
    XTS_SPRD_STRING             = 0x13,
//    XTS_SPRD_BASEBAND           = 0x14,
//    XTS_SPRD_PULSEDOPPLER       = 0x15,
    XTS_SPRD_USER               = 0x50,
} serial_protocol_response_datatype_t;

typedef enum serial_flag_byte_en
{
    XTS_FLAG_START          = 0x7d,
    XTS_FLAG_END            = 0x7e,
    XTS_FLAG_ESC            = 0x7f,
} serial_flag_byte_t;
#define XTS_FLAGSEQUENCE_START_NOESCAPE    0x7c7c7c7c

typedef enum serial_system_test_code_en
{
    XTS_SSTC_SUCCESS            = 0x00,
    XTS_SSTC_FAILED             = 0x01,
    XTS_SSTC_NOTIMPLEMENTED     = 0x02,
    XTS_SSTC_TEST_FIRST         = 0x10,
    XTS_SSTC_TEST_RESETCOUNT    = 0x10,
    XTS_SSTC_TEST_EXTRAM        = 0x11,
    XTS_SSTC_TEST_EXTFLASH      = 0x12,
    XTS_SSTC_TEST_PMIC          = 0x13,
    XTS_SSTC_TEST_INPUT_HIGH    = 0x14,
    XTS_SSTC_TEST_INPUT_LOW     = 0x15,
    XTS_SSTC_TEST_OUTPUT_HIGH   = 0x16,
    XTS_SSTC_TEST_OUTPUT_LOW    = 0x17,
    XTS_SSTC_TEST_READ_IOPINS   = 0x18,
    XTS_SSTC_TEST_X4_CONN       = 0x19,
    XTS_SSTC_TEST_CERT_MODE_EN  = 0x1A,
    XTS_SSTC_TEST_CERT_MODE_DIS = 0x1B,
    XTS_SSTC_TEST_HW_CONFIG     = 0x1C,
    XTS_SSTC_TEST_CERT_TX_ONLY  = 0x1D,
    XTS_SSTC_TEST_CERT_TX_ONLY_TX_OFF   = 0x1E,
    XTS_SSTC_TEST_CERT_STREAM_TX_OFF    = 0x1F,
    XTS_SSTC_TEST_LAST          = 0x1F

} serial_system_test_code_t;


// Moved to stid.h. Kept for legacy purposes, might be removed.
typedef enum serial_system_info_code_en
{
    XTS_SSIC_ITEMNUMBER         = XTID_SSIC_ITEMNUMBER,
    XTS_SSIC_ORDERCODE          = XTID_SSIC_ORDERCODE,
    XTS_SSIC_FIRMWAREID         = XTID_SSIC_FIRMWAREID,
    XTS_SSIC_VERSION            = XTID_SSIC_VERSION,
    XTS_SSIC_BUILD              = XTID_SSIC_BUILD,
    XTS_SSIC_APPIDLIST          = 0x05,
    XTS_SSIC_SERIALNUMBER       = XTID_SSIC_SERIALNUMBER
} serial_system_info_code_t;


typedef enum
{
    XTS_SM_RESERVED             = 0,
    XTS_SM_RUN                  = XTID_SM_RUN,
    XTS_SM_NORMAL               = 0x10,
    XTS_SM_IDLE                 = XTID_SM_IDLE,
    XTS_SM_MANUAL               = XTID_SM_MANUAL,
    XTS_SM_STOP                 = XTID_SM_STOP, // X4M Only
    XTS_SM_REG                  = 0x50
} sensor_mode_t;


typedef enum
{
    XTS_SM_REG_IDLE                 = 0,
    XTS_SM_REG_CERT_TX_ONLY         = 1,
    XTS_SM_REG_CERT_TX_ONLY_TX_OFF  = 2,
    XTS_SM_REG_CERT_STREAM_TX_OFF   = 3,
    XTS_SM_REG_CERT_TX_ONLY_EXTCLK  = 4,
    XTS_SM_REG_CERT_STREAM          = 5,
    XTS_SM_REG_CERT_TX_ONLY_CLKOUT  = 6,
    XTS_SM_REG_CERT_RX_FAILURE_EMU = 7
} sensor_mode_reg_submode_t;


typedef enum
{
    XTS_SM_NORMAL_EMBEDDED          = 0,
    XTS_SM_NORMAL_HIL_UP            = 1,
    XTS_SM_NORMAL_HIL_DOWN          = 2,
    XTS_SM_NORMAL_DEFAULT           = 0xff
} sensor_mode_normal_submode_t;


/* static const uint32_t ID_BASEBAND_BUFFER_IQCHANN = XTS_ID_BASEBAND_IQ; */
/* static const uint32_t ID_BASEBAND_AP = XTS_ID_BASEBAND_AMPLITUDE_PHASE; */

typedef enum
{
    ID_RAW_FRAME_BUFFER = 0,
    ID_BASEBAND_BUFFER_ICHANN,
    ID_BASEBAND_BUFFER_QCHANN,
    ID_BASEBAND_BUFFER_IQCHANN_LEGACY, // legacy version not to be used anymore
    ID_BASEBAND_ENVELOPE,
    ID_DOPPLER_FFT_INPUT,
    ID_DOPPLER_FFT_OUTPUT,
    ID_PULSEDOPPLER_MATRIX,
    ID_NOISEMAP,
    ID_FRAMESTORAGE_BUFFER_ICHANN,
    ID_FRAMESTORAGE_BUFFER_QCHANN,
    ID_RAW_FLASH_FRAME,

   // ID_BASEBAND_IQ = XTS_ID_BASEBAND_IQ,
   // ID_BASEBAND_AP = XTS_ID_BASEBAND_AMPLITUDE_PHASE,
	ID_BASEBAND_IQ = 0x0c,
	ID_BASEBAND_AP = 0x0d,

    ID_GENERIC_BYTE_STREAM,
    NUMBER_OF_MESSAGE_IDS
} eContentID;


typedef enum
{
    XTS_SACR_ID_BASEBAND_OUTPUT_OFF = 0,
    XTS_SACR_ID_BASEBAND_OUTPUT_IQ = 1,
    XTS_SACR_ID_BASEBAND_OUTPUT_AMPLITUDE_PHASE = 2,
    /* XTS_BASEBAND_OUTPUT_OFF      = 0, */
    /* XTS_BASEBAND_IQ              = 1, */
    /* XTS_BASEBAND_AMPLITUDE_PHASE = 2, */
    XTS_BASEBAND_AMPLITUDE_ONLY  = 3,
    XTS_BASEBAND_PHASE_ONLY      = 4,
} baseband_output_type_t;

typedef struct
{
    eContentID id;
    uint32_t Counter;
    uint32_t NumOfBins;
    float BinLength;
    float SamplingFrequency;
    float CarrierFrequency;
    float RangeOffset;
} baseband_output_header_t;

typedef enum
{
    XTS_PULSEDOPPLER_OUTPUT_OFF = 0,
    XTS_PULSEDOPPLER_AS_FLOAT   = 1,
    XTS_PULSEDOPPLER_AS_BYTE    = 2
} pulsedoppler_output_type_t;


typedef enum
{
    CRC_ERROR = 1,
    LOST_BYTES,
    INCOMPLETE_PACKET,
    OUT_OF_BUFFER,
    COMMAND_TIMEOUT,
} protocol_errors;


typedef struct xtsUserVsdmStatus
{
    uint32_t frameCtr;
    uint32_t state;
    uint32_t stateData; // RPM or Errorcode
    float objectDistance;
    float objectMovement;
    uint32_t signalQuality;
} xtsUserVsdmStatus;

typedef struct
{
    uint32_t presence;
    float objectDistance;
    float detectionValue;
    uint32_t signalQuality;
} xtsUserPresenceStatus;

typedef struct
{
    // Timing Measurement values
    float timingValue;
    uint64_t timingValueRaw;            // The raw version of the \ref timingValue variable.
    int32_t PRF;                        // PRF used when timing measurement was executed.
    int32_t delayInCoarseMeasured;      // Flag used to indicate if coarse tune delay elements have been measured.
    int32_t delayInMediumMeasured;      // Flag used to indicate if medium tune delay elements have been measured.
    int32_t delayInFineMeasured;
    float delayCoarseTune;              // Average length of all measured coarse tune delay elements.
    float delayMediumTune;              // Average length of all measured medium tune delay elements.
    float delayFineTune;                // Average length of all measured fine tune delay elements.
    float samplesPerSecond;             // Measured samples per second (sampling rate). This can also give the length between each sampler.
    float frameSize;                    // The length of the sampler array. This can also give the length between each sampler.
    float mediumInCoarseAverage;        // Internal variable used to measure delay elements.
    float fineInMediumAverage;          // Internal variable used to measure delay elements.
    float delayLengthCoarseTune[64];    // The length of each measured coarse tune delay element.
    float delayLengthMediumTune[64];    // The length of each medium tune delay element.
    float delayLengthFineTune[64];      // The length of each fine tune delay element (if measured).
    int32_t stopWatchInitialized;
    float frameLengths[15];
    float origSampleDelayLength;
    float sampleDelayLengthError;
    float PGDelay[16];
    int32_t chekcsum;                   // Checksum used when data is stored to Flash.

    // Radar configuration
    bool modified;
    int32_t cfgPulsesPerStep;
    int32_t cfgIterations;
    int32_t cfgDACStep;
    int32_t cfgDACMin;
    int32_t cfgDACMax;
    int32_t cfgPGSelect;
    int32_t cfgMClkDiv;
    int32_t cfgStaggeredPRFEnable;
    float cfgSampleDelay;
    int32_t cfgDACAuto;
    int32_t cfgFPS;
    int32_t cfgFrameStitch;

    // ChipParams misc
    float carrierFreqFromPG[13];
} xtssRadarSettings_t;


typedef struct
{
    uint32_t swRadarSweep;
    uint32_t swRadarCaptureFrame;
    uint32_t swRadarNormalizeFrame;
    uint32_t swDoPrimary;
    uint32_t swDoSecondary;
    uint32_t swDoOutput;

    uint32_t frameStitch;
    uint32_t FPS;

    uint32_t overflowRadarTrigger;
    uint32_t overflowPrimary;
    uint32_t overflowOutput;

    uint32_t memSysStackUntouched;
    uint32_t memOSHeap;
    uint32_t memTaskRadar;
    uint32_t memTaskAppEnginePrimary;
    uint32_t memTaskAppEngineSecondary;
    uint32_t memTaskSystem;
    uint32_t memTaskUartRx;
    uint32_t memTaskUartTx;

    uint32_t idleTicks;
    uint32_t comTxBufHighWatermark;

    int32_t periodSystemTicks;
    int32_t lastSystemTick;
} xtsUserRadarPerformanceStatus_t;

#endif /* XTSERIAL_DEFINITIONS_H_ */
