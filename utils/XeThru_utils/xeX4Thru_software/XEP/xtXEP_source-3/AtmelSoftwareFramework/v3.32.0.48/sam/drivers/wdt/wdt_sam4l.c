/**
 * \file
 *
 * \brief Watchdog Timer (WDT) driver for SAM4L.
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

#include "wdt_sam4l.h"
#include "sysclk.h"

/**
 * \ingroup group_sam_drivers_wdt_sam4l
 * @{
 */

/** Watchdog control register first write keys */
#define WDT_FIRST_KEY     0x55ul
/** Watchdog control register second write keys */
#define WDT_SECOND_KEY    0xAAul

/**
 * \brief Sets the WatchDog Timer Control register to the \a ctrl value thanks
 *        to the WatchDog Timer key.
 *
 * \param ctrl  Value to set the WatchDog Timer Control register to.
 */
static void wdt_set_ctrl(uint32_t ctrl)
{
	volatile uint32_t dly;

	/* Calculate delay for internal synchronization, see 44.1.2 WDT errata */
	if ((WDT->WDT_CTRL & WDT_CTRL_CSSEL) == WDT_CLK_SRC_RCSYS) {
		dly = div_ceil(sysclk_get_cpu_hz() * 2, OSC_RCSYS_NOMINAL_HZ);
	} else { /* WDT_CLK_SRC_32K */
		dly = div_ceil(sysclk_get_cpu_hz() * 2, OSC_RC32K_NOMINAL_HZ);
	}
	dly >>= 3; /* ~8 cycles for one while loop */
	while(dly--);
	WDT->WDT_CTRL = ctrl | WDT_CTRL_KEY(WDT_FIRST_KEY);
	WDT->WDT_CTRL = ctrl | WDT_CTRL_KEY(WDT_SECOND_KEY);
}

/**
 * \brief Initializes a Watchdog Timer configuration structure to defaults.
 *
 *  Initializes a given Watchdog Timer configuration structure to a set of
 *  known default values. This function should be called on all new
 *  instances of these configuration structures before being modified by the
 *  user application.
 *
 *  The default configuration is as follows:
 *  - Select the RCSYS oscillator as clock source.
 *  - Not locked, to allow for further (re-)configuration.
 *  - A timeout period of 131072 clocks of the Watchdog module clock (about 1s).
 *  - No window period, so that the watchdog count can be reset at any time.
 *
 *  \param cfg    Configuration structure to initialize to default values.
 */
void wdt_get_config_defaults(struct wdt_config *const cfg)
{
	/* Sanity check arguments */
	Assert(cfg);

	/* Default configuration values */
	cfg->clk_src = WDT_CLK_SRC_RCSYS;
	cfg->wdt_mode = WDT_MODE_BASIC;
	cfg->wdt_int = WDT_INT_DIS;
	cfg->timeout_period = WDT_PERIOD_131072_CLK;
	cfg->window_period = WDT_PERIOD_NONE;
	cfg->disable_flash_cali = true;
	cfg->disable_wdt_after_reset = true;
	cfg->always_on = false;
}

/**
 * \brief Initialize the WDT module.
 *
 * \param dev_inst    Device structure pointer.
 * \param wdt         Base address of the WDT instance.
 * \param cfg         Pointer to WDT configuration.
 *
 * \retval true if the initialization was successful.
 * \retval false if initialization failed.
 */
bool wdt_init(
		struct wdt_dev_inst *const dev_inst,
		Wdt *const wdt,
		struct wdt_config *const cfg)
{
	/* Sanity check arguments */
	Assert(dev_inst);
	Assert(wdt);
	Assert(cfg);

	dev_inst->hw_dev = wdt;
	dev_inst->wdt_cfg = cfg;

	/* Enable APB clock for WDT */
	sysclk_enable_peripheral_clock(wdt);

	/* Check SFV bit: if SFV bit is set, Control Register is locked */
	if (wdt->WDT_CTRL & WDT_CTRL_SFV) {
		return false;
	}

