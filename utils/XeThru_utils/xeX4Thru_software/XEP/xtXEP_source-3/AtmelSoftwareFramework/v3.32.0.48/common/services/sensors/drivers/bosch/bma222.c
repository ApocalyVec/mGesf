/**
 * \file
 *
 * \brief Bosch BMA222 3-axis accelerometer driver.
 *
 * This file contains functions for initializing and reading data
 * from a Bosch BMA222 3-axis accelerometer.
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
#include "bma222.h"
#include "bma_axis.inc.c"

static bool bma222_read(sensor_t *, sensor_read_t, sensor_data_t *);
static bool bma222_ioctl(sensor_t *, sensor_command_t, void *);
static bool bma222_selftest(sensor_t *, int *, void *);
static bool bma222_event(sensor_t *, sensor_event_t, sensor_event_callback_t *,
		bool);
static void bma222_isr(volatile void *);
static bool bma222_set_state(sensor_t *, sensor_state_t);

/** \brief Bosch BMA222 Range Table (milli-g, register value) */
static const sensor_map_t range_table [] = {
	{{ 2000}, BMA222_RANGE_2G},
	{{ 4000}, BMA222_RANGE_4G},
	{{ 8000}, BMA222_RANGE_8G},
	{{16000}, BMA222_RANGE_16G}
};

/** \brief Bosch BMA222 Bandwidth Table (hertz, register value) */
static const sensor_map_t band_table [] = {
	{{   8}, BMA222_BANDWIDTH_8Hz  }, /*    7.81 Hz */
	{{  16}, BMA222_BANDWIDTH_16Hz }, /*   15.63 Hz */
	{{  31}, BMA222_BANDWIDTH_31Hz }, /*   31.25 Hz */
	{{  63}, BMA222_BANDWIDTH_63Hz }, /*   62.50 Hz */
	{{ 125}, BMA222_BANDWIDTH_125Hz}, /*  125.00 Hz */
	{{ 250}, BMA222_BANDWIDTH_250Hz}, /*  250.00 Hz */
	{{ 500}, BMA222_BANDWIDTH_500Hz}, /*  500.00 Hz */
	{{1000}, BMA222_BANDWIDTH_1000Hz} /* 1000.00 Hz */
};

/** Sensor Event Callback Descriptors (data=0, motion=1, low-g=2, high-g=3,
 * tap=4) */
static sensor_event_callback_t event_cb[5] = {
	{.handler = default_event_handler},
	{.handler = default_event_handler},
	{.handler = default_event_handler},
	{.handler = default_event_handler},
	{.handler = default_event_handler}
};

/** \brief Sensor Event Registers */
static struct {
	bma_axis_t acc[3];                 /**< Acceleration data */
	int8_t temp;                       /**< Temperature data */

	union {
		uint8_t status_byte[4];        /**< Status bytes */
		struct {                       /**< Status fields */
			uint8_t flat_int      : 1; /**< Flat interrupt triggered */
			uint8_t orient_int    : 1; /**< Orientation interrupt triggered */
			uint8_t s_tap_int     : 1; /**< Single-tap interrupt triggered */
			uint8_t d_tap_int     : 1; /**< double-tap interrupt triggered */
			uint8_t reserved_09   : 1;
			uint8_t slope_int     : 1; /**< Slope criteria triggered */
			uint8_t high_int      : 1; /**< High-g criteria triggered */
			uint8_t low_int       : 1; /**< Low-g criteria triggered */

			uint8_t data_int      : 1; /**< New data interrupt triggered */
			uint8_t reserved_0a   : 7;

			uint8_t tap_sign      : 1; /**< Tap axis motion direction */
			uint8_t tap_first_z   : 1; /**< z-axis tap interrupt */
			uint8_t tap_first_y   : 1; /**< y-axis tap interrupt */
			uint8_t tap_first_x   : 1; /**< x-axis tap interrupt */
			uint8_t slope_sign    : 1; /**< Axis motion direction */
			uint8_t slope_first_z : 1; /**< z-axis any-motion interrupt */
			uint8_t slope_first_y : 1; /**< y-axis any-motion interrupt */
			uint8_t slope_first_x : 1; /**< x-axis any-motion interrupt */

