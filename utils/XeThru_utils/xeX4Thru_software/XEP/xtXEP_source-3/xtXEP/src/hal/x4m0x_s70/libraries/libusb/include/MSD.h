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
 * \section Purpose
 *
 * Mass Storage class definitions.
 *
 * See
 * - <a
 * href="http://www.usb.org/developers/devclass_docs/usb_msc_overview_1.2.pdf"> 
 * USB Mass Storage Class Spec. Overview</a>
 * - <a href="http://www.usb.org/developers/devclass_docs/usbmassbulk_10.pdf"> 
 * USB Mass Storage Class Bulk-Only Transport</a>
 *
 * \section Usage
 *
 * -# Uses "MSD Requests" to check incoming requests from USB Host.
 * -# Uses "MSD Subclass Codes" and "MSD Protocol Codes" to fill %device
 *    interface descriptors for a MSD %device.
 * -# Handle the incoming Bulk data with "MSD CBW Definitions" and MSCbw
 *    structure.
 * -# Prepare the outgoing Bulk data with "MSD CSW Definitions" and MSCsw
 *    structure.
 */

#ifndef MSD_H
#define MSD_H

/** \addtogroup usbd_msd
 *@{
 */

/*------------------------------------------------------------------------------
 *      Includes
 *------------------------------------------------------------------------------*/

#include <stdint.h>

/*------------------------------------------------------------------------------
 *      Definitions
 *------------------------------------------------------------------------------*/

/*
 * MSD Requests
 * This section lists MSD-specific requests ( Actually for Bulk-only protocol ).
 *
 * \section Requests
 * - MSD_BULK_ONLY_RESET
 * - MSD_GET_MAX_LUN
 */

/** Reset the mass storage %device and its associated interface. */
#define MSD_BULK_ONLY_RESET                     0xFF
/** Return the maximum LUN number supported by the %device. */
#define MSD_GET_MAX_LUN                         0xFE

/** \addtogroup usbd_msd_subclass MSD Subclass Codes
 *      @{
 * This page lists the Subclass Codes for bInterfaceSubClass field.
 * (Table 2.1, USB Mass Storage Class Spec. Overview)
 *
 * \section SubClasses
 * - MSD_SUBCLASS_RBC
 * - MSD_SUBCLASS_SFF_MCC
 * - MSD_SUBCLASS_QIC
 * - MSD_SUBCLASS_UFI
 * - MSD_SUBCLASS_SFF
 * - MSD_SUBCLASS_SCSI
 */

/** Reduced Block Commands (RBC) T10 */
#define MSD_SUBCLASS_RBC                        0x01
/** C/DVD devices */
#define MSD_SUBCLASS_SFF_MCC                    0x02
/** Tape device */
#define MSD_SUBCLASS_QIC                        0x03
/** Floppy disk drive (FDD) device */
#define MSD_SUBCLASS_UFI                        0x04
/** Floppy disk drive (FDD) device */
#define MSD_SUBCLASS_SFF                        0x05
/** SCSI transparent command set */
#define MSD_SUBCLASS_SCSI                       0x06
/**      @} */


/** \addtogroup usbd_msd_protocol_codes MSD Protocol Codes
 *      @{
 * This page lists the Transport Protocol codes for MSD.
 * (Table 3.1, USB Mass Storage Class Spec. Overview)
 *
 * \section Protocols
 * - MSD_PROTOCOL_CBI_COMPLETION
 * - MSD_PROTOCOL_CBI
 * - MSD_PROTOCOL_BULK_ONLY
 */

/** Control/Bulk/Interrupt (CBI) Transport (with command complete interrupt) */
#define MSD_PROTOCOL_CBI_COMPLETION             0x00
/** Control/Bulk/Interrupt (CBI) Transport (no command complete interrupt) */
#define MSD_PROTOCOL_CBI                        0x01
/** Bulk-Only Transport */
#define MSD_PROTOCOL_BULK_ONLY                  0x50
/**      @}*/

