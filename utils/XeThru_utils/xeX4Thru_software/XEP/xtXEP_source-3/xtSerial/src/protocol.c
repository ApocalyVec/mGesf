#include "protocol.h"
#include "protocol_helpers.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    MCP_PACKETMODE_DEFAULT,
    MCP_PACKETMODE_NOESCAPE
} mcpPacketMode_t;


struct xtProtocol
{
    unsigned char * destination;
    unsigned int max_size;
    unsigned int len;
    PacketHandlerFunc packetHandler;
    ErrorHandlerFunc errorHandler;
    bool DEPRECATEDfastMode;
    mcpPacketMode_t packetMode;
    void * user_data;
    bool escaped;
};

unsigned int getInstanceSize()
{
    return sizeof(xtProtocol);
}


static void reset(xtProtocol * instance)
{
    instance->len = 0;
    instance->escaped = false;
    instance->DEPRECATEDfastMode = false;
    instance->packetMode = MCP_PACKETMODE_DEFAULT;
}

xtProtocol * createApplicationProtocol(
        PacketHandlerFunc packetHandler,
        ErrorHandlerFunc errorHandler,
        void * user_data,
        void * instance_memory,
        unsigned char * buffer,
        unsigned int max_size)
{
    xtProtocol * instance = (xtProtocol *)instance_memory;
    instance->destination = buffer;
    instance->max_size = max_size;
    instance->packetHandler = packetHandler;
    instance->errorHandler = errorHandler;
    instance->user_data = user_data;
    reset(instance);
    return instance;
}


void destroyProtocol(xtProtocol * instance)
{
    (void)instance;
}


bool isSpecialByte(const unsigned char byte)
{
    return
        byte == START_BYTE ||
        byte == STOP_BYTE ||
        byte == ESCAPE_BYTE;

}


static unsigned char crc_ = 0;

static void _packet_reset(xtProtocol * instance)
{
    instance->DEPRECATEDfastMode = false;
    instance->len = 0;
    instance->destination[0] = 0;
    instance->packetMode = MCP_PACKETMODE_DEFAULT;
}

void packet_start(
    AppendCallback callback,
    void * user_data)
{
    crc_ = START_BYTE;
    callback(START_BYTE, user_data);
}


void process_byte(
    unsigned char byte,
    AppendCallback callback,
    void * user_data)
{
    if(isSpecialByte(byte))
        callback(ESCAPE_BYTE, user_data);

    crc_ ^= byte;
    callback(byte, user_data);
}

void process_bytes(
    const unsigned char * bytes,
    unsigned int number_of_bytes,
    AppendCallback callback,
    void * user_data)
{
    unsigned int i = 0;
    for (i = 0; i < number_of_bytes; ++i) {
        process_byte(bytes[i], callback, user_data);
    }
}


void process_int(
    int value,
    AppendCallback callback,
    void * user_data)
{
    process_byte(value & 0xff, callback, user_data);
    process_byte(value >> 8 & 0xff, callback, user_data);
    process_byte(value >> 16 & 0xff, callback, user_data);
    process_byte(value >> 24& 0xff, callback, user_data);
}

void process_ints(
    const int * data,
    const unsigned int length,
    AppendCallback callback,
    void * user_data)
{
    unsigned int i = 0;
    for (i = 0; i < length; ++i) {
        process_int(data[i], callback, user_data);
    }
}

void process_uint(
    unsigned int value,
    AppendCallback callback,
    void * user_data)
{
    process_byte(value & 0xff, callback, user_data);
    process_byte(value >> 8 & 0xff, callback, user_data);
    process_byte(value >> 16 & 0xff, callback, user_data);
    process_byte(value >> 24& 0xff, callback, user_data);
}

/* void process_uints( */
/*     const unsigned int * data, */
/*     const unsigned int length, */
/*     AppendCallback callback, */
/*     void * user_data) */
/* { */
/*     unsigned int i = 0; */
/*     for (i = 0; i < length; ++i) { */
/*         process_uint(data[i], callback, user_data); */
/*     } */
/* } */


void process_float(
    float data,
    AppendCallback callback,
    void * user_data)
{
    unsigned char * p = (unsigned char *)&data;
    unsigned int i = 0;
    for(i = 0; i < sizeof(float); ++i) {
        process_byte(*p++, callback, user_data);
    }
}