			uint8_t flat          : 1; /**< Orientation with respect
			                            * to gravity */
			uint8_t orient        : 3; /**< orientation with respect
			                            * to gravity */
			uint8_t high_sign     : 1; /**< High-g interrupt sign */
			uint8_t high_first_z  : 1; /**< z-axis high-g interrupt */
			uint8_t high_first_y  : 1; /**< y-axis high-g interrupt */
			uint8_t high_first_x  : 1; /**< x-axis high-g interrupt */
		} status_field;
	};
} event_regs;

/**
 * @brief Bosch BMA222 accelerometer driver initialization.
 *
 * This is the main initialization function for the BMA222 device.
 *
 * @param sensor    Address of a sensor device descriptor.
 * @param resvd     Reserved value.
 * @return bool     true if the call succeeds, else false is returned.
 */
bool bma222_init(sensor_t *sensor, int resvd)
{
	bool status = false;
	sensor_hal_t *const hal = sensor->hal;

	if (BMA222_ID_VAL == sensor_bus_get(hal, BMA222_CHIP_ID)) {
		/* Set the driver function table and capabilities pointer. */
		static const sensor_device_t bma222_device = {
			.func.read        = bma222_read,
			.func.ioctl       = bma222_ioctl,
			.func.selftest    = bma222_selftest,
			.func.event       = bma222_event,

			.caps.feature     = SENSOR_CAPS_3_AXIS     |
					SENSOR_CAPS_SELFTEST   |
					SENSOR_CAPS_HI_G_EVENT |
					SENSOR_CAPS_LO_G_EVENT |
					SENSOR_CAPS_TAP_EVENT  |
					SENSOR_CAPS_TILT_EVENT |
					SENSOR_CAPS_AUX_TEMP,

			.caps.vendor      = SENSOR_VENDOR_BOSCH,
			.caps.range_table = range_table,
			.caps.band_table  = band_table,
			.caps.range_count = ARRAYSIZE(range_table),
			.caps.band_count  = ARRAYSIZE(band_table),
			.caps.units       = SENSOR_UNITS_g0,
			.caps.scale       = SENSOR_SCALE_milli,
			.caps.name = "BMA222 Digital, triaxial acceleration sensor"
		};

		sensor->drv = &bma222_device;

		/* Set the driver (device) default range, bandwidth, &
		 * resolution.
		 *
		 * \internal
		 * Per the BMA222 Datasheet the default range and bandwidth
		 * after device reset are +/- 2g and 1kHz respectively.
		 */
		bma222_set_state(sensor, SENSOR_STATE_RESET);

		hal->range      = 2000;
		hal->bandwidth  = 1000;
		hal->resolution = BMA222_DATA_RESOLUTION;
		hal->burst_addr = BMA222_NEW_DATA_X;

		/* Install an interrupt handler. */
		if ((STATUS_OK == hal->bus.status) &&
				sensor_irq_connect(hal->mcu_sigint, bma222_isr,
				hal)) {
			status = true;
		}
	}

	return status;
}

/**
 * @brief Bosch BMA222 driver interrupt service routine.
 *
 * This is the common interrupt service routine for all enabled BMA222 interrupt
 * events.  Five different types of interrupts can be programmed.  All interrupt
 * criteria are combined and drive the interrupt pad with a Boolean \c OR
 * condition.
 *
 * Interrupt criteria are tested against values from the BMA222 digital filter
 * output.  All thresholds are scaled using the current device range.  Timings
 * for high and low acceleration are absolute values (1 LSB of HG_dur and LG_dur
 * registers corresponds to 1 millisecond, +/- 10%).  Timing for the any-motion
 * interrupt and alert detection are proportional to the bandwidth setting.
 *
 * This routine handles interrupts generated when low-g, high-g, any-motion,
 * alert, and new data criteria are satisfied and the corresponding event
 * notification is enabled in the device.
 *
 * The BMA222 device does not provide any way to definitively identify an
 * any-motion interrupt once it has occurred.  So, if a handler has been
 * installed for that event, it will always be called by this routine,
 * and the SENSOR_EVENT_MOTION indicator will be set in the event type field.
 *
 * @param arg       The address of the driver sensor_hal_t descriptor.
 * @return Nothing.
 */
