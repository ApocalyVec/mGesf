/**
 * \file
 *
 * \brief Configurations for Widget Toolkit library
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
#ifndef CONF_WTK_H
#define CONF_WTK_H

/**
 * \weakgroup wtk_group
 * @{
 */

/** Datatype holding the length of strings. */
typedef uint16_t wtk_string_size_t;

/**
 * \ingroup wtk_frame_group
 * @{
 */

/** Left frame border size. */
#define WTK_FRAME_LEFTBORDER              2

/** Right frame border size. */
#define WTK_FRAME_RIGHTBORDER             2

/** Top frame border size. */
#define WTK_FRAME_TOPBORDER               2

/** Bottom frame border size. */
#define WTK_FRAME_BOTTOMBORDER            4

/** Radius of resize handle (second quadrant of circle). */
#define WTK_FRAME_RESIZE_RADIUS           7

/** Width of resize handle. */
#define WTK_FRAME_RESIZE_WIDTH            (WTK_FRAME_RESIZE_RADIUS + 1)

/** Height of resize handle. */
#define WTK_FRAME_RESIZE_HEIGHT           (WTK_FRAME_RESIZE_RADIUS + 1)

/** Minimum width of contents window of a frame. */
#define WTK_FRAME_MIN_WIDTH  (WTK_FRAME_RESIZE_WIDTH - WTK_FRAME_RIGHTBORDER)

/** Minimum height of contents window of a frame. */
#define WTK_FRAME_MIN_HEIGHT (WTK_FRAME_RESIZE_HEIGHT - WTK_FRAME_BOTTOMBORDER)

/** Height of titlebar area. */
#define WTK_FRAME_TITLEBAR_HEIGHT         9

/** Caption text X offset inside title bar. */
#define WTK_FRAME_CAPTION_X               3

/** Caption text Y offset inside title bar. */
#define WTK_FRAME_CAPTION_Y               1

/** Background color of frame contents window. */
#define WTK_FRAME_BACKGROUND_COLOR        GFX_COLOR_GRAY

/** Frame border color. */
#define WTK_FRAME_BORDER_COLOR            GFX_COLOR(240, 240, 240)

/** Background color of frame title bar. */
#define WTK_FRAME_TITLEBAR_COLOR          GFX_COLOR(128, 128, 255)

/** Caption text foreground color. */
#define WTK_FRAME_CAPTION_COLOR           GFX_COLOR_WHITE

/** Resize handle color. */
#define WTK_FRAME_RESIZE_COLOR            GFX_COLOR(64, 192, 192)

/** Distance from origin before dragging operation kicks in. */
#define WTK_DRAG_THRESHOLD                4

/** Radius of drag origin and target handles. */
#define WTK_DRAG_HANDLE_RADIUS            4

/** Pixmap pixel count for storing drag handle background graphics. */
#define WTK_DRAG_PIXMAP_SIZE              (WTK_DRAG_HANDLE_RADIUS * 2 + 1)

/** Drag origin handle color. */
#define WTK_DRAG_ORIGIN_COLOR             GFX_COLOR_RED

/** Drag target handle color. */
#define WTK_DRAG_TARGET_COLOR             GFX_COLOR_GREEN

/** @} */

/**
 * \ingroup wtk_button_group
 * @{
 */

/** Button border color. */
#define WTK_BUTTON_BORDER_COLOR           GFX_COLOR_BLACK

/** Color of button background (text color when highlighted). */
#define WTK_BUTTON_BACKGROUND_COLOR       GFX_COLOR_GRAY

/** Color of button text (background color when highlighted). */
#define WTK_BUTTON_CAPTION_COLOR          GFX_COLOR_WHITE

/** @} */

/**
 * \ingroup wtk_check_box_group
 * @{
 */

/** Size of checkbox square. */
#define WTK_CHECKBOX_BOX_SIZE             12

/** Checkbox square X offset. */
#define WTK_CHECKBOX_BOX_X                0

/** Checkbox square Y offset. */
#define WTK_CHECKBOX_BOX_Y                0

/** Checkbox caption text X offset. */
#define WTK_CHECKBOX_CAPTION_X            (WTK_CHECKBOX_BOX_SIZE + 4)

