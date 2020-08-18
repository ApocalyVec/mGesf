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

/**\file
 *
 * \section Purpose
 *
 * Definitions used when declaring an HID report descriptors.
 *
 * \section Usage
 *
 * Use the definitions provided here when declaring a report descriptor,
 * which shall be an uint8_t array.
*/

#ifndef _HIDREPORTS_H_
#define _HIDREPORTS_H_
/** \addtogroup usb_hid
 *@{
 *  \addtogroup usb_hid_report USB HID Report
 *  @{
 */

/*----------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>

/*----------------------------------------------------------------------------
 *         Definitions
 *----------------------------------------------------------------------------*/

/** \addtogroup usb_hid_item HID Items Definitions
 *      @{
 */
/** Item size 0 bytes */
#define HIDItemSize_0B                          0
/** Item size 1 bytes */
#define HIDItemSize_1B                          1
/** Item size 2 bytes */
#define HIDItemSize_2B                          2
/** Item size 4 bytes */
#define HIDItemSize_4B                          3

/** Item type: Main */
#define HIDItemType_MAIN                        0
/** Item type: Global */
#define HIDItemType_GLOBAL                      1
/** Item type: Local */
#define HIDItemType_LOCAL                       2

/** Item prefix for long items */
#define HIDItem_LONGITEM                        0xFE
/**     @}*/

/** \addtogroup usb_hid_main HID Main Item Tags
 *      @{
 * This section lists the Main Item Tags defined for HID device.
 * ( HID Spec. 6.2.2.4 )
 *
 * - \ref HIDReport_INPUT
 * - \ref HIDReport_FEATURE
 * - \ref HIDReport_COLLECTION
 * - \ref HIDReport_ENDCOLLECTION
 */
/** Input item. */
#define HIDReport_INPUT                         0x80
/** Output item. */
#define HIDReport_OUTPUT                        0x90
/** Feature item. */
#define HIDReport_FEATURE                       0xB0
/** Collection item. */
#define HIDReport_COLLECTION                    0xA0
/** End of collection item. */
#define HIDReport_ENDCOLLECTION                 0xC0
/**     @}*/

/** \addtogroup usb_hid_data HID Items for Data Fields
 *      @{
 * This section lists definitions for HID Input, Output and Feature items that
 * are used to create the data fields within a report.
 * ( HID Spec. 6.2.2.5 )
 * - \ref HIDReport_CONSTANT
 * - \ref HIDReport_VARIABLE
 * - \ref HIDReport_RELATIVE
 * - \ref HIDReport_WRAP
 * - \ref HIDReport_NONLINEAR
 * - \ref HIDReport_NOPREFERRED
 * - \ref HIDReport_NULLSTATE
 * - \ref HIDReport_VOLATILE
 * - \ref HIDReport_BUFFEREDBYTES
 */
/** The report value is constant (vs. variable). */
#define HIDReport_CONSTANT                      (1 << 0)
/** Data reported is a variable (vs. array). */
#define HIDReport_VARIABLE                      (1 << 1)
/** Data is relative (vs. absolute). */
#define HIDReport_RELATIVE                      (1 << 2)
/** Value rolls over when it reach a maximum/minimum. */
#define HIDReport_WRAP                          (1 << 3)
/** Indicates that the data reported has been processed and is no longer */
/** linear with the original measurements. */
#define HIDReport_NONLINEAR                     (1 << 4)
/** Device has no preferred state to which it automatically returns. */
#define HIDReport_NOPREFERRED                   (1 << 5)
/** Device has a null state, in which it does not report meaningful */
/** information. */
#define HIDReport_NULLSTATE                     (1 << 6)
/** Indicates data can change without the host intervention. */
#define HIDReport_VOLATILE                      (1 << 7)
/** Indicates the device produces a fixed-length stream of bytes. */
#define HIDReport_BUFFEREDBYTES                 (1 << 8)
/**     @}*/

/** \addtogroup usb_hid_collection HID Collection Items
 *      @{
 * This section lists definitions for HID Collection Items.
 * ( HID Spec. 6.2.2.6 )
 * - \ref HIDReport_COLLECTION_PHYSICAL
 * - \ref HIDReport_COLLECTION_APPLICATION
 * - \ref HIDReport_COLLECTION_LOGICAL
 * - \ref HIDReport_COLLECTION_REPORT
 * - \ref HIDReport_COLLECTION_NAMEDARRAY
 * - \ref HIDReport_COLLECTION_USAGESWITCH
 * - \ref HIDReport_COLLECTION_USAGEMODIFIER
 */
/** Physical collection. */
#define HIDReport_COLLECTION_PHYSICAL           0x00
/** Application collection. */
#define HIDReport_COLLECTION_APPLICATION        0x01
/** Logical collection. */
#define HIDReport_COLLECTION_LOGICAL            0x02
/** Report collection. */
#define HIDReport_COLLECTION_REPORT             0x03
/** Named array collection. */
#define HIDReport_COLLECTION_NAMEDARRAY         0x04
/** Usage switch collection. */
#define HIDReport_COLLECTION_USAGESWITCH        0x05
/** Usage modifier collection */
#define HIDReport_COLLECTION_USAGEMODIFIER      0x06
/**     @}*/

/** \addtogroup usb_hid_global HID Global Items
 *      @{
 * This section lists HID Global Items.
 * ( HID Spec. 6.2.2.7 )
 * - \ref HIDReport_GLOBAL_USAGEPAGE
 * - \ref HIDReport_GLOBAL_LOGICALMINIMUM
 * - \ref HIDReport_GLOBAL_LOGICALMAXIMUM
 * - \ref HIDReport_GLOBAL_PHYSICALMINIMUM
 * - \ref HIDReport_GLOBAL_PHYSICALMAXIMUM
 * - \ref HIDReport_GLOBAL_UNITEXPONENT
 * - \ref HIDReport_GLOBAL_UNIT
 * - \ref HIDReport_GLOBAL_REPORTSIZE
 * - \ref HIDReport_GLOBAL_REPORTID
 * - \ref HIDReport_GLOBAL_REPORTCOUNT
 * - \ref HIDReport_GLOBAL_PUSH
 * - \ref HIDReport_GLOBAL_POP
 */