static void bma222_isr(volatile void *arg)
{
	sensor_hal_t *const hal = (sensor_hal_t *)arg;

	hal->bus.no_wait = true;
	sensor_bus_read(hal, hal->burst_addr, &event_regs, sizeof(event_regs));
	hal->bus.no_wait = false;

	if (STATUS_OK == hal->bus.status) {
		sensor_event_data_t event_data = {.data.scaled = true};

		event_data.data.timestamp = sensor_timestamp();
		event_data.event = SENSOR_EVENT_UNKNOWN;

		format_axis_data(hal, event_regs.acc, &(event_data.data));

		if (event_regs.status_field.data_int) {
			event_data.event |= SENSOR_EVENT_NEW_DATA;
			(event_cb[0].handler)(&event_data, event_cb[0].arg);
		}

		if (event_regs.status_field.slope_int) {
			event_data.event |= SENSOR_EVENT_MOTION;
			(event_cb[1].handler)(&event_data, event_cb[1].arg);
		}

		if (event_regs.status_field.low_int) {
			event_data.event |= SENSOR_EVENT_LOW_G;
			(event_cb[2].handler)(&event_data, event_cb[2].arg);
		}

		if (event_regs.status_field.high_int) {
			event_data.event |= SENSOR_EVENT_HIGH_G;
			(event_cb[3].handler)(&event_data, event_cb[3].arg);
		}

		if (event_regs.status_field.s_tap_int) {
			event_data.event |= SENSOR_EVENT_S_TAP;
			(event_cb[4].handler)(&event_data, event_cb[4].arg);
		}

		if (event_regs.status_field.d_tap_int) {
			event_data.event |= SENSOR_EVENT_D_TAP;
			(event_cb[4].handler)(&event_data, event_cb[4].arg);
		}
	}
}

/**
 * @brief Read BMA222 device ID and revision numbers.
 *
 * This function reads the accelerometer hardware identification registers
 * and returns these values in the specified data structure.
 *
 * @param hal       Address of an initialized sensor hardware descriptor.
 * @param data      Address of sensor_data_t structure to return values.
 * @return bool     true if the call succeeds, else false is returned.
 */
static bool bma222_device_id(sensor_hal_t *hal, sensor_data_t *data)
{
	data->device.id = sensor_bus_get(hal, BMA222_CHIP_ID);
	data->device.version = 0;

	return (STATUS_OK == hal->bus.status);
}

/**
 * @brief Read BMA222 acceleration data.
 *
 * This function obtains accelerometer data for all three axes of the Bosch
 * device.  The data is read from three device registers using a multi-byte
 * bus transfer.
 *
 * Along with the actual sensor data, the LSB byte contains a "new" flag
 * indicating if the data for this axis has been updated since the last
 * time the axis data was read.  Reading either LSB or MSB data will
 * clear this flag.
 *
 * @param sensor    Address of an initialized sensor device descriptor.
 * @param data      The address of a vector storing sensor axis data.
 * @return bool     true if the call succeeds, else false is returned.
 */
static bool bma222_get_accel(sensor_hal_t *hal, sensor_data_t *data)
{
	size_t const count = sensor_bus_read(hal, hal->burst_addr,
			event_regs.acc, sizeof(event_regs.acc));

	format_axis_data(hal, event_regs.acc, data);

	return (count == sizeof(event_regs.acc));
}

/**
 * @brief Read BMA222 temperature data.
 *
 * This function reads temperature data, where center temperature 24 C
 * corresponds to a value 0x00 read from the temperature register with
 * temperature slope 0.5 C/LSB.
 *
 * @param hal       Address of an initialized sensor hardware descriptor.
 * @param data      The address where temperature samples are returned.
 * @return bool     true if the call succeeds, else false is returned.
 */
static bool bma222_get_temperature(sensor_hal_t *hal, sensor_data_t *data)
{
	int8_t const temp_data = (int8_t)sensor_bus_get(hal, BMA222_TEMP);

	if (data->scaled) {
		data->temperature.value = BMA222_TEMP_OFFSET + (temp_data / 2);
	} else {
		data->temperature.value = temp_data;
	}

	return (STATUS_OK == hal->bus.status);
}

/**
 * @brief Read sensor data
 *
 * This routine calls the appropriate internal data function to obtain
 * the specified type of data from the sensor device.
 *
 * @param sensor    Address of an initialized sensor device descriptor.
 * @param type      Type of sensor data to read.
 * @param data      The address where data values are returned.
 * @return bool     true if the call succeeds, else false is returned.
 */