	/* Disable the WDT */
	if (wdt->WDT_CTRL & WDT_CTRL_EN) {
		wdt_set_ctrl(wdt->WDT_CTRL & ~WDT_CTRL_EN);
		while (wdt->WDT_CTRL & WDT_CTRL_EN) {
		}
	}
	/* Disable the WDT clock */
	if (wdt->WDT_CTRL & WDT_CTRL_CEN) {
		wdt_set_ctrl(wdt->WDT_CTRL & ~WDT_CTRL_CEN);
		while (wdt->WDT_CTRL & WDT_CTRL_CEN) {
		}
	}

	/* Initialize the WDT with new configurations */
	wdt_set_ctrl(cfg->clk_src |
			cfg->wdt_mode |
			cfg->wdt_int |
			WDT_CTRL_PSEL(cfg->timeout_period) |
			WDT_CTRL_TBAN(cfg->window_period) |
			(cfg->disable_flash_cali ? WDT_CTRL_FCD : 0) |
			(cfg->disable_wdt_after_reset ? WDT_CTRL_DAR : 0)
	);
	wdt_set_ctrl(wdt->WDT_CTRL | WDT_CTRL_CEN);
	while (!(wdt->WDT_CTRL & WDT_CTRL_CEN)) {
	}

	/* Disable APB clock for WDT */
	sysclk_disable_peripheral_clock(wdt);

	return true;
}

/**
 * \brief Enable the WDT module.
 *
 * \param dev_inst    Device structure pointer.
 */
void wdt_enable(struct wdt_dev_inst *const dev_inst)
{
	Wdt *wdt = dev_inst->hw_dev;
	struct wdt_config *cfg = dev_inst->wdt_cfg;

	/* Enable APB clock for WDT */
	sysclk_enable_peripheral_clock(wdt);

	/* Enable the WDT, and lock WDT CTRL register if need */
	wdt_set_ctrl(wdt->WDT_CTRL | WDT_CTRL_EN |
		(cfg->always_on ? WDT_CTRL_SFV : 0));
	while ((WDT->WDT_CTRL & WDT_CTRL_EN) != WDT_CTRL_EN) {
	}
}

/**
 * \brief Disable the WDT module.
 *
 * \param dev_inst    Device structure pointer.
 */
void wdt_disable(struct wdt_dev_inst *const dev_inst)
{
	Wdt *wdt = dev_inst->hw_dev;

	/* Disable the WDT */
	wdt_set_ctrl(wdt->WDT_CTRL & ~WDT_CTRL_EN);
	while (wdt->WDT_CTRL & WDT_CTRL_EN) {
	}

	/* Disable APB clock for WDT */
	sysclk_disable_peripheral_clock(wdt);
}

/**
 * \brief Restart the watchdog timer.
 *
 * \param dev_inst    Device structure pointer.
 */
void wdt_clear(struct wdt_dev_inst *const dev_inst)
{
	Wdt *wdt = dev_inst->hw_dev;

	while ((wdt->WDT_SR & WDT_SR_CLEARED) != WDT_SR_CLEARED);
	wdt->WDT_CLR = WDT_CLR_KEY(WDT_FIRST_KEY) | WDT_CLR_WDTCLR;
	wdt->WDT_CLR = WDT_CLR_KEY(WDT_SECOND_KEY) | WDT_CLR_WDTCLR;
}

/**
 * \brief Reset MCU by generating a WDT reset as soon as possible.
 *
 * \note This function will not work if the SFV bit was previously set
 * (The WDT configuration has previously been locked).
 *
 * \return \b false Cannot reset MCU with a WDT reset, or the MCU will be reset.
 */
bool wdt_reset_mcu(void)
{
	struct wdt_dev_inst wdt_inst;
	struct wdt_config   wdt_cfg;

	/*
	 * Enable the watchdog with minimum period
	 * (fastest way to get a watchdog reset).
	 */
	wdt_get_config_defaults(&wdt_cfg);
	wdt_cfg.timeout_period = WDT_PERIOD_MIN_CLK;
	wdt_cfg.disable_wdt_after_reset = true;
	if (!wdt_init(&wdt_inst, WDT, &wdt_cfg)) {
		return false;
	}
	Disable_global_interrupt();
	wdt_enable(&wdt_inst);

	while (1) {
		/* Waiting for watchdog reset */
	}
}



/**
 * @}
 */