/** Current usage page. */
#define HIDReport_GLOBAL_USAGEPAGE              0x04
/** Minimum value that a variable or array item will report. */
#define HIDReport_GLOBAL_LOGICALMINIMUM         0x14
/** Maximum value that a variable or array item will report. */
#define HIDReport_GLOBAL_LOGICALMAXIMUM         0x24
/** Minimum value for the physical extent of a variable item. */
#define HIDReport_GLOBAL_PHYSICALMINIMUM        0x34
/** Maximum value for the physical extent of a variable item. */
#define HIDReport_GLOBAL_PHYSICALMAXIMUM        0x44
/** Value of the unit exponent in base 10. */
#define HIDReport_GLOBAL_UNITEXPONENT           0x54
/** Unit values. */
#define HIDReport_GLOBAL_UNIT                   0x64
/** Size of the report fields in bits. */
#define HIDReport_GLOBAL_REPORTSIZE             0x74
/** Specifies the report ID. */
#define HIDReport_GLOBAL_REPORTID               0x84
/** Number of data fields for an item. */
#define HIDReport_GLOBAL_REPORTCOUNT            0x94
/** Places a copy of the global item state table on the stack. */
#define HIDReport_GLOBAL_PUSH                   0xA4
/** Replaces the item state table with the top structure from the stack. */
#define HIDReport_GLOBAL_POP                    0xB4
/**     @}*/

/** \addtogroup usb_hid_local HID Local Items
 *      @{
 * This section lists definitions for HID Local Items.
 * ( HID Spec. 6.2.2.8 )
 * - \ref HIDReport_LOCAL_USAGE
 * - \ref HIDReport_LOCAL_USAGEMINIMUM
 * - \ref HIDReport_LOCAL_USAGEMAXIMUM
 * - \ref HIDReport_LOCAL_DESIGNATORINDEX
 * - \ref HIDReport_LOCAL_DESIGNATORMINIMUM
 * - \ref HIDReport_LOCAL_DESIGNATORMAXIMUM
 * - \ref HIDReport_LOCAL_STRINGINDEX
 * - \ref HIDReport_LOCAL_STRINGMINIMUM
 * - \ref HIDReport_LOCAL_STRINGMAXIMUM
 * - \ref HIDReport_LOCAL_DELIMITER
 */
/** Suggested usage for an item or collection. */
#define HIDReport_LOCAL_USAGE                   0x08
/** Defines the starting usage associated with an array or bitmap. */
#define HIDReport_LOCAL_USAGEMINIMUM            0x18
/** Defines the ending usage associated with an array or bitmap. */
#define HIDReport_LOCAL_USAGEMAXIMUM            0x28
/** Determines the body part used for a control. */
#define HIDReport_LOCAL_DESIGNATORINDEX         0x38
/** Defines the index of the starting designator associated with an array or */
/** bitmap. */
#define HIDReport_LOCAL_DESIGNATORMINIMUM       0x48
/** Defines the index of the ending designator associated with an array or */
/** bitmap. */
#define HIDReport_LOCAL_DESIGNATORMAXIMUM       0x58
/** String index for a string descriptor. */
#define HIDReport_LOCAL_STRINGINDEX             0x78
/** Specifies the first string index when assigning a group of sequential */
/** strings to controls in an array or bitmap. */
#define HIDReport_LOCAL_STRINGMINIMUM           0x88
/** Specifies the last string index when assigning a group of sequential */
/** strings to controls in an array or bitmap. */
#define HIDReport_LOCAL_STRINGMAXIMUM           0x98
/** Defines the beginning or end of a set of local items. */
#define HIDReport_LOCAL_DELIMITER               0xA8
/**     @}*/

/*----------------------------------------------------------------------------
 *         Types
 *----------------------------------------------------------------------------*/

#pragma pack(1)
#if defined   ( __CC_ARM   ) /* Keil ��Vision 4 */
#elif defined ( __ICCARM__ ) /* IAR Ewarm 5.41+ */
#define __attribute__(...)
#elif defined (  __GNUC__  ) /* GCC CS3 2009q3-68 */
#endif

/** HID Short Item Header, followed by bSize bytes of data */
typedef struct _HIDShortItem {
    uint8_t bSize:2,    /**< data size (0, 1, 2 or 4) */
            bType:2,    /**< fundamental type */
            bTag:4;     /**< item type */
} HIDShortItem;

/** HID Long Item Header, followed by bDataSize bytes of data */
typedef struct _HIDLongItem {
    uint8_t  bPrefix;       /**< Prefix, 0xFE */
    uint8_t  bDataSize;     /**< data size */
    uint16_t bLongItemTag;  /**< item type */
} HIDLongItem;

/** HID Report without ID (with one byte data) */
typedef struct _HIDReportNoID {
    uint8_t  bData[1];      /**< First report data byte */
} HIDReportNoID;

/** HID Report with ID (with one byte data) */
typedef struct _HIDReport {
    uint8_t  bID;           /**< Report ID */
    uint8_t  bData[1];      /**< First report data byte */
} HIDReport;

#pragma pack()

/*----------------------------------------------------------------------------
 *         Functions
 *----------------------------------------------------------------------------*/

/** @}*/
/**@}*/
#endif /*#ifndef _HIDREPORTS_H_ */