static bool bma222_read(sensor_t *sensor, sensor_read_t type,
		sensor_data_t *data)
{
	sensor_hal_t *const hal = sensor->hal;

	switch (type) {
	case SENSOR_READ_ACCELERATION:
		return bma222_get_accel(hal, data);

	case SENSOR_READ_TEMPERATURE:
		return bma222_get_temperature(hal, data);

	case SENSOR_READ_ID:
		return bma222_device_id(hal, data);

	default:
		sensor->err = SENSOR_ERR_FUNCTION;
		return false;
	}
}

/**
 * @brief Enable or disable the BMA222 sleep mode.
 *
 * This routine enables or disables the BMA222 sleep mode depending upon
 * the value of the \sleep parameter; a \true value enables sleep mode and
 * a \false value disables sleep mode by setting or clearing the 'sleep_en'
 * bit, respectively.
 *
 * @param hal       Address of an initialized sensor hardware descriptor.
 * @param sleep     Set flag \true to enable sleep mode.
 * @return Nothing
 */
static inline void bma222_sleep_en(sensor_hal_t *hal, bool sleep)
{
	uint8_t const power_mode_val = (sleep == true)
			? (BMA222_LOWPOWER_EN | BMA222_SLEEP_DUR_1ms) : 0;

	sensor_bus_put(hal, BMA222_POWER_MODES, power_mode_val);
}

/**
 * @brief Set the BMA222 execution mode.
 *
 * This routine sets a specified BMA222 execution state to one of the
 * following:
 *
 * SENSOR_STATE_SUSPEND or SENSOR_STATE_LOWEST_POWER
 *      The BMA222 can be put into a suspend mode to easily achieve a power
 *      consumption below 1uA.  In this mode all analog modules except for
 *      power-on reset will be disabled.  Only reads through the serial
 *      interface are supported during suspend.
 *
 *  SENSOR_STATE_SLEEP or SENSOR_STATE_LOW_POWER
 *      This option sets the BMA222 to a low-power mode.  In this mode, the
 *      device periodically wakes up, evaluates acceleration data with respect
 *      to interrupt criteria defined by the user and goes back to sleep if no
 *      interrupt has occurred using the following procedure:
 *
 *      1. Wake-up
 *
 *      2. Enable analog front-end and convert acceleration data until the
 *         low-pass filters have settled.
 *
 *      3. Enable interrupt controller and evaluate interrupt conditions.
 *         Once interrupt conditions are evaluated and no interrupt has
 *         occurred, the chip goes back to sleep.  If no interrupts are enabled,
 *         acceleration for all three axes are converted once before the chip
 *         goes back to sleep.
 *
 *      4. Sleep for the programmed duration.  Available sleep durations are
 *         2ms, 10ms, 25ms, 50ms, 100ms, 500ms, 1s, and 2s.
 *
 *  SENSOR_STATE_NORMAL or SENSOR_STATE_HIGHEST_POWER
 *      In normal mode the sensor IC data and status registers can be accessed
 *      without restriction.  The device current consumption is typically
 *      250 microamps in this state.
 *
 *  SENSOR_STATE_X_AXIS_STANDBY
 *  SENSOR_STATE_Y_AXIS_STANDBY
 *  SENSOR_STATE_Z_AXIS_STANDBY
 *      In order to optimize further power consumption of the BMA222, data
 *      evaluation of individual axes can be deactivated.
 *
 *  SENSOR_STATE_RESET
 *      This function resets the device and internal registers to the power-up
 *      default settings.
 *
 * In the wake-up mode, the BMA222 automatically switches from sleep mode to
 * normal mode after a delay defined by a programmable \c wake_up_pause value.
 * After transitioning from sleep to normal mode, acceleration data acquisition
 * and interrupt verification are performed.  The sensor automatically returns
 * to sleep mode again if no interrupt criteria are satisfied.
 *
 * @param sensor    Address of a sensor device descriptor.
 * @param mode      A specified sensor operational mode.
 * @return bool     true if the call succeeds, else false is returned.
 */