void process_floats(
    const float * data,
    const unsigned int length,
    AppendCallback callback,
    void * user_data)
{
    unsigned int i = 0;
    for (i = 0; i < length; ++i) {
        process_float(data[i], callback, user_data);
    }
}


void packet_end(
    AppendCallback callback,
    void * user_data)
{
   if(isSpecialByte(crc_))
       callback(ESCAPE_BYTE, user_data);

    callback(crc_, user_data);
    callback(STOP_BYTE, user_data);
}

int createCommand(
    const unsigned char * src,
    unsigned int length,
    AppendCallback callback,
    void * user_data)

{
    packet_start(callback, user_data);
    unsigned int i = 0;
    for (i = 0; i < length; ++i) {
        process_byte(src[i], callback, user_data);
    }

    packet_end(callback, user_data);
    return 0;
}


static unsigned char crc8(
    unsigned char crc,
    const unsigned char *data,
    unsigned int length)
{
    while (length)
    {
        --length;
        crc ^= *data;
        data++;
    }
    return crc;
}


static bool check_crc(
    const unsigned char * destination,
    const unsigned int length)
{
    const unsigned int data_length = length - 1;
    const unsigned char calc_crc = crc8(START_BYTE, destination, data_length);
    const unsigned int crc_pos = data_length;
    const unsigned char packet_crc = destination[crc_pos];

    return calc_crc == packet_crc;
}


static bool outOfBuffer(const xtProtocol * instance)
{
    //printf("%s, %d -- %d, %d\n", __func__, __LINE__, instance->len, instance->max_size);
    return instance->len == instance->max_size;
}




static int checkCrcAndSend(const xtProtocol * instance)
{
    if (!check_crc(instance->destination, instance->len)) {
        instance->errorHandler(instance->user_data, CRC_ERROR);
        return 0;
    }

    instance->packetHandler(
        instance->user_data,
        instance->destination,
        instance->len - 1); // do not include the crc
    return 1;
}


static int handleSpecialBytes(
    xtProtocol * instance,
    const unsigned char byte)
{
    int packets_sent = 0;

    if (byte == ESCAPE_BYTE) {
        instance->escaped = true;
    }
    else if ( byte == START_BYTE) {
        if(instance->len != 0)
            instance->errorHandler(instance->user_data, LOST_BYTES);
        _packet_reset(instance); //instance->len = 0;
    }
    else if (byte == STOP_BYTE){
        if (instance->len == 0) {
            instance->errorHandler(instance->user_data, INCOMPLETE_PACKET);
        }
        else {
            packets_sent += checkCrcAndSend(instance);
            _packet_reset(instance); //instance->len = 0;
        }
    }
    else if (byte == DEPRECATED_FAST_PACKET)
    {
        instance->DEPRECATEDfastMode = true;
        //packets_sent++;
    }

    return packets_sent;
}


static int handleSingleByte(
    xtProtocol * instance,
    const unsigned char byte)
{
    if (instance->packetMode == MCP_PACKETMODE_NOESCAPE)
    {
        instance->destination[instance->len++] = byte;
        if (instance->len<4) // Wait for packet length info is ready.
            return 0;

        const uint32_t header_length = 5; // Length + CRC
        const uint32_t data_length = *(uint32_t*)(&instance->destination[0]);
        /* if (packet_length < header_length) */
        /* { */
        /*     // Abort. */
        /*     _packet_reset(instance); */
        /*     return 0; */
        /* } */

        if (instance->len == data_length + header_length)
        {
            //const uint32_t data_length = packet_length - header_length;
            //const uint8_t crc = *(unsigned int*)(&instance->destination[4]);

            //(void)crc; // CRC not used yet. Assume OK.

            uint8_t* data_ptr = (uint8_t*)(&instance->destination[header_length]); 

            instance->packetHandler(
                instance->user_data,
                (unsigned char*)data_ptr,
                data_length);

            _packet_reset(instance);
            return 1;
        }
    }
    else
    {
        if (instance->escaped) {
            instance->escaped = false;
            instance->destination[instance->len++] = byte;
        }
        else if (isSpecialByte(byte)) {
            return handleSpecialBytes(instance, byte);
        }
        else {
            /*printf("%s: %d - byte: 0x%02x len %d\n", __FILE__, __LINE__, (int) byte,  instance->len);*/

            if (byte == DEPRECATED_FAST_PACKET && instance->len == 0)
                instance->DEPRECATEDfastMode = true;
            instance->destination[instance->len++] = byte;
        }

        if ((instance->destination[0] != XTS_FLAG_START) && (instance->len>=4))
        {
            // Check for NoEscape mode.
            uint32_t *noescape_candidate = (uint32_t*)(&instance->destination[instance->len-4]);
            if (*noescape_candidate == XTS_FLAGSEQUENCE_START_NOESCAPE)
            {
                _packet_reset(instance);
                instance->packetMode = MCP_PACKETMODE_NOESCAPE;
            }
        }
    }
    return 0;
}


