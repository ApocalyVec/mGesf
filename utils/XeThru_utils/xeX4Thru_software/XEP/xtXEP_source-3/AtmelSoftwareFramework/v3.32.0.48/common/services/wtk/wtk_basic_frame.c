/**
 * \file
 *
 * \brief Basic frame widget implementation
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
#include <stdint.h>
#include <stdlib.h>
#include <membag.h>
#include <string.h>
#include <wtk.h>

/**
 * \ingroup wtk_basic_frame_group
 * @{
 */

/**
 * \brief Basic frame control struct
 *
 * This is the basic frame control struct. It contains references to the window
 * composing the frame, as well as non-window information required.
 *
 * \internal
 * \note Structure fields are only internally accessible, only pointers to
 *       structure is needed externally.
 */
struct wtk_basic_frame {
	/** Frame window. */
	struct win_window *win;
	/** Frame command event handler. */
	wtk_basic_frame_command_handler_t frame_handler;
	/** Data for applications and handlers. */
	void *custom_data;
	/** Frame draw event handler. */
	wtk_basic_frame_draw_handler_t draw_handler;
};

/**
 * \brief Get reference to basic frame window.
 *
 * This function returns a reference to the window that should be used when
 * managing the widget, such as move, resize, destroy and reparenting.
 *
 * \param basic_frame Basic frame widget to manage.
 *
 * \return Window to be used for managing the basic frame.
 */
struct win_window *wtk_basic_frame_as_child(struct wtk_basic_frame *basic_frame)
{
	Assert(basic_frame);
	return basic_frame->win;
}

/**
 * \brief Get basic frame custom data
 *
 * This function returns the custom data from the basic frame's attributes.
 * The custom data can be used for e.g. linking to associated applications.
 *
 * \param basic_frame Pointer to basic frame.
 *
 * \return Copy of the custom data.
 */
void *wtk_basic_frame_get_custom_data(const struct wtk_basic_frame *basic_frame)
{
	Assert(basic_frame);
	return basic_frame->custom_data;
}

/**
 * \brief Basic frame event handler
 *
 * This function is the window event handler for basic frame widgets.
 * It handles all events sent to the windows composing the widget.
 *
 * \param win  Window receiving the event.
 * \param type The event type.
 * \param data Custom data, depending on event type.
 *
 * \return True if the event was recognized and accepted.
 */
static bool wtk_basic_frame_handler(struct win_window *win,
		enum win_event_type type, const void *data)
{
	/* Custom data for windows of a widget points back to the widget itself. */
	struct wtk_basic_frame *frame;
	bool should_destroy;
	struct win_clip_region const *clip;

	frame = win_get_custom_data(win);

	switch (type) {
	case WIN_EVENT_DRAW:

		/* For DRAW events, the data parameter points to the
		 * clipping region.
		 */
		clip = (struct win_clip_region const *)data;

		if (win == frame->win) {
			if (frame->draw_handler) {
				/* Call the draw handler */
				frame->draw_handler(win, clip);
			}
		}

		/* Always accept DRAW events, as the return value is
		 * ignored anyway for that event type.
		 */
		return true;

	case WIN_EVENT_POINTER:

		/* Accept all POINTER events so that it does not
		 * propagate up the window tree to our parent in case
		 * we did not click anything useful inside the frame.
		 */
		return true;

	case WIN_EVENT_DESTROY:

		/* When the frame window is destroyed, also destroy
		 * the rest of the non-window frame allocations.
		 */
		if (win == frame->win) {
			/* Memory allocated for windows will be
			 * automatically destroyed by the window
			 * system. We must destroy other allocations.
			 */
			membag_free(frame);
		}

		/* Always accept DESTROY events, as the return value is
		 * ignored anyway for that event type.
		 */
		return true;

	case WIN_EVENT_COMMAND:

		/* When commands are received either directly or
		 * propagated from child widgets and windows, send it
		 * to the frame handler.
		 */
		if (win == frame->win) {
			if (frame->frame_handler) {
				/* If the frame handler returns true,
				 * it wants us to destroy the frame.
				 * This is normally used by CLOSE
				 * buttons.
				 */
				should_destroy = frame->frame_handler(frame,
						(win_command_t)data);

				/* It is safe to destroy it here, since
				 * the event handling finished right
				 * after this handler returns, and no
				 * other references to this frame or
				 * its contents will be done.
				 */
				if (should_destroy) {
					win_destroy(frame->win);
				}

				/* Accept the event if there was a
				 * handler installed.
				 */
				return true;
			}
		}

		/* Reject the event if there was no handler, or this
		 * was not the container window at all.
		 */
		return false;

	default:
		/* Reject unknown event types. */
		return false;
	}
}

/**
 * \brief Create basic frame widget
 *
 * This function creates a new basic frame widget. It allocates required memory
 * and intializes necessary windows to create the widget. If there is not enough
 * memory, the function returns NULL.
 *
 * To destroy the widget and all its contents, and free its memory, call
 * win_destroy() on the basic frame's child reference, given by
 * wtk_basic_frame_as_child(), like this:
 * "win_destroy(wtk_basic_frame_as_child(my_frame_ptr));".
 *
 * \param parent         Parent window.
 * \param area           Area of the internal contents.
 * \param background     Pointer to background bitmap for frame. NULL for
 *                       transparent background. When background is transparent
 *                       the parent window will automatically be redrawn
 *                       when the basic frame is drawn.
 * \param draw_handler   Draw event handler. NULL for no handler.
 * \param frame_handler  Command event handler. NULL for no handler.
 * \param custom_data    Optional custom data link for application's command
 *                       events.
 *
 * \return Pointer to basic frame, or NULL if failed.
 */
struct wtk_basic_frame *wtk_basic_frame_create(struct win_window *parent,
		const struct win_area *area, struct gfx_bitmap *background,
		wtk_basic_frame_draw_handler_t draw_handler,
		wtk_basic_frame_command_handler_t frame_handler,
		void *custom_data)
{
	struct win_attributes attr;
	struct wtk_basic_frame *basic_frame;

	Assert(area);
	Assert(parent);

	/* Allocate memory for frame control data. */
	basic_frame = membag_alloc(sizeof(struct wtk_basic_frame));
	if (!basic_frame) {
		goto outofmem_basic_frame;
	}

	/* Set window attributes */
	basic_frame->frame_handler = frame_handler;
	basic_frame->custom_data = custom_data;
	basic_frame->draw_handler = draw_handler;

	/* Copy area info */
	attr.area = *area;
	attr.event_handler = wtk_basic_frame_handler;
	attr.custom = basic_frame;

	/* Set background for window */
	if (background) {
		attr.background = background;
		attr.behavior = 0;
	} else {
		attr.background = NULL;
		attr.behavior = WIN_BEHAVIOR_REDRAW_PARENT;
	}

	/* Create the window */
	basic_frame->win = win_create(parent, &attr);
	if (!basic_frame->win) {
		goto outofmem_win;
	}

	return basic_frame;

outofmem_win:
	membag_free(basic_frame);

outofmem_basic_frame:
	return NULL;
}

/** @} */