static bool bma222_set_state(sensor_t *sensor, sensor_state_t mode)
{
	sensor_hal_t *const hal = sensor->hal;

	/* Perform a state transition out of the previous mode
	 * into the new mode.
	 */

	switch (sensor->mod) {
	case SENSOR_STATE_NORMAL :
	case SENSOR_STATE_HIGHEST_POWER:

		if ((SENSOR_STATE_SLEEP == mode) ||
				(SENSOR_STATE_LOW_POWER == mode)) {
			/* Enter sleep from normal mode. */
			bma222_sleep_en(hal, true);
		} else if ((SENSOR_STATE_SUSPEND == mode) ||
				(SENSOR_STATE_LOWEST_POWER == mode)) {
			/* Enter suspend mode from normal mode. */
			sensor_bus_put(hal, BMA222_POWER_MODES, BMA222_SUSPEND);
		}

		break;

	case SENSOR_STATE_SLEEP:
	case SENSOR_STATE_LOW_POWER:

		bma222_sleep_en(hal, false);

		if ((SENSOR_STATE_SUSPEND == mode) ||
				(SENSOR_STATE_LOWEST_POWER == mode)) {
			/* Enter suspend mode from sleep mode. */
			sensor_bus_put(hal, BMA222_POWER_MODES, BMA222_SUSPEND);
		}

		break;

	case SENSOR_STATE_SUSPEND:
	case SENSOR_STATE_LOWEST_POWER:

		if ((SENSOR_STATE_SLEEP == mode) ||
				(SENSOR_STATE_LOW_POWER == mode)) {
			/* Enter sleep from suspend mode. */
			bma222_sleep_en(hal, true);
		} else if ((SENSOR_STATE_NORMAL == mode) ||
				(SENSOR_STATE_HIGHEST_POWER == mode)) {
			/* Enter normal mode from suspend mode. */
			sensor_bus_put(hal, BMA222_POWER_MODES, 0);
		}

		break;

	case SENSOR_STATE_RESET:

		/*
		 * \todo
		 * Update sensor device descriptor operational settings.
		 */
		sensor_bus_put(hal, BMA222_SOFTRESET, BMA222_RESET);
		sensor_bus_put(hal, BMA222_SOFTRESET, BMA222_RESET);
		break;

	default:
		return false;
	}

	return true;
}

/**
 * @brief Set the BMA222 full scale acceleration range.
 *
 * @param hal       Address of an initialized sensor hardware descriptor.
 * @param range     The index of a driver-specific range table entry.
 * @return bool     true if the call succeeds, else false is returned.
 */
static bool bma222_set_range(sensor_hal_t *hal, int16_t range)
{
	sensor_bus_put(hal, BMA222_G_RANGE, range_table[range].reserved_val);
	return (STATUS_OK == hal->bus.status);
}

/**
 * @brief Set the BMA222 digital filter cut-off frequency
 *
 * @param hal      Address of an initialized sensor hardware descriptor.
 * @param band     The index of a driver-specific bandwidth table entry.
 * @return bool     true if the call succeeds, else false is returned.
 */
static bool bma222_set_bandwidth(sensor_hal_t *hal, int16_t band)
{
	sensor_bus_put(hal, BMA222_BANDWIDTH, band_table[band].reserved_val);
	return (STATUS_OK == hal->bus.status);
}

/**
 * @brief Set event threshold value
 *
 * @param hal       Address of an initialized sensor hardware descriptor.
 * @param threshold Address of threshold descriptor.
 * @return bool     true if the call succeeds, else false is returned.
 */
static bool bma222_set_threshold
	(sensor_hal_t *hal, sensor_threshold_desc_t *threshold)
{
	/* Threshold values will be passed in milli-g units (assumed). */
	int32_t value = scaled_to_raw(hal, threshold->value);

	switch (threshold->type) {
	default:
		return false;

	case SENSOR_THRESHOLD_MOTION:

		/*
		 * Any-Motion (slope) Threshold
		 *
		 * The slope interrupt threshold value LSB corresponds to an LSB
		 * of acceleration data for the selected g-range. The default
		 * value of 14h for the default 2mg range implies a threshold of
		 * 312.5mg.
		 */
		sensor_bus_put(hal, BMA222_SLOPE_THRESHOLD, (uint8_t)value);
		break;

	case SENSOR_THRESHOLD_TAP:

		/*
		 * Single-Tap or Double-Tap Threshold
		 *
		 * An LSB of tap threshold depends upon the selected g-range
		 * where an acceleration delta of 62.5mg in 2-g, 125mg in 4-g,
		 * etc. will apply. The default 0ah raw value corresponds to the
		 * default 2mg range.
		 */
	{
		int8_t const mask = BMA222_TAP_TH_FIELD;
		sensor_reg_fieldset(hal, BMA222_TAP_CONFIG, mask,
				(uint8_t)value);
	}
	break;

	case SENSOR_THRESHOLD_LOW_G:

		/*
		 * Low-G Threshold
		 *
		 * An LSB of low-g threshold always corresponds to an
		 * acceleration of 7.81mg; namely, the increment is independent
		 * of the g-range. Divide the requested threshold in milli-g
		 * by 7.81mg (781/100) to calculate the register value.
		 */
		value = (threshold->value * 100) / 781;
		sensor_bus_put(hal, BMA222_LOW_G_THRESHOLD, (uint8_t)value);
		break;

	case SENSOR_THRESHOLD_HIGH_G:

		/*
		 * High-G Threshold
		 *
		 * An LSB of high-g threshold depends upon the selected g-range
		 * where an acceleration delta of 62.5mg in 2-g, 125mg in 4-g,
		 * etc. will apply. The default 0ah raw value corresponds to the
		 * default 2mg range.
		 */
		sensor_bus_put(hal, BMA222_HIGH_G_THRESHOLD, (uint8_t)value);
		break;
	}

	return (STATUS_OK == hal->bus.status);
}

