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
 *  Implementation of the HIDDKeyboardInputReport class.
 */

/** \addtogroup usbd_hid_key
 *@{
 */

/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/

#include <HIDDKeyboard.h>
#include <USBLib_Trace.h>

/*------------------------------------------------------------------------------
 *         Exported functions
 *------------------------------------------------------------------------------*/

/**
 * Initializes a keyboard input report instance.
 * \param report Pointer to a HIDDKeyboardInputReport instance.
 */
void HIDDKeyboardInputReport_Initialize(HIDDKeyboardInputReport *report)
{
	uint32_t i;

	report->bmModifierKeys = 0;

	for (i = 0; i < HIDDKeyboardInputReport_MAXKEYPRESSES; i++)
		report->pressedKeys[i] = 0;
}

/**
 * Reports a standard key as being pressed.
 * \param report Pointer to a HIDDKeyboardInputReport instance.
 * \param key Key code of the standard key.
 */
void HIDDKeyboardInputReport_PressStandardKey(HIDDKeyboardInputReport *report,
		uint8_t key)
{
	/* Find first available slot */
	uint32_t i = 0;
	uint8_t found = 0;

	while ((i < HIDDKeyboardInputReport_MAXKEYPRESSES) && !found) {

		/* Free slot: no key referenced (code = 0) or ErrorRollOver */
		if ((report->pressedKeys[i] == 0)
			|| (report->pressedKeys[i] == HIDKeypad_ERRORROLLOVER)) {

			found = 1;
			report->pressedKeys[i] = key;
		}

		i++;
	}

	/* Report ErrorRollOver in all fields if too many keys are pressed */
	if (!found) {

		for (i = 0; i < HIDDKeyboardInputReport_MAXKEYPRESSES; i++)

			report->pressedKeys[i] = HIDKeypad_ERRORROLLOVER;
	}
}

/**
 * Reports a standard key as not being pressed anymore.
 * \param report Pointer to a HIDDKeyboardInputReport instance.
 * \param key Key code of the standard key
 */
void HIDDKeyboardInputReport_ReleaseStandardKey(HIDDKeyboardInputReport *report,
		uint8_t key)
{
	/* Look for key in array */
	uint32_t i = 0;
	uint8_t found = 0;

	while ((i < HIDDKeyboardInputReport_MAXKEYPRESSES) && !found) {

		if (report->pressedKeys[i] == key) {

			found = 1;
			report->pressedKeys[i] = 0;
		}

		i++;
	}
}

/**
 * Reports a modifier key as being currently pressed.
 * \param report Pointer to a HIDDKeyboardInputReport instance.
 * \param key Key code of the modifier key.
 */
void HIDDKeyboardInputReport_PressModifierKey(HIDDKeyboardInputReport *report,
		uint8_t key)
{
	/* Set corresponding bit */
	uint8_t bit = key - HIDDKeyboardDescriptors_FIRSTMODIFIERKEY;
	report->bmModifierKeys |= 1 << bit;
}

/**
 * Reports a modifier key as not being pressed anymore.
 * \param report Pointer to a HIDDKeyboardInputReport instance.
 * \param key Key code of the modifier key.
 */
void HIDDKeyboardInputReport_ReleaseModifierKey(HIDDKeyboardInputReport *report,
		uint8_t key)
{
	/* Clear corresponding bit */
	uint8_t bit = key - HIDDKeyboardDescriptors_FIRSTMODIFIERKEY;
	report->bmModifierKeys &= ~(1 << bit);
}

/**@}*/
