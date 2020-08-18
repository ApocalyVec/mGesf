#ifndef PROTOCOL_HELPERS_H
#define PROTOCOL_HELPERS_H


#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


static const unsigned char START_BYTE = 0x7d;
static const unsigned char STOP_BYTE = 0x7e;
static const unsigned char ESCAPE_BYTE = 0x7f;
static const unsigned char START_BYTE_NOESC = 0x80;

static const unsigned char DEPRECATED_FAST_PACKET = DEPRECATED_XTS_SPR_APPDATA_FAST;
//static const unsigned char DEPRECATED_FAST_PACKET = 0xcc;

bool isSpecialByte(const unsigned char byte);

void packet_start(
    AppendCallback callback,
    void * user_data);

void process_byte(
    unsigned char byte,
    AppendCallback callback,
    void * user_data);

void process_bytes(
    const unsigned char * bytes,
    unsigned int number_of_bytes,
    AppendCallback callback,
    void * user_data);

void process_int(int value,
    AppendCallback callback,
    void * user_data);

void process_ints(
    const int * data,
    const unsigned int length,
    AppendCallback callback,
    void * user_data);

void process_uint(unsigned int value,
    AppendCallback callback,
    void * user_data);

void process_uints(
    const unsigned int * data,
    const unsigned int length,
    AppendCallback callback,
    void * user_data);

void process_float(
    float data,
    AppendCallback callback,
    void * user_data);

void process_floats(
    const float * data,
    const unsigned int length,
    AppendCallback callback,
    void * user_data);

void packet_end(
    AppendCallback callback,
    void * user_data);


#ifdef __cplusplus
}
#endif


#endif