/**
 * @brief Get event threshold value
 *
 * @param hal       Address of an initialized sensor hardware descriptor.
 * @param threshold Address of threshold descriptor.
 * @return bool     true if the call succeeds, else false is returned.
 */
static bool bma222_get_threshold
	(sensor_hal_t *hal, sensor_threshold_desc_t *threshold)
{
	switch (threshold->type) {
	default:
		return false;

	case SENSOR_THRESHOLD_MOTION:
		threshold->value = raw_to_scaled(hal,
				sensor_bus_get(hal, BMA222_SLOPE_THRESHOLD));
		break;

	case SENSOR_THRESHOLD_TAP:
	{
		uint8_t const mask = BMA222_TAP_TH_FIELD;
		threshold->value = raw_to_scaled(hal,
				sensor_reg_fieldget(hal, BMA222_TAP_CONFIG,
				mask));
	}
	break;

	case SENSOR_THRESHOLD_LOW_G:
		threshold->value = (781 / 100) * sensor_bus_get(hal,
				BMA222_LOW_G_THRESHOLD);
		break;

	case SENSOR_THRESHOLD_HIGH_G:
		threshold->value = raw_to_scaled(hal,
				sensor_bus_get(hal, BMA222_HIGH_G_THRESHOLD));
		break;
	}

	return (STATUS_OK == hal->bus.status);
}

/**
 * @brief Run BMA222 self-tests.
 *
 * @param sensor    Address of an initialized sensor device descriptor.
 * @param test_code Address of a device-specific numeric test code.
 * @param arg       Device-specific self-test argument options.
 * @return bool     true if the call succeeds, else false is returned.
 */
static bool bma222_selftest(sensor_t *sensor, int *test_code, void *arg)
{
	bool result = false;

	if (SENSOR_TEST_DEFLECTION == *test_code) {
		/* \todo Execute BMA222 electrostatic deflection self-test. */
	}

	return result;
}

/**
 * @brief Enable or disable BMA222 sensor events.
 *
 * @param  sensor       Address of an initialized sensor device descriptor
 * @param  sensor_event Specifies the sensor event type
 * @param  callback     Application-defined event callback handler descriptor
 * @param  enable       Enable flag: true = enable event, false = disable event
 * @return bool         true if the call succeeds, else false is returned
 */
