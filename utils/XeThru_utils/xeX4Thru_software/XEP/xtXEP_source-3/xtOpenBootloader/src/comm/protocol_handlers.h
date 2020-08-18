/**
 * @file
 *
 * @brief Communication protocol handlers
 *
 */

#ifndef PROTOCOL_HANDLERS_H_
#define PROTOCOL_HANDLERS_H_

#include "compiler.h"
#include <stdint.h>

/* 
 * @brief Bootloader's protocol packets
 */
enum {
	PROTO_DEVICE_INFO= 0x00,                  ///< Device information
	PROTO_PAGE_SIZE= 0x01,                    ///< Flash page size information
	PROTO_PAGE_CNT= 0x02,                     ///< Number of flash pages
	PROTO_WRITE_PAGE = 0x10,                  ///< Write page
	PROTO_START_APP = 0x20,                   ///< Start application
	PROTO_RSP_SUCCESS = 0xE0,                 ///< Success response
	PROTO_RSP_ERROR = 0xE1,                   ///< Error response
	PROTO_ECHO = 0xEF,                        ///< Echo response
};

/**
 * @brief Callback to handle protocol packets 
 */
void handleProtocolPacket(
	void * userData,                          ///< Pointer to user data
	const unsigned char * data,               ///< Pointer to packed data
	unsigned int length                       ///< Length of the packet
);
/**
 * @brief Callback to handle erroneous protocol packets 
 */
void handleProtocolError(
	void * userData,                          ///< Pointer to user data
	unsigned int error                        ///< Error code
);
/**
 * @brief Handles protocol handlers 
 */
void handle_packet(
	uint8_t *pkt,                             ///< Pointer to packet data
	uint16_t length                           ///< Length of the packet
);
/**
 * @brief Device info request handler 
 */
void send_device_info(void);
/**
 * @brief Flash page size request handler 
 */
void send_page_size(void);
/**
 * @brief Flash page count request handler 
 */
void send_page_count(void);
/**
 * @brief Function sends response packet 
 */
void send(
	const unsigned char * data,               ///< Packet's data
	unsigned int length                       ///< Length of the packet
);
/**
 * @brief Function writes flash memory page 
 */
void write_single_page(
	uint16_t page,                            ///< Page number
	const uint8_t * pkt                       ///< Data to write
);
/**
 * @brief Write page request handler 
 */
void handle_proto_write_page(
	uint8_t *pkt                              ///< Pointer to packet data
);
/**
 * @brief Start application request handler 
 */
void handle_start_app(void);

#endif /* PROTOCOL_HANDLERS_H_ */