/** \addtogroup usbd_msd_cbw_def MSD CBW Definitions
 *      @{
 * This page lists the Command Block Wrapper (CBW) definitions.
 *
 * \section Constants
 * - MSD_CBW_SIZE
 * - MSD_CBW_SIGNATURE
 *
 * \section Fields
 * - MSD_CBW_DEVICE_TO_HOST
 */

/** Command Block Wrapper Size */
#define MSD_CBW_SIZE                            31
/** 'USBC' 0x43425355 */
#define MSD_CBW_SIGNATURE                       0x43425355

/** CBW bmCBWFlags field */
#define MSD_CBW_DEVICE_TO_HOST                  (1 << 7)
#define MSD_CBW_DEVICE_TO_DEVICE                (0 << 7)
/**      @}*/

/** \addtogroup usbd_msd_csw_def MSD CSW Definitions
 *      @{
 * This page lists the Command Status Wrapper (CSW) definitions.
 *
 * \section Constants
 * - MSD_CSW_SIZE
 * - MSD_CSW_SIGNATURE
 *
 * \section Command Block Status Values
 * (Table 5.3 , USB Mass Storage Class Bulk-Only Transport)
 * - MSD_CSW_COMMAND_PASSED
 * - MSD_CSW_COMMAND_FAILED
 * - MSD_CSW_PHASE_ERROR
 */

/** Command Status Wrapper Size */
#define MSD_CSW_SIZE                            13
/** 'USBS' 0x53425355 */
#define MSD_CSW_SIGNATURE                       0x53425355

/** Command Passed (good status) */
#define MSD_CSW_COMMAND_PASSED                  0
/** Command Failed */
#define MSD_CSW_COMMAND_FAILED                  1
/** Phase Error */
#define MSD_CSW_PHASE_ERROR                     2
/**      @}*/


/*------------------------------------------------------------------------------
 *      Structures
 *------------------------------------------------------------------------------*/
COMPILER_PACK_SET(1)
/*------------------------------------------------------------------------------
 * Command Block Wrapper (CBW),
 * See Table 5.1, USB Mass Storage Class Bulk-Only Transport.
 *
 * The CBW shall start on a packet boundary and shall end as a
 * short packet with exactly 31 (1Fh) bytes transferred.
 *------------------------------------------------------------------------------*/
typedef struct {

    /** 'USBC' 0x43425355 (little endian) */
    uint32_t  dCBWSignature;
    /** Must be the same as dCSWTag */
    uint32_t  dCBWTag;
    /** Number of bytes transfer */
    uint32_t  dCBWDataTransferLength;
    /** Indicates the directin of the transfer:
     *  - 0x80=IN=device-to-host;
     *  - 0x00=OUT=host-to-device
     */
    uint8_t bmCBWFlags;
    /** bits 0->3: bCBWLUN */
    uint8_t bCBWLUN   :4,
            bReserved1:4;           /** reserved */
    /** bits 0->4: bCBWCBLength */
    uint8_t bCBWCBLength:5,
            bReserved2  :3;         /** reserved */
    /** Command block */
    uint8_t pCommand[16];

} MSCbw;

/*------------------------------------------------------------------------------
 * Command Status Wrapper (CSW),
 * See Table 5.2, USB Mass Storage Class Bulk-Only Transport.
 *------------------------------------------------------------------------------*/
typedef struct
{
    /** 'USBS' 0x53425355 (little endian) */
    uint32_t  dCSWSignature;
    /** Must be the same as dCBWTag */
    uint32_t  dCSWTag;
    /**
     * For Data-Out the device shall report in the dCSWDataResidue the
     * difference between the amount of data expected as stated in the
     * dCBWDataTransferLength, and the actual amount of data processed by
     * the device. For Data-In the device shall report in the dCSWDataResidue
     * the difference between the amount of data expected as stated in the
     * dCBWDataTransferLength and the actual amount of relevant data sent by
     * the device. The dCSWDataResidue shall not exceed the value sent in the
     * dCBWDataTransferLength.
     */
    uint32_t  dCSWDataResidue;
    /** Indicates the success or failure of the command. */
    uint8_t bCSWStatus;

} MSCsw;

COMPILER_PACK_RESET()
/**@}*/
#endif /*#ifndef MSD_H */