static bool bma222_event(sensor_t *sensor, sensor_event_t sensor_event,
		sensor_event_callback_t *callback, bool enable)
{
	sensor_hal_t *const hal = sensor->hal;

	bool status = false;

	uint8_t int_enable1 = sensor_bus_get(hal, BMA222_16_INTR_EN);
	uint8_t int_enable2 = sensor_bus_get(hal, BMA222_17_INTR_EN);

	if (sensor_event & SENSOR_EVENT_NEW_DATA) {
		if (callback) {
			event_cb[0] = *callback;
		}

		if (enable) {
			int_enable2 |=  BMA222_DATA_EN;
		} else {
			int_enable2 &= ~BMA222_DATA_EN;
		}

		status = true;
	}

	if (sensor_event & SENSOR_EVENT_MOTION) {
		if (callback) {
			event_cb[1] = *callback;
		}

		if (enable) {
			/* Enable slope detection on x, y, and z axes using the
			 * default settings for the slope threshold & duration.
			 */
			int_enable1 |=  (BMA222_SLOPE_EN_Z | BMA222_SLOPE_EN_Y |
					BMA222_SLOPE_EN_X);
		} else {
			int_enable1 &= ~(BMA222_SLOPE_EN_Z | BMA222_SLOPE_EN_Y |
					BMA222_SLOPE_EN_X);
		}

		status = true;
	}

	if (sensor_event & SENSOR_EVENT_LOW_G) {
		if (callback) {
			event_cb[2] = *callback;
		}

		if (enable) {
			int_enable2 |=  BMA222_LOW_EN;
		} else {
			int_enable2 &= ~BMA222_LOW_EN;
		}

		status = true;
	}

	if (sensor_event & SENSOR_EVENT_HIGH_G) {
		if (callback) {
			event_cb[3] = *callback;
		}

		if (enable) {
			/* Enable high-g detection on x, y, and z axes using the
			 * default settings for the high-g duration &
			 * hysteresis.
			 */
			int_enable2 |=  (BMA222_HIGH_EN_Z | BMA222_HIGH_EN_Y |
					BMA222_HIGH_EN_X);
		} else {
			int_enable2 &= ~(BMA222_HIGH_EN_Z | BMA222_HIGH_EN_Y |
					BMA222_HIGH_EN_X);
		}

		status = true;
	}

	if (sensor_event & SENSOR_EVENT_TAP) {
		if (callback) {
			event_cb[4] = *callback;
		}

		if (enable) {
			int_enable1 |=  (BMA222_S_TAP_EN | BMA222_D_TAP_EN);
		} else {
			int_enable1 &= ~(BMA222_S_TAP_EN | BMA222_D_TAP_EN);
		}

		status = true;
	}

	sensor_bus_put(hal, BMA222_16_INTR_EN, int_enable1);
	sensor_bus_put(hal, BMA222_17_INTR_EN, int_enable2);

	return status;
}

/**
 * @brief BMA222 tap detection configuration
 *
 * @param hal       Address of an initialized sensor hardware descriptor.
 * @param params    Address of an initialized tap parameter structure.
 * @return bool     true if the call succeeds, else false is returned.
 */
static bool bma222_set_tap(sensor_hal_t *hal, sensor_tap_params_t *params)
{
	/* \todo Implement the device tap functions. */

	return false;
}

/**
 * @brief BMA222 ioctl control entry point
 *
 * @param hal           Address of an initialized sensor hardware descriptor.
 * @param cmd           Command to execute
 * @param arg           Argument for command (varies)
 * @return bool     true if the call succeeds, else false is returned.
 */
static bool bma222_ioctl(sensor_t *sensor, sensor_command_t cmd, void *arg)
{
	sensor_hal_t *const hal = sensor->hal;
	sensor_data_t sample = {.scaled = true};

	switch (cmd) {
	default:
		sensor->err = SENSOR_ERR_UNSUPPORTED;
		return false;

	case SENSOR_SET_STATE:
	{
		sensor_state_t const mode = *((sensor_state_t *)arg);

		if ((mode != sensor->mod) && bma222_set_state(sensor, mode)) {
			sensor->mod = (mode == SENSOR_STATE_RESET)
					? SENSOR_STATE_NORMAL : mode;
			return true;
		} else {
			return false;
		}
	}

	case SENSOR_SET_RANGE:
		return bma222_set_range(hal, (uint16_t)*((int *)arg));

	case SENSOR_SET_BANDWIDTH:
		return bma222_set_bandwidth(hal, (uint16_t)*((int *)arg));

	case SENSOR_ENABLE_EVENT:
		return bma222_event(sensor, *((sensor_event_t *)arg), 0, true);

	case SENSOR_DISABLE_EVENT:
		return bma222_event(sensor, *((sensor_event_t *)arg), 0, false);

	case SENSOR_SET_THRESHOLD:
		return bma222_set_threshold(hal,
				(sensor_threshold_desc_t *)arg);

	case SENSOR_GET_THRESHOLD:
		return bma222_get_threshold(hal,
				(sensor_threshold_desc_t *)arg);

	case SENSOR_SET_TAP:
		return bma222_set_tap(hal, (sensor_tap_params_t *)arg);

	case SENSOR_READ_VECTOR:
		if (bma222_get_accel(hal, &sample)) {
			vector3_t *const pvec = (vector3_t *)arg;
			pvec->x = sample.axis.x;
			pvec->y = sample.axis.y;
			pvec->z = sample.axis.z;
			return true;
		} else {
			return false;
		}
	}
}