/** Checkbox caption text Y offset. */
#define WTK_CHECKBOX_CAPTION_Y            0

/** Checkbox square frame color. */
#define WTK_CHECKBOX_BOX_COLOR            GFX_COLOR_BLACK

/** Checkbox square background color. */
#define WTK_CHECKBOX_BACKGROUND_COLOR     GFX_COLOR_WHITE

/** Checkbox square select mark color. */
#define WTK_CHECKBOX_SELECT_COLOR         GFX_COLOR_DK_RED

/** Checkbox caption text color. */
#define WTK_CHECKBOX_CAPTION_COLOR        GFX_COLOR_BLACK

/** @} */

/**
 * \ingroup wtk_radio_button_group
 * @{
 */

/** Datatype holding reference count for radio button groups. */
typedef uint8_t wtk_radio_group_size_t;

/** Radius of radio button circle. */
#define WTK_RADIOBUTTON_RADIUS            6

/** Radio button circle center X offset. */
#define WTK_RADIOBUTTON_BUTTON_X          (WTK_RADIOBUTTON_RADIUS)

/** Radio button circle center Y offset. */
#define WTK_RADIOBUTTON_BUTTON_Y          (WTK_RADIOBUTTON_RADIUS)

/** Radio button caption text X offset. */
#define WTK_RADIOBUTTON_CAPTION_X         ((WTK_RADIOBUTTON_RADIUS * 2) + 4)

/** Radio button caption text Y offset. */
#define WTK_RADIOBUTTON_CAPTION_Y         0

/** Radio button circle color. */
#define WTK_RADIOBUTTON_BUTTON_COLOR      GFX_COLOR_BLACK

/** Radio button circle background color. */
#define WTK_RADIOBUTTON_BACKGROUND_COLOR  GFX_COLOR_WHITE

/** Radio button select mark color. */
#define WTK_RADIOBUTTON_SELECT_COLOR      GFX_COLOR_DK_RED

/** Radio button caption text color. */
#define WTK_RADIOBUTTON_CAPTION_COLOR     GFX_COLOR_BLACK

/** @} */

/**
 * \ingroup wtk_slider_group
 * @{
 */

/**
 * \brief Move support for sliders
 *
 * This define enables and disables move support for sliders. Move support
 * is needed if the application is using moving parent widgets/windows.
 * If not needed, disable it to save CPU cycles.
 *
 * Set to \c true to enable move support or \c false to disable move support.
 */
#define WTK_SLIDER_PARENT_MOVE_SUPPORT    false

/** Width of the slider knob, including borders, in pixels. */
#define WTK_SLIDER_KNOB_WIDTH             10

/** Color of all the slider's borders. */
#define WTK_SLIDER_BORDER_COLOR           GFX_COLOR_BLACK

/** Color of the slider's background. */
#define WTK_SLIDER_BACKGROUND_COLOR       GFX_COLOR_GRAY

/** Color of the slider knob when normal. */
#define WTK_SLIDER_KNOB_COLOR_NORMAL      GFX_COLOR_RED

/** Color of the slider knob when moving. */
#define WTK_SLIDER_KNOB_COLOR_MOVING      GFX_COLOR_WHITE

/** @} */

/**
 * \ingroup wtk_progress_bar_group
 * @{
 */

/** Color of the progress bar's frame border. */
#define WTK_PROGRESS_BAR_BORDER_COLOR     GFX_COLOR_BLACK

/** @} */

/**
 * \ingroup wtk_icon_button_group
 * @{
 */

/** Datatype holding reference count for icon button groups. */
typedef uint8_t wtk_icon_group_size_t;

/** Icon button select mark color. */
#define WTK_ICONBUTTON_SELECT_COLOR       GFX_COLOR_BLUE

/** @} */

/**
* \ingroup gfx_wtk_plot
* @{
*/

/** Color of the plot's frame border. */
#define WTK_PLOT_BORDER_COLOR             GFX_COLOR_BLACK

/** Scale factor for fixed-point calculation. */
#define WTK_PLOT_SCALE_FACTOR             128

/** The length of the tick marker. */
#define WTK_PLOT_TICK_MARKER_LENGTH       5

/** @} */

/** @} */

#endif /* CONF_WTK_H */
