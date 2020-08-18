/**
 * \file
 *
 * \brief Graphic library API header file
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef GFX_TEXT_H_INCLUDED
#define GFX_TEXT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "compiler.h"
#ifdef CONFIG_HAVE_HUGEMEM
# include "hugemem.h"
#endif
#if XMEGA
# include "progmem.h"
#endif

/**
 * \ingroup gfx_group
 * \defgroup gfx_font_group Font support
 * This modules provides functionality for outputting a font to a display.
 *
 * @{
 */

/**
 * \brief Valid storage locations for font data
 *
 * \todo Add support for fonts in regular ram
 */
enum font_data_type {
	/*! Font data stored in program/flash memory. */
	FONT_LOC_PROGMEM,
#ifdef CONFIG_HAVE_HUGEMEM
	/*! Font data stored in HUGEMEM. */
	FONT_LOC_HUGEMEM,
#endif
};

/** Text alignment options for text lines within a paragraph. */
enum gfx_text_alignment {
	/**< Align each line to the left side of the paragraph bounding box */
	TEXT_ALIGN_LEFT,
	/**< Align each line to the right side of the paragraph bounding box */
	TEXT_ALIGN_RIGHT,
	/**< Align each line to the center of the paragraph bounding box */
	TEXT_ALIGN_CENTER,
};

/** Text paraphraph positioning options. */
enum gfx_text_position {
	/**< Position text paragraph vertically below given coordinate */
	TEXT_POS_TOP        = (1 << 0),	
	/**< Position text paragraph vertically centered on given coordinate */
	TEXT_POS_CENTER_Y   = (1 << 1),
	/**< Position text paragraph vertically above given coordinate */
	TEXT_POS_BOTTOM     = (1 << 2),
	/**< Position text paragraph horizontally right to the given coordinate */
	TEXT_POS_LEFT       = (1 << 3),
	/**< Position text paragraph horizontally centered on given coordinate */
	TEXT_POS_CENTER_X   = (1 << 4),
	/**< Position text paragraph horizontally left to the given coordinate */
	TEXT_POS_RIGHT      = (1 << 5),
	
	TEXT_POS_TOP_LEFT      = TEXT_POS_TOP      | TEXT_POS_LEFT,
	TEXT_POS_TOP_CENTER    = TEXT_POS_TOP      | TEXT_POS_CENTER_X,
	TEXT_POS_TOP_RIGHT     = TEXT_POS_TOP      | TEXT_POS_RIGHT,
	TEXT_POS_CENTER_LEFT   = TEXT_POS_CENTER_Y | TEXT_POS_LEFT,
	TEXT_POS_CENTER        = TEXT_POS_CENTER_Y | TEXT_POS_CENTER_X,
	TEXT_POS_CENTER_RIGHT  = TEXT_POS_CENTER_Y | TEXT_POS_RIGHT,
	TEXT_POS_BOTTOM_LEFT   = TEXT_POS_BOTTOM   | TEXT_POS_LEFT,
	TEXT_POS_BOTTOM_CENTER = TEXT_POS_BOTTOM   | TEXT_POS_CENTER_X,
	TEXT_POS_BOTTOM_RIGHT  = TEXT_POS_BOTTOM   | TEXT_POS_RIGHT,
};

/** Storage structure for font meta data. */
struct font {
	/*! Type of storage used for binary font data. See \ref font_data_type. **/
	enum font_data_type type;
	union {
		/**
		 * Pointer to where the binary font data is stored. This
		 * variable is accessed either through hugemem or progmem
		 * depending on the value of \a type.
		 */
#ifdef CONFIG_HAVE_HUGEMEM
		hugemem_ptr_t hugemem;
#endif
		uint8_t PROGMEM_PTR_T progmem;
	} data;
	
	/*! Width of one font character, in pixels. */
	uint8_t width;
	/*! Height of one font character, in pixels. */
	uint8_t height;
	/*! ASCII value of first character in font set. */
	uint8_t first_char;
	/*! ASCII value of last character in the set. */
	uint8_t last_char;
};

/** \name Strings and characters located in RAM */
/** @{ */
void gfx_draw_char(const char c, const gfx_coord_t x, const gfx_coord_t y,
		const struct font *font, const gfx_color_t bg_color,
		const gfx_color_t text_color);

void gfx_draw_string(const char *str, const gfx_coord_t x,
		const gfx_coord_t y, const struct font *font,
		const gfx_color_t bg_color, const gfx_color_t text_color);

void gfx_draw_string_aligned(const char *str, gfx_coord_t x, gfx_coord_t y,
		const struct font *font, const gfx_color_t bg_color,
		const gfx_color_t text_color, enum gfx_text_position text_pos,
		enum gfx_text_alignment text_align);

void gfx_get_string_bounding_box(char const *str, const struct font *font,
		gfx_coord_t *width, gfx_coord_t *height);
/** @} */

/** \name Strings located in flash
 * @{
 */
void gfx_draw_progmem_string(char PROGMEM_PTR_T str, gfx_coord_t x,
		gfx_coord_t y, const struct font *font,
		const gfx_color_t bg_color, const gfx_color_t text_color);

void gfx_draw_progmem_string_aligned(char PROGMEM_PTR_T str,
		gfx_coord_t x, gfx_coord_t y, const struct font *font,
		const gfx_color_t bg_color, const gfx_color_t text_color,
		enum gfx_text_position text_pos, enum gfx_text_alignment text_align);

void gfx_get_progmem_string_bounding_box(char PROGMEM_PTR_T str,
		const struct font *font, gfx_coord_t *width,
		gfx_coord_t *height);
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* GFX_TEXT_H_INCLUDED */