static int DEPRECATEDhandleFastSingleByte(
    xtProtocol * instance,
    const unsigned char byte)
{
    /*printf("DEPRECATEDhandleFastSingleByte - 0x%02x\n", byte);*/
    instance->destination[instance->len++] = byte;

    if (instance->len < 5)
        return 0;

    const unsigned int offset = 1; // data_type
    const unsigned int packet_length = *(unsigned int*)(&instance->destination[offset]);

    const unsigned int data_type_size = 1;
    const unsigned int packet_len_size = 4;
    const unsigned int crc_size = 1;
    const unsigned int total_length =
        data_type_size + packet_len_size + packet_length + crc_size;

    if (instance->len == total_length) {
        instance->packetHandler(
            instance->user_data,
            instance->destination,
             instance->len - 1);

        instance->DEPRECATEDfastMode = false;
        instance->len = 0;
        return 1;
    }

    return 0;
}


int parseData(
    xtProtocol * instance,
    const unsigned char * data,
    unsigned int length)
{
    int packets_sent = 0;
    unsigned int i = 0;
    for(i = 0; i < length; ++i) {

        if (outOfBuffer(instance)) {
            instance->errorHandler(instance->user_data, OUT_OF_BUFFER);
            reset(instance);
            return -1;
        }

        if (instance->DEPRECATEDfastMode) {
            packets_sent += DEPRECATEDhandleFastSingleByte(instance, data[i]);
        }
        else {
            packets_sent += handleSingleByte(instance, data[i]);
        }
    }
    return packets_sent;
}


int createSetSensorModeCommand(
    sensor_mode_t mode,
    unsigned char data,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_MOD_SETMODE, callback, user_data);
    process_byte(mode, callback, user_data);
    if ( mode == XTS_SM_REG ||
         mode == XTS_SM_NORMAL) {
        process_byte(data, callback, user_data);
    }
    packet_end(callback, user_data);
    return 0;
}

int createGetSensorModeCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_MOD_GETMODE, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createPingCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_PING, callback, user_data);
    process_byte(0xae, callback, user_data);
    process_byte(0xea, callback, user_data);
    process_byte(0xaa, callback, user_data);
    process_byte(0xee, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createStartBootloaderCommand(
    uint32_t key,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_START_BOOTLOADER, callback, user_data);
    process_uint(key, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createLoadAppCommand(
    uint32_t app_id,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_MOD_LOADAPP, callback, user_data);
    process_byte((unsigned char)(app_id & 0xff), callback, user_data);
    process_byte((unsigned char)(app_id >> 8 & 0xff), callback, user_data);
    process_byte((unsigned char)(app_id >> 16 & 0xff), callback, user_data);
    process_byte((unsigned char)(app_id >> 24 & 0xff), callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createModuleResetCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_MOD_RESET, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createResetToFactoryPreset(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);
    process_byte(XTS_SPCA_FACTORY_PRESET, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createSetLedControlCommand(
    unsigned char mode,
    unsigned char intensity,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_MOD_SETLEDCONTROL, callback, user_data);
    process_byte(mode, callback, user_data);
    process_byte(intensity, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createGetLedControlCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);    
    process_byte(XTS_SPCA_GET, callback, user_data);
    process_int(XTS_ID_LED_CONTROL, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createSetBaudRateCommand(
    int baudrate,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_COMM_SETBAUDRATE, callback, user_data);
    process_int(baudrate, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createSystemGetVersionCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_SYSTEM_GET_VERSION, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createSystemRunTest(
    const unsigned char test_code,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_SYSTEM_TEST, callback, user_data);
    process_byte(test_code, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createSystemGetInfoCommand(
    unsigned char info_code,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_SYSTEM_GET_INFO, callback, user_data);
    process_byte(info_code, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createInjectFrameCommand(
    uint32_t frame_counter,
    uint32_t frame_length,
    const float * frame,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_INJECT_FRAME, callback, user_data);
    process_uint(frame_counter, callback, user_data);
    process_uint(frame_length, callback, user_data);
    process_floats(frame, frame_length * 2, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createPrepareInjectFrameCommand(
    uint32_t num_frames,
    uint32_t num_bins,
    uint32_t mode,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_PREPARE_INJECT_FRAME, callback, user_data);
    process_uint(num_frames, callback, user_data);
    process_uint(num_bins, callback, user_data);
    process_uint(mode, callback, user_data);    
    packet_end(callback, user_data);
    return 0;
}

int createAppCommand(
    unsigned char app_command,
    const unsigned char * data,
    unsigned int length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);
    process_byte(app_command, callback, user_data);
    unsigned int i = 0;
    for(i = 0; i < length; ++i) {
        process_byte(data[i], callback, user_data);
    }
    packet_end(callback, user_data);
    return 0;
}


int createAppSetIntCommand(
    int reference,
    const int * data,
    unsigned int data_length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_APP_SETINT, callback, user_data);
    process_int(reference, callback, user_data);
    process_int(data_length, callback, user_data);
    process_ints(data, data_length, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createConfigSetIntCommand(
    int reference,
    int data,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_SYSCFG_SETINT, callback, user_data);
    process_int(reference, callback, user_data);
    process_int(data, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createGetSystemInfoCommand(
    const unsigned char info_code,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_SYSTEM_GET_INFO, callback, user_data);
    process_byte(info_code, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createSetDetectionZoneCommand(
    float start,
    float end,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);
    process_byte(XTS_SPCA_SET, callback, user_data);
    process_int(XTS_ID_DETECTION_ZONE, callback, user_data);
    process_float(start, callback, user_data);
    process_float(end, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createSetSensitivityCommand(
    const uint32_t sensitivity,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);
    process_byte(XTS_SPCA_SET, callback, user_data);
    process_int(XTS_ID_SENSITIVITY, callback, user_data);
    process_int(sensitivity, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createGetSensitivityCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);    
    process_byte(XTS_SPCA_GET, callback, user_data);
    process_int(XTS_ID_SENSITIVITY, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createSetTxCenterFrequencyCommand(
    const uint32_t frequencyBand,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);
    process_byte(XTS_SPCA_SET, callback, user_data);
    process_int(XTS_ID_TX_CENTER_FREQ, callback, user_data);
    process_int(frequencyBand, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}
int createGetTxCenterFrequencyCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);
    process_byte(XTS_SPCA_GET, callback, user_data);
    process_int(XTS_ID_TX_CENTER_FREQ, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createSetParameterFileCommand(
    uint32_t filename_length,
    uint32_t data_length,
    const char * filename,
    const char * data,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);
    process_byte(XTS_SPCA_SET, callback, user_data);
    process_int(XTS_ID_PROFILE_PARAMETERFILE, callback, user_data);
    process_int(filename_length + 1, callback, user_data);
    process_int(data_length + 1, callback, user_data);
    process_bytes((unsigned char *)filename, filename_length + 1, callback, user_data);
    process_bytes((unsigned char *)data, data_length + 1, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createGetParameterFileCommand(
    uint32_t filename_length,
    const char * filename,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);
    process_byte(XTS_SPCA_GET, callback, user_data);
    process_int(XTS_ID_PROFILE_PARAMETERFILE, callback, user_data);
    process_int(filename_length + 1, callback, user_data);
    process_bytes((unsigned char *)filename, filename_length + 1, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createConfigSetFloatCommand(
    int reference,
    float data,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_SYSCFG_SETFLOAT, callback, user_data);
    process_int(reference, callback, user_data);
    process_float(data, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createDebugLevelCommand(
    unsigned char debug_level,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DEBUG_LEVEL, callback, user_data);
    process_byte(debug_level, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createPageWriteCommand(
        uint16_t page_address,
        const unsigned char * page_data,
        unsigned int page_length,
        AppendCallback callback,
        void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_BOOTLOADER, callback, user_data);
    process_byte(XTS_SPCB_WRITE_PAGE, callback, user_data);
    process_byte(page_address >> 8 & 0xff, callback, user_data);
    process_byte(page_address & 0xff, callback, user_data);
    const unsigned char padding = 0x00;
    process_byte(padding, callback, user_data);
    process_bytes(page_data, page_length, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createApplicationStartCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_BOOTLOADER, callback, user_data);
    process_byte(XTS_SPCB_START_APP, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createBootloaderDeviceInfoCommand(AppendCallback callback, void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_BOOTLOADER, callback, user_data);
    process_byte(XTS_SPCB_GET_DEVICE_INFO, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createSetRegisterCommand(
    const uint32_t id,
    const uint32_t value,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_RADLIB_SETREGFIELD, callback, user_data);
    process_int(id, callback, user_data);
    process_int(value, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createGetRegisterCommand(
    const uint32_t id,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_RADLIB_GETREGFIELD, callback, user_data);
    process_int(id, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createGetIntCommand(
    uint32_t id,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_RADLIB_GETVARINT, callback, user_data);
    process_int(id, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createSetIntCommand(
    uint32_t id,
    uint32_t value,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_RADLIB_SETVARINT, callback, user_data);
    process_int(id, callback, user_data);
    process_int(value, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createRunTimingMeasurementCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_RADLIB_TIMINGMEASUREMENT, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createSetFloatCommand(
    const uint32_t id,
    const float value,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_RADLIB_SETVARFLOAT, callback, user_data);
    process_int(id, callback, user_data);
    process_float(value, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createGetFloatCommand(
    const uint32_t id,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_RADLIB_GETVARFLOAT, callback, user_data);
    process_int(id, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createGetFrame(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_RADLIB_GETFRAMEFLOAT, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverSetFpsCommand(
    const float fps,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_FPS, callback, user_data);
    process_float(fps, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverSetIterationsCommand(
    const uint32_t iterations,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_ITERATIONS, callback, user_data);
    process_int(iterations, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}
int createX4DriverGetIterationsCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_ITERATIONS, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}
int createX4DriverSetPulsesPerStepCommand(
    const uint32_t pulsesperstep,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_PULSESPERSTEP, callback, user_data);
    process_int(pulsesperstep, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}
int createX4DriverGetPulsesPerStepCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_PULSESPERSTEP, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverSetDownconversionCommand(
    const uint8_t downconversion,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_DOWNCONVERSION, callback, user_data);
    process_byte(downconversion, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}
int createX4DriverGetDownconversionCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_DOWNCONVERSION, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}
int createX4DriverGetFrameBinCountCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_FRAMEBINCOUNT, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverSetFrameAreaCommand(
    const float start,
    const float end,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_FRAMEAREA, callback, user_data);
    process_float(start, callback, user_data);
    process_float(end, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverInitCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_INIT, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverSetDacStepCommand(
    const uint8_t dac_step,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_DACSTEP, callback, user_data);
    process_byte(dac_step, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}
int createX4DriverGetDacStepCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_DACSTEP, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}
int createX4DriverSetDacMinCommand(
    const uint32_t dac_min,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_DACMIN, callback, user_data);
    process_int(dac_min, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}
int createX4DriverGetDacMinCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_DACMIN, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverSetDacMaxCommand(
    const uint32_t dac_max,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_DACMAX, callback, user_data);
    process_int(dac_max, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverGetDacMaxCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_DACMAX, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverSetFrameAreaOffsetCommand(
    const float offset,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_FRAMEAREAOFFSET, callback, user_data);
    process_float(offset, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverSetEnableCommand(
    const uint8_t enable,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_ENABLE, callback, user_data);
    process_byte(enable, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverSetTxCenterFrequencyCommand(
    const uint8_t tx_center_frequency,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_TXCENTERFREQUENCY, callback, user_data);
    process_byte(tx_center_frequency, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverGetTxCenterFrequencyCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_TXCENTERFREQUENCY, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverSetTxPowerCommand(
    const uint8_t tx_power,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_TXPOWER, callback, user_data);
    process_byte(tx_power, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}
int createX4DriverGetTxPowerCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_TXPOWER, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverGetFpsCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_FPS, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createX4DriverSetSpiRegisterCommand(
    const uint8_t address,
    const uint8_t value,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_SPIREGISTER, callback, user_data);
    process_byte(address, callback, user_data);
    process_byte(value, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverGetSpiRegisterCommand(
    const uint8_t address,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_SPIREGISTER, callback, user_data);
    process_byte(address, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverWriteToSpiRegisterCommand(
    const uint8_t address,
    const uint8_t *values,
    const uint32_t length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_WRITE, callback, user_data);
    process_int(XTS_SPCXI_SPIREGISTER, callback, user_data);
    process_byte(address, callback, user_data);
    process_bytes(values, length, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverReadFromSpiRegisterCommand(
    const uint8_t address,
    const uint32_t length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_READ, callback, user_data);
    process_int(XTS_SPCXI_SPIREGISTER, callback, user_data);
    process_byte(address, callback, user_data);
    process_uint(length, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverSetPifRegisterCommand(
    const uint8_t address,
    const uint8_t value,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_PIFREGISTER, callback, user_data);
    process_byte(address, callback, user_data);
    process_byte(value, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverGetPifRegisterCommand(
    const uint8_t address,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_PIFREGISTER, callback, user_data);
    process_byte(address, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverSetXifRegisterCommand(
    const uint8_t address,
    const uint8_t value,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_XIFREGISTER, callback, user_data);
    process_byte(address, callback, user_data);
    process_byte(value, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverGetXifRegisterCommand(
    const uint8_t address,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_XIFREGISTER, callback, user_data);
    process_byte(address, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverSetPrfDivCommand(
    const uint8_t prf_div,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_SET, callback, user_data);
    process_int(XTS_SPCXI_PRFDIV, callback, user_data);
    process_byte(prf_div, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverGetPrfDivCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_PRFDIV, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverGetFrameAreaCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_FRAMEAREA, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createX4DriverGetFrameAreaOffsetCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_X4DRIVER, callback, user_data);
    process_byte(XTS_SPCX_GET, callback, user_data);
    process_int(XTS_SPCXI_FRAMEAREAOFFSET, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createSetIOPinControlCommand(
    const uint32_t pin_id,
    const uint32_t pin_setup,
    const uint32_t pin_feature,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_IOPIN, callback, user_data);
    process_byte(XTS_SPCIOP_SETCONTROL, callback, user_data);
    process_uint(pin_id, callback, user_data);
    process_uint(pin_setup, callback, user_data);
    process_uint(pin_feature, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createGetIOPinControlCommand(
    const uint32_t pin_id,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_IOPIN, callback, user_data);
    process_byte(XTS_SPCIOP_GETCONTROL, callback, user_data);
    process_uint(pin_id, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createSetIOPinValueCommand(
    const uint32_t pin_id,
    const uint32_t pin_value,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_IOPIN, callback, user_data);
    process_byte(XTS_SPCIOP_SETVALUE, callback, user_data);
    process_uint(pin_id, callback, user_data);
    process_uint(pin_value, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createGetIOPinValueCommand(
    const uint32_t pin_id,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_IOPIN, callback, user_data);
    process_byte(XTS_SPCIOP_GETVALUE, callback, user_data);
    process_uint(pin_id, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createLoadProfileCommand(
    uint32_t profile_id,
    AppendCallback callback,
    void * user_data)
{
    return createLoadAppCommand(profile_id, callback, user_data);
}

int createGetDetectionZoneCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);
    process_byte(XTS_SPCA_GET, callback, user_data);
    process_int(XTS_ID_DETECTION_ZONE, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createGetDetectionZoneLimitsCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);
    process_byte(XTS_SPCA_GET, callback, user_data);
    process_int(XTS_ID_DETECTION_ZONE_LIMITS, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createSetOutputControlCommand(
    const uint32_t output_feature,
    const uint32_t output_control,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_OUTPUT, callback, user_data);
    process_byte(XTS_SPCO_SETCONTROL, callback, user_data);
    process_uint(output_feature, callback, user_data);
    process_uint(output_control, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createGetOutputControlCommand(
    const uint32_t output_feature,    
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_OUTPUT, callback, user_data);
    process_byte(XTS_SPCO_GETCONTROL, callback, user_data);
    process_uint(output_feature, callback, user_data);    
    packet_end(callback, user_data);
    return 0;
}

int createSetDebugOutputControlCommand(
    const uint32_t output_feature,
    const uint32_t output_control,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DEBUG_OUTPUT, callback, user_data);
    process_byte(XTS_SPCO_SETCONTROL, callback, user_data);
    process_uint(output_feature, callback, user_data);
    process_uint(output_control, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createGetDebugOutputControlCommand(
    const uint32_t output_feature,    
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DEBUG_OUTPUT, callback, user_data);
    process_byte(XTS_SPCO_GETCONTROL, callback, user_data);
    process_uint(output_feature, callback, user_data);    
    packet_end(callback, user_data);
    return 0;
}

int createSetBaudRate(
    const uint32_t baudrate,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_COMM_SETBAUDRATE, callback, user_data);
    process_uint(baudrate, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createStoreNoiseMapCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);
    process_byte(XTS_SPCA_STORE_NOISEMAP, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createLoadNoiseMapCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);
    process_byte(XTS_SPCA_LOAD_NOISEMAP, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createDeleteNoiseMapCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_APPCOMMAND, callback, user_data);
    process_byte(XTS_SPCA_DELETE_NOISEMAP, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createSetNoiseMapControlCommand(
    const uint32_t control,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_MOD_NOISEMAP, callback, user_data);
    process_byte(XTS_SPCN_SETCONTROL, callback, user_data);
    process_uint(control, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createGetNoiseMapControlCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_MOD_NOISEMAP, callback, user_data);
    process_byte(XTS_SPCN_GETCONTROL, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

/* int createGetParameterFileCommand( */
/*     AppendCallback callback, */
/*     void * user_data) */
/* { */
/*     packet_start(callback, user_data); */
/*     process_byte(XTS_SPC_DIR_COMMAND, callback, user_data); */
/*     process_byte(XTS_SDC_GET_PARAMETER_FILE, callback, user_data); */
/*     packet_end(callback, user_data); */
/*     return 0; */
/* } */


int createSearchForFileTypeCommand(
    uint32_t type,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_SEARCH_FILE_TYPE, callback, user_data);
    process_uint(type, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createFindAllFilesCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_FIND_ALL_FILES, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createNewFileCommand(
    uint32_t file_type,
    uint32_t identifier,
    uint32_t length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_CREATE_NEW_FILE, callback, user_data);
    process_uint(file_type, callback, user_data);
    process_uint(identifier, callback, user_data);
    process_uint(length, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createOpenFileCommand(
    uint32_t file_type,
    uint32_t identifier,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_OPEN_FILE, callback, user_data);
    process_uint(file_type, callback, user_data);
    process_uint(identifier, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createSetFileDataCommand(
    uint32_t type,
    uint32_t identifier,
    uint32_t offset,
    uint32_t length,
    const uint8_t * data,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_SET_FILE_DATA, callback, user_data);
    process_uint(type, callback, user_data);
    process_uint(identifier, callback, user_data);
    process_uint(offset, callback, user_data);
    process_uint(length, callback, user_data);
    process_bytes(data, length, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createCloseFileCommand(
    uint32_t type,
    uint32_t identifier,
    uint32_t commit,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_CLOSE_FILE, callback, user_data);
    process_uint(type, callback, user_data);
    process_uint(identifier, callback, user_data);
    process_uint(commit, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createGetFileLengthCommand(
    uint32_t type,
    uint32_t identifier,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_GET_FILE_LENGTH, callback, user_data);
    process_uint(type, callback, user_data);
    process_uint(identifier, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createDeleteFileCommand(
    uint32_t type,
    uint32_t identifier,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_DELETE_FILE, callback, user_data);
    process_uint(type, callback, user_data);
    process_uint(identifier, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}


int createGetFileDataCommand(
    uint32_t type,
    uint32_t identifier,
    uint32_t offset,
    uint32_t length,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_GET_FILE_DATA, callback, user_data);
    process_uint(type, callback, user_data);
    process_uint(identifier, callback, user_data);
    process_uint(offset, callback, user_data);
    process_uint(length, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}

int createFormatFilesystemCommand(
    uint32_t key,
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_FORMAT_FILESYSTEM, callback, user_data);
    process_uint(key, callback, user_data);
    packet_end(callback, user_data);
    return 0;

}

int createGetProfileIdCommand(
    AppendCallback callback,
    void * user_data)
{
    packet_start(callback, user_data);
    process_byte(XTS_SPC_DIR_COMMAND, callback, user_data);
    process_byte(XTS_SDC_GET_PROFILEID, callback, user_data);
    packet_end(callback, user_data);
    return 0;
}



#ifdef __cplusplus
}
#endif /* __cplusplus */


