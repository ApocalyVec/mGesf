/* ---------------------------------------------------------------------------- */
/*                  Atmel Microcontroller Software Support                      */
/*                       SAM Software Package License                           */
/* ---------------------------------------------------------------------------- */
/* Copyright (c) 2015, Atmel Corporation                                        */
/*                                                                              */
/* All rights reserved.                                                         */
/*                                                                              */
/* Redistribution and use in source and binary forms, with or without           */
/* modification, are permitted provided that the following condition is met:    */
/*                                                                              */
/* - Redistributions of source code must retain the above copyright notice,     */
/* this list of conditions and the disclaimer below.                            */
/*                                                                              */
/* Atmel's name may not be used to endorse or promote products derived from     */
/* this software without specific prior written permission.                     */
/*                                                                              */
/* DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR   */
/* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE   */
/* DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,      */
/* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT */
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,  */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    */
/* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING         */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, */
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/* ---------------------------------------------------------------------------- */

/** \file
 *
 *  Definitions and classes for USB Audio class descriptors.
 *
 */

#ifndef _VIDEODESCRIPTORS_H_
#define _VIDEODESCRIPTORS_H_
/** \addtogroup usb_video_class
 *@{
 */

/*----------------------------------------------------------------------------
 *         Includes
 *----------------------------------------------------------------------------*/
 
#include <stdint.h>
#include <USBVideo.h>
/*----------------------------------------------------------------------------
 *         Definitions
 *----------------------------------------------------------------------------*/

/** Video frame BitsPerPixel */
#define FRAME_BPP         (16)
/** Video frame buffer size calculation */
#define FRAME_BUFFER_SIZEC(W,H)  ((W)*(H)*FRAME_BPP/8)
/** Video frame bit-rate calculation */
#define FRAME_BITRATEC(W,H,FR)  ((FR)*FRAME_BUFFER_SIZEC(W,H)*8)
/** Video frame interval calculation (100ns) */
#define FRAME_INTERVALC(FR) (1*1000*1000*(1000/100)/(FR))
/** Packet size for FS */
#define FRAME_PACKET_SIZE_FS    640

/** Packet size for HS */
#define FRAME_PACKET_SIZE_HS    (1020)

/** Payload header size */
#define FRAME_PAYLOAD_HDR_SIZE  2

/** High Bandwidth mode: 0 ~ 2 */
#define ISO_HIGH_BW_MODE    2

#define VIDCAMD_EpDesc_MaxPacketSize   (FRAME_PACKET_SIZE_HS | (ISO_HIGH_BW_MODE << 11))

/** Interface number of USB Video Control Interface */
#define VIDCAMD_ControlInterfaceNum     0
/** Interface number of USB Video Streaming Interface */
#define VIDCAMD_StreamInterfaceNum      1
/** Endpoint number of USB Video Streaming ISO IN endpoint */
#define VIDCAMD_IsoInEndpointNum        2

/** Number of Video Frame Types */
#define VIDCAMD_NumFrameTypes           3

#define VIDCAMD_FW_1                 320
#define VIDCAMD_FH_1                 240

#define VIDCAMD_FW_2                 640
#define VIDCAMD_FH_2                 480

#define VIDCAMD_FW_3                 176
#define VIDCAMD_FH_3                 144

/*----------------------------------------------------------------------------
 *         Types
 *----------------------------------------------------------------------------*/
#pragma pack(1)
#if defined   ( __CC_ARM   ) /* Keil ��Vision 4 */
#elif defined ( __ICCARM__ ) /* IAR Ewarm */
#define __attribute__(...)
#define __packed__  packed
#elif defined (  __GNUC__  ) /* GCC CS3 */
#define __packed__  aligned(1)
#endif
/**
 * VideoControl Interface with 1 streaming interface
 */
typedef struct _UsbVideoControlInterfaceHeader1 {
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint8_t     bDescriptorSubType;
    uint16_t    bcdVDC;
    uint16_t    wTotalLength;
    uint32_t    dwClockFrequency;
    uint8_t     bInCollection;
    uint8_t     bInterface1;
} UsbVideoControlInterfaceHeader1;

/**
 * Input header descriptor (with 1 format)
 */
typedef struct _UsbVideoInputHeaderDescriptor1 {
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint8_t     bDescriptorSubType;
    uint8_t     bNumFormats;
    uint16_t    wTotalLength;
    uint8_t     bEndpointAddress;
    uint8_t     bmInfo;
    uint8_t     bTerminalLink;
    uint8_t     bStillCaptureMethod;
    uint8_t     bTriggerSupport;
    uint8_t     bTriggerUsage;
    uint8_t     bControlSize;
    uint8_t     bmaControls1;
} UsbVideoInputHeaderDescriptor1;

/**
 * Class-specific USB VideoControl Interface descriptor list
 */
typedef struct _UsbVideoControlInterfaceDescriptor {
    UsbVideoControlInterfaceHeader1 header;
    USBVideoCameraTerminalDescriptor it;
    USBVideoOutputTerminalDescriptor ot;
} UsbVideoControlInterfaceDescriptor;

/** USB Video Format with 1 frame, 1 compression, without STI */
typedef struct _UsbVideoFormatDescriptor {
    USBVideoUncompressedFormatDescriptor payload;
    USBVideoUncompressedFrameDescriptor1 frame320x240; /* 153.6K/f,  4f/s */
    USBVideoUncompressedFrameDescriptor1 frame640x480; /* 614.4K/f,  1f/s */
    USBVideoUncompressedFrameDescriptor1 frame160x120; /* 614.4K/f,  16f/s */
    USBVideoColorMatchingDescriptor colorUncompressed;
} UsbVideoFormatDescriptor;

typedef struct _UsbVideoStreamingInterfaceDescriptor {
    UsbVideoInputHeaderDescriptor1 inHeader;
    UsbVideoFormatDescriptor format;
} UsbVideoStreamingInterfaceDescriptor;

struct UsbVideoCamConfigurationDescriptors {
    /* Configuration descriptor */
    USBConfigurationDescriptor configuration;
    /* IAD */
    USBInterfaceAssociationDescriptor iad;
    /* VideoControl I/F */
    USBInterfaceDescriptor interface0;
    /* VideoControl I/F Descriptors */
    UsbVideoControlInterfaceDescriptor vcInterface;
    /* VideoStreaming I/F */
    USBInterfaceDescriptor interface10;
    /* VideoStreaming I/F Descriptors */
    UsbVideoStreamingInterfaceDescriptor vsInterface;
    /* VideoStreaming I/F */
    USBInterfaceDescriptor interface11;
    /* Endpoint */
    USBEndpointDescriptor ep11;
};

#pragma pack()


/**@}*/
#endif /* _VIDEODESCRIPTORS_H_ */
