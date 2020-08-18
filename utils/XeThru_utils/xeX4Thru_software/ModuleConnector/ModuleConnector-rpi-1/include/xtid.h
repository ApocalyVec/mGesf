#ifndef XTID_XEP_H
#define XTID_XEP_H

#ifdef __cplusplus
extern "C" {
#endif

// Profile IDs
#define XTS_ID_APP_UNKNOWN          0x0
#define XTS_ID_APP_PRESENCE         0x00288912
#define XTS_ID_APP_RESP             0x1423a2d6
#define XTS_ID_APP_SLEEP            0x00f17b17
#define XTS_ID_APP_RESPIRATION_2    0x064e57ad
#define XTS_ID_APP_RESPIRATION_3    0x47fabeba
#define XTS_ID_APP_RESPIRATION_4    0x4ac5d074
#define XTS_ID_APP_RESPIRATION_5    0xa9e03260
#define XTS_ID_APP_HEARTRATE        0x6b5c1609
#define XTS_ID_APP_DECIM            0x9bb3a2c6
#define XTS_ID_APP_PRESENCE_2       0x014d4ab8
#define XTS_ID_APP_PRESENCE_3       0x114d4ab8
#define XTS_ID_APP_PRESENCE_3_DEBUG 0x114d4ab9
#define XTS_ID_APP_HIGHBAY_1        0xd0f2862d
#define XTS_ID_APP_HIGHBAY_1_DEBUG  0xd0f2862e
#define XTS_ID_APP_RADAR            0x57dabf09

// Profile data and feature IDs
#define XTS_ID_RADAR_RF                                 0xc1a3117e
#define XTS_ID_RADAR_RF_NORMALIZED                      0xb13b8f12
#define XTS_ID_RADAR_BASEBAND_FLOAT                     0x026f1379
#define XTS_ID_RADAR_BASEBAND_Q15                       0xec8d0aa5
#define XTS_ID_DETECTION_ZONE                           0x96a10a1c
#define XTS_ID_APPLICATION_USER_ZONE                    0xa53695bb
#define XTS_ID_DETECTION_ZONE_LIMITS                    0x96a10a1d
#define XTS_ID_SENSITIVITY                              0x10a5112b
#define XTS_ID_PRESENCE_STATUS                          0x991a52be
#define XTS_ID_RESP_STATUS                              0x2375fe26
#define XTS_ID_RESP_STATUS_EXT                          0x2375a16b
#define XTS_ID_SLEEP_STATUS                             0x2375a16c
#define XTS_ID_PRESENCE_SINGLE                          0x723bfa1e
#define XTS_ID_PRESENCE_MOVINGLIST                      0x723bfa1f
#define XTS_ID_PROFILE_PARAMETERFILE                    0x32ba7623
#define XTS_ID_BASEBAND_IQ                              0x0000000c
#define XTS_ID_BASEBAND_AMPLITUDE_PHASE                 0x0000000d
#define XTS_ID_DECIM_STATUS                             0x327645aa
#define XTS_ID_PULSEDOPPLER_FLOAT                       0x00000010
#define XTS_ID_PULSEDOPPLER_BYTE                        0x00000011
#define XTS_ID_NOISEMAP_FLOAT                           0x00000012
#define XTS_ID_NOISEMAP_BYTE                            0x00000013
#define XTS_ID_RESPIRATION_MOVINGLIST                   0x610a3b00
#define XTS_ID_RESPIRATION_MOVEMENTLIST                 0x610a3b00
#define XTS_ID_RESPIRATION_DETECTIONLIST                0x610a3b02
#define XTS_ID_RESPIRATION_NORMALIZEDMOVEMENTLIST       0xC3A331CF
#define XTS_ID_VITAL_SIGNS                              0x20020102
#define XTS_ID_SLEEPSTAGE                               0x41083A02
#define XTS_ID_TX_CENTER_FREQ                           0x71AF2A2B
#define XTS_ID_LED_CONTROL                              0x71AF2A2C

// System Info Code definitions
#define XTID_SSIC_ITEMNUMBER         (0x00)
#define XTID_SSIC_ORDERCODE          (0x01)
#define XTID_SSIC_FIRMWAREID         (0x02)
#define XTID_SSIC_VERSION            (0x03)
#define XTID_SSIC_BUILD              (0x04)
#define XTID_SSIC_SERIALNUMBER       (0x06)
#define XTID_SSIC_VERSIONLIST        (0x07)
#define XTID_SSIC_SYSTEMCOREID       (0x08)
#define XTID_SSIC_BOOTLOADER         (0x09)

// Sensor mode IDs
#define XTID_SM_RUN                  (0x01)
#define XTID_SM_NORMAL               (0x10)
#define XTID_SM_IDLE                 (0x11)
#define XTID_SM_MANUAL               (0x12)
#define XTID_SM_STOP                 (0x13)

// Frame Inject mode IDs
#define XTID_FIM_LOOP                (0x01)
#define XTID_FIM_SEQUENTIAL          (0x02)
#define XTID_FIM_SINGLE              (0x03)

#define XTID_BAUDRATE_9600	     9600
#define XTID_BAUDRATE_19200	     19200
#define XTID_BAUDRATE_38400	     38400
#define XTID_BAUDRATE_115200         115200
#define XTID_BAUDRATE_230400         230400
#define XTID_BAUDRATE_460800         460800
#define XTID_BAUDRATE_500000         500000
#define XTID_BAUDRATE_576000         576000
#define XTID_BAUDRATE_921600         921600
#define XTID_BAUDRATE_1000000        1000000
#define XTID_BAUDRATE_2000000        2000000
#define XTID_BAUDRATE_3000000        3000000
#define XTID_BAUDRATE_4000000        4000000


// IOPin definitions
#define XTID_IOPIN_SETUP_INPUT         (0)
#define XTID_IOPIN_SETUP_OUTPUT        (1)
#define XTID_IOPIN_SETUP_PUSH_PULL     (2)
#define XTID_IOPIN_SETUP_INVERTED      (4)
#define XTID_IOPIN_SETUP_PULLUP        (8)

#define XTID_IOPIN_FEATURE_DISABLE     (0)
#define XTID_IOPIN_FEATURE_DEFAULT     (1)
#define XTID_IOPIN_FEATURE_PASSIVE     (2)
#define XTID_IOPIN_FEATURE_PRESENCE    (3)
#define XTID_IOPIN_FEATURE_MOVEMENT    (4)
#define XTID_IOPIN_FEATURE_BREATHING   (5)
#define XTID_IOPIN_FEATURE_NOPRESENCE  (6)
#define XTID_IOPIN_FEATURE_LAST        (6)

// Output control
#define XTID_OUTPUT_CONTROL_DISABLE    (0)
#define XTID_OUTPUT_CONTROL_ENABLE     (1)
#define XTID_OUTPUT_CONTROL_DEBUG      (1<<31)

// PD output control (bitfield)
#define XTID_OUTPUT_CONTROL_PD_SLOW_ENABLE    (1)
#define XTID_OUTPUT_CONTROL_PD_FAST_ENABLE    (2)

// Noisemap control (bitfield)
#define XTID_NOISEMAP_CONTROL_USE_DEFAULT   (0<<0)
#define XTID_NOISEMAP_CONTROL_DISABLE       (0<<0)
#define XTID_NOISEMAP_CONTROL_ENABLE        (1<<0)

#define XTID_NOISEMAP_CONTROL_NONADAPTIVE   (0<<1)
#define XTID_NOISEMAP_CONTROL_ADAPTIVE      (1<<1)

#define XTID_NOISEMAP_CONTROL_USE_STORED    (0<<2)
#define XTID_NOISEMAP_CONTROL_INIT_ON_RESET (1<<2)

// Led control
#define XTID_LED_MODE_OFF       (0)
#define XTID_LED_MODE_SIMPLE    (1)
#define XTID_LED_MODE_FULL      (2)

// Center Frequency
#define XTID_CENTER_FREQ_LOWBAND    (3)
#define XTID_CENTER_FREQ_HIGHBAND   (4)


// Serial protocol IDs
#define XTID_MCP_ERROR_NOT_RECOGNIZED   (1)


// Profile codes
#define XTS_VAL_RESP_STATE_BREATHING				0x00 // Valid RPM sensing
#define XTS_VAL_RESP_STATE_MOVEMENT					0x01 // Detects motion, but can not identify breath
#define XTS_VAL_RESP_STATE_MOVEMENT_TRACKING		0x02 // Detects motion, possible breathing soon
#define XTS_VAL_RESP_STATE_NO_MOVEMENT				0x03 // No movement detected
#define XTS_VAL_RESP_STATE_INITIALIZING				0x04 // Initializing sensor
#define XTS_VAL_RESP_STATE_ERROR					0x05 // Sensor has detected some problem. StatusValue indicates problem.
#define XTS_VAL_RESP_STATE_UNKNOWN					0x06 // Undefined state.
#define XTS_VAL_RESP_STATE_HEART_RATE_AND_BREATHING	0x07 // Heart rate and beathing

#define XTS_VAL_PRESENCE_PRESENCESTATE_NO_PRESENCE      0 // No presence detected
#define XTS_VAL_PRESENCE_PRESENCESTATE_PRESENCE         1 // Presence detected
#define XTS_VAL_PRESENCE_PRESENCESTATE_INITIALIZING     2 // The sensor initializes after the Presence 2 Profile is executed
#define XTS_VAL_PRESENCE_PRESENCESTATE_UNKNOWN          3 // The sensor is in an unknown state and requires a Profile and User Settings to be loaded



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XTID_XEP_H */
