/**
 * \file
 *
 * \file
 *
 * \brief Honeywell HMC5883L 3-axis magnetometer.
 *
 * This file contains functions for initializing and reading data
 * from a Honeywell HMC5883L 3-axis magnetometer.
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
#ifndef _HMC5883L_H_
#define _HMC5883L_H_

#include <asf.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TWI/I2C slave address (write @ 0x3c on bus, read @ 0x3d on bus) */
#define HMC5883L_TWI_ADDR       (0x1e)

/* Signed axis data sample resolution (bits) */
#define HMC5883L_DATA_RESOLUTION (12)

/** \brief Data scaling - varies by range/gain setting */
/** @{ */

#define SCALE_0_9GA             (1370)  /* 0.9 Ga (1370 counts / Gauss) */
#define SCALE_1_3GA             (1090)  /* 1.3 Ga (1090 counts / Gauss) */
#define SCALE_1_9GA             (820)   /* 1.9 Ga (820 counts / Gauss) */
#define SCALE_2_5GA             (660)   /* 2.5 Ga (660 counts / Gauss) */
#define SCALE_4_0GA             (440)   /* 4.0 Ga (440 counts / Gauss) */
#define SCALE_4_7GA             (390)   /* 4.7 Ga (390 counts / Gauss) */
#define SCALE_5_6GA             (330)   /* 5.6 Ga (330 counts / Gauss) */
#define SCALE_8_1GA             (230)   /* 8.1 Ga (230 counts / Gauss) */

/** @} */

/** \brief Device ID Definitions */
/** @{ */

#define ID_A_DEFAULT            (0x48)  /* normal value of ID register A */
#define ID_B_DEFAULT            (0x34)  /* normal value of ID register B */
#define ID_C_DEFAULT            (0x33)  /* normal value of ID register C */
#define HMC5883L_DEV_ID         (0x483433)  /* combined ID value */

/** @} */

/** \brief HMC5883L Register Addresses */
/** @{ */

#define HMC5883L_CONFIG_REG_A   (0x00)  /* configuration register A */
#define HMC5883L_CONFIG_REG_B   (0x01)  /* configuration register B */
#define HMC5883L_MODE_REG       (0x02)  /* mode register */
#define HMC5883L_MAG_X_HI       (0x03)  /* X mag reading - MSB */
#define HMC5883L_MAG_X_LO       (0x04)  /* X mag reading - LSB */
#define HMC5883L_MAG_Z_HI       (0x05)  /* Z mag reading - MSB */
#define HMC5883L_MAG_Z_LO       (0x06)  /* Z mag reading - LSB */
#define HMC5883L_MAG_Y_HI       (0x07)  /* Y mag reading - MSB */
#define HMC5883L_MAG_Y_LO       (0x08)  /* Y mag reading - LSB */
#define HMC5883L_STATUS_REG     (0x09)  /* device status */
#define HMC5883L_ID_REG_A       (0x0a)  /* ID register A */
#define HMC5883L_ID_REG_B       (0x0b)  /* ID register B */
#define HMC5883L_ID_REG_C       (0x0c)  /* ID register C */

/** @} */

/** \brief HMC5883L Register Bit Definitions */
/** @{ */

/* HMC5883L_CONFIG_REG_A (0x00) */

#define MEAS_MODE               (0x03)  /* measurement mode mask (2 bits) */
#define MEAS_MODE_NORM          (0x00)  /* normal measurement mode */
#define MEAS_MODE_POS           (0x01)  /* positive bias */
#define MEAS_MODE_NEG           (0x02)  /* negative bias */

#define DATA_RATE               (0x1c)  /* data rate mask (3 bits) */
#define DATA_RATE_0_75HZ        (0x00)  /* 0.75 Hz */
#define DATA_RATE_1_5HZ         (0x04)  /* 1.5 Hz */
#define DATA_RATE_3HZ           (0x08)  /* 3 Hz */
#define DATA_RATE_7_5HZ         (0x0c)  /* 7.5 Hz */
#define DATA_RATE_15HZ          (0x10)  /* 15 Hz */
#define DATA_RATE_30HZ          (0x14)  /* 30 Hz */
#define DATA_RATE_75HZ          (0x18)  /* 75 Hz */

#define MEAS_AVG                (0x60)  /* sample average mask (2 bits) */
#define MEAS_AVG_1              (0x00)  /* output = 1 sample (no avg.) */
#define MEAS_AVG_2              (0x20)  /* output = 2 samples averaged */
#define MEAS_AVG_4              (0x40)  /* output = 4 samples averaged */
#define MEAS_AVG_8              (0x60)  /* output = 8 samples averaged */

/* HMC5883L_CONFIG_REG_B (0x01) */

#define GAIN_0_9GA              (0x00)  /* +/- 0.9 Ga (1370 counts/ Gauss) */
#define GAIN_1_3GA              (0x20)  /* +/- 1.3 Ga (1090 counts/ Gauss) */
#define GAIN_1_9GA              (0x40)  /* +/- 1.9 Ga (820 counts / Gauss) */
#define GAIN_2_5GA              (0x60)  /* +/- 2.5 Ga (660 counts / Gauss) */
#define GAIN_4_0GA              (0x80)  /* +/- 4.0 Ga (440 counts / Gauss) */
#define GAIN_4_7GA              (0xa0)  /* +/- 4.7 Ga (390 counts / Gauss) */
#define GAIN_5_6GA              (0xc0)  /* +/- 5.6 Ga (330 counts / Gauss) */
#define GAIN_8_1GA              (0xe0)  /* +/- 8.1 Ga (230 counts / Gauss) */

/* HMC5883L_MODE_REG (0x02) */

#define MODE_CONTIN             (0x00)  /* continuous conversion mode */
#define MODE_SINGLE             (0x01)  /* single measurement mode */
#define MODE_IDLE               (0x02)  /* idle mode */
#define MODE_SLEEP              (0x03)  /* sleep mode */

/* HMC5883L_STATUS_REG (0x09) */

#define STATUS_RDY              (0x01)  /* data ready */
#define STATUS_LOCK             (0x02)  /* data output locked */

/* Self-test Definitions */

#define HMC5883L_TEST_GAIN      GAIN_2_5GA  /* gain value during self-test */
#define HMC5883L_TEST_X_MIN     550         /* min X */
#define HMC5883L_TEST_X_NORM    766         /* normal X */
#define HMC5883L_TEST_X_MAX     850         /* max X */
#define HMC5883L_TEST_Y_MIN     550         /* min Y */
#define HMC5883L_TEST_Y_NORM    766         /* normal Y */
#define HMC5883L_TEST_Y_MAX     850         /* max Y */
#define HMC5883L_TEST_Z_MIN     550         /* min Z */
#define HMC5883L_TEST_Z_NORM    713         /* normal Z */
#define HMC5883L_TEST_Z_MAX     850         /* max Z */

/** @} */

extern bool hmc5883l_init(sensor_t *, int);

#ifdef __cplusplus
}
#endif

#endif
