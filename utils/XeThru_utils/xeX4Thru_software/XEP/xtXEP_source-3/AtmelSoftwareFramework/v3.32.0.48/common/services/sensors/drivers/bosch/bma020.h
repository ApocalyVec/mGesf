/**
 * \file
 *
 * \brief Bosch BMA020 3-axis accelerometer.
 *
 * This file contains functions for initializing and reading data
 * from a Bosch BMA020 3-axis accelerometer.
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
#ifndef _BMA020_H_
#define _BMA020_H_

#include <asf.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TWI/I2C address (write @ 0x70 on bus, read @ 0x71 on bus) */
#define BMA020_TWI_ADDR             (0x38)
#define BMA020_SPI_MODE             (3)

/* Signed 16-bit axis data sample resolution and alignment */
#define BMA020_DATA_SHIFT_RIGHT     (6)
#define BMA020_DATA_RESOLUTION      (10)

/** \brief BMA020 Register Addresses */
/** @{ */
#define BMA020_CHIP_ID              (0x00)  /* chip ID - always 0x02 */
#define BMA020_CHIP_VERSION         (0x01)  /* chip revision */
#define BMA020_ACC_X_LSB            (0x02)  /* X accel - LSB + new data */
#define BMA020_ACC_X_MSB            (0x03)  /* X accel - MSB */
#define BMA020_ACC_Y_LSB            (0x04)  /* Y accel - LSB + new data */
#define BMA020_ACC_Y_MSB            (0x05)  /* Y accel - MSB */
#define BMA020_ACC_Z_LSB            (0x06)  /* Z accel - LSB + new data */
#define BMA020_ACC_Z_MSB            (0x07)  /* Z accel - MSB */
#define BMA020_STATUS1              (0x09)  /* device status register 1 */
#define BMA020_CTRL1                (0x0a)  /* device control 1 */
#define BMA020_CTRL2                (0x0b)  /* device control 2 */
#define BMA020_LG_THRES             (0x0c)  /* low g threshold */
#define BMA020_LG_DUR               (0x0d)  /* low g duration */
#define BMA020_HG_THRES             (0x0e)  /* high g threshold */
#define BMA020_HG_DUR               (0x0f)  /* high g duration */
#define BMA020_ANY_MOTION_THRES     (0x10)  /* any motion threshold */
#define BMA020_CTRL3                (0x11)  /* motion duration & g-hysteresis */
#define BMA020_CUST1                (0x12)  /* customer reserved reg 1 */
#define BMA020_CUST2                (0x13)  /* customer reserved reg 2 */
#define BMA020_CTRL4                (0x14)  /* range & bandwidth */
#define BMA020_CTRL5                (0x15)  /* interrupt & wake control */
/** @} */

/** \brief BMA020 Register Bit Definitions */
/** @{ */

/* BMA020_CHIP_ID (0x00) */

#define BMA020_ID_VAL               (0x02)  /* BMA020 chip id value */

/* Bosch BMA020 / BMA150 Common Register Definitions */

/* BMA_ACC_X_LSB (0x02), BMA_ACC_Y_LSB (0x04), BMA_ACC_Z_LSB (0x06) */

#define ACC_NEW_DATA                (0x01)  /* new data available */
#define ACC_LSB_0_1                 (0xc0)  /* bits 0 and 1 of accel reading */

/* BMA_STATUS1 (0x09) */

#define STATUS1_HG                  (0x01)  /* high-g currently detected */
#define STATUS1_LG                  (0x02)  /* low-g currently detected */
#define STATUS1_HG_LATCHED          (0x04)  /* high-g event detected */
#define STATUS1_LG_LATCHED          (0x08)  /* low-g event detected */

/* BMA_CTRL1 (0x0a) */

#define CTRL1_SLEEP                 (0x01)   /* enter sleep mode */
#define CTRL1_SOFT_RESET            (0x02)   /* reset device */
#define CTRL1_SELF_TEST_0           (0x04)   /* perform self test 0 */
#define CTRL1_SELF_TEST_1           (0x08)   /* perform self test 1 */
#define CTRL1_RESET_INT             (0x40)   /* reset interrupt */

/* BMA_CTRL2 (0x0b) */

#define CTRL2_ENABLE_LG             (0x01)   /* enable low g interrupt */
#define CTRL2_ENABLE_HG             (0x02)   /* enable high g interrupt */
#define CTRL2_COUNTER_LG            (0x0c)   /* low g int count (2 bits) */
#define CTRL2_COUNTER_HG            (0x30)   /* high g int count (2 bits) */
#define CTRL2_ANY_MOTION            (0x40)   /* enable any-motion interrupt */
#define CTRL2_ALERT                 (0x80)   /* enable any-motion alert mode */

/* BMA_CTRL3 (0x11) */

#define CTRL3_LG_HYST               (0x07)   /* low g hysteresis (3 bits) */
#define CTRL3_HG_HYST               (0x28)   /* high g hysteresis (3 bits) */
#define CTRL3_ANY_MOTION_DUR        (0xc0)   /* any motion duration (2 bits) */

/* BMA_CTRL4 (0x14) */

#define CTRL4_BANDWIDTH             (0x07)  /* bandwidth (3 bits) */
#define BANDWIDTH_25HZ              (0x00)  /*   25 Hz */
#define BANDWIDTH_50HZ              (0x01)  /*   50 Hz */
#define BANDWIDTH_100HZ             (0x02)  /*   100 Hz */
#define BANDWIDTH_190HZ             (0x03)  /*   190 Hz */
#define BANDWIDTH_375HZ             (0x04)  /*   375 Hz */
#define BANDWIDTH_750HZ             (0x05)  /*   750 Hz */
#define BANDWIDTH_1500HZ            (0x06)  /*   1500 Hz */
#define CTRL4_RANGE                 (0x18)  /* range (2 bits) */
#define RANGE_2G                    (0x00)  /*   +/- 2g */
#define RANGE_4G                    (0x08)  /*   +/- 4g */
#define RANGE_8G                    (0x10)  /*   +/- 8g */

/* BMA_CTRL5 (0x15) */

#define CTRL5_WAKE_UP              (0x01)   /* enable automatic wake up */
#define CTRL5_WAKE_UP_PAUSE        (0x06)   /* duration between wakes, 2bits */
#define CTRL5_SHADOW_DIS           (0x08)   /* disable lsb/msb read blocking */
#define CTRL5_LATCH_INT            (0x10)   /* latch interrupts until reset */
#define CTRL5_NEW_DATA_INT         (0x20)   /* enable new data interrupt */
#define CTRL5_ENABLE_ADV_INT       (0x40)   /* enable advanced interrupts */
#define CTRL5_SPI4                 (0x80)   /* SPI uses 4-wire interface */

/** @} */

extern bool bma020_init(sensor_t *, int);

#ifdef __cplusplus
}
#endif

#endif
