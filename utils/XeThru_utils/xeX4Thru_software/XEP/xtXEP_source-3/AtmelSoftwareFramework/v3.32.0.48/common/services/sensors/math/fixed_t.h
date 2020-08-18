/**
 * \file
 *
 * \brief ASF Sensor API fixed point math operations
 *
 * This header defines types and operations that support basic fixed-point
 * arithmetic operations.  The \p fixed_t type is a signed 32-bit value that
 * is stored and operated on using integer operations.  The number of
 * fractional bits operated on will be context-specific.
 *
 * The conversion functions and some of the arithmetic functions require
 * a \a Q parameter specifying the number of fractional bits in the operands.
 * Arithmetic functions that do not require the \a Q parameter assume that
 * both operands have the same \a Q format (the same number of fractional bits).
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#ifndef _FIXED_MATH_TYPE_H_
#define _FIXED_MATH_TYPE_H_

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Signed fixed-point format storage type. */
typedef int32_t fixed_t;

#if 0
/** \brief Rounding conversion from floating-point to integer. */
static inline long round(double x)
{
	return (x >= 0 ? (long)(x + 0.5) : (long)(x - 0.5));
}
#endif

/** \brief fixed point \a Q format conversion
 *
 * This routine performs a change of exponent operation where
 * fixed-point value \a a in \a q1 format is converted to \a q2
 * format.  The change of binary exponent is derived from the
 * following relation:
 * \f[
 *      n2^{-p} = k2^{-r} \Longrightarrow k = n2^{r - p}
 * \f]
 * Thus, the converted value \a k is obtained by dividing \a n
 * by \f$ 2^{p - r} \f$ when \f$ (p > r) \f$ or by
 * multiplying by \f$ 2^{r - p} \f$ when \f$ (r \ge p) \f$.
 */
static inline fixed_t fixed_conv(fixed_t a, int q1, int q2)
{
	return (q1 == q2) ? a : ((q2 > q1) ? a << (q2 - q1) : a >> (q1 - q2));
}

/** \name Type Conversion Primitives
 * @{
 */
static inline fixed_t long_to_fixed(long n, int Q)
{
	return ((fixed_t)n << Q);
}

static inline fixed_t float_to_fixed(float x, int Q)
{
	return (fixed_t)(x * (float)(1L << Q));
}

static inline fixed_t double_to_fixed(double x, int Q)
{
	return (fixed_t)(x * (double)(1L << Q));
}

static inline long fixed_to_long(fixed_t f, int Q)
{
	return (f >> Q);
}

static inline float fixed_to_float(fixed_t f, int Q)
{
	return ((float)f / (1L << Q));
}

static inline double fixed_to_double(fixed_t f, int Q)
{
	return ((double)f / (1L << Q));
}
/** @} */

/** \name Addition and Subtraction Primitives
 * @{
 */
static inline fixed_t fixed_add(fixed_t a, fixed_t b)
{
	return (a + b);
}

static inline fixed_t fixed_addl(fixed_t a, long b, int Q)
{
	return (a + long_to_fixed(b, Q));
}

static inline fixed_t fixed_sub(fixed_t a, fixed_t b)
{
	return (a - b);
}

static inline fixed_t fixed_subl(fixed_t a, long b, int Q)
{
	return (a - long_to_fixed(b, Q));
}
/** @} */

/** \name Multiplication and Division Primitives
 * @{
 */
static inline fixed_t fixed_mul(fixed_t a, fixed_t b, int Q)
{
	return (((int64_t)a * b) >> Q);
}

static inline fixed_t fixed_mull(fixed_t a, long b)
{
	return ((int64_t)a * b);
}

static inline fixed_t fixed_mul_rounded(fixed_t a, fixed_t b, int Q)
{
	return ((((int64_t)a * b) + (1L << (Q - 1))) >> Q);
}

static inline fixed_t fixed_div(fixed_t a, fixed_t b, int Q)
{
	return (((int64_t)a << Q) / b);
}

static inline fixed_t fixed_divl(fixed_t a, long b)
{
	return ((int64_t)a / b);
}

static inline fixed_t fixed_div_rounded(fixed_t a, fixed_t b, int Q)
{
	return ((((int64_t)a << Q) + (b >> 1)) / b);
}
/** @} */

/** \name Algebraic and Transcendental Functions
 * @{
 */

/** \brief Calculate the square root of an integer
 *
 * This routine calculates the integer square root, \a isqrt(), of a positive
 * integer argument \a n:
 * \f[
 *          isqrt(n) = \lfloor n^{1/2} \rfloor
 * \f]
 * The computed value is the largest positive integer \a m with a square
 * that is less than or equal to the given integer \a n such that
 * \f$ m^{2} \le n \f$.
 *
 * \param   f   A fixed-point value.  Negative values will be computed using
 *              the unsigned magnitude without reporting a domain error.
 * \param   Q   The number of fractional bits in parameter \a f.
 *
 * \return  The integer square root.
 */
extern fixed_t fixed_sqrt(fixed_t f, int Q);

/** @} */

/** \name fixed-point resolution and range utilities
 *
 * The fixed_range() and fixed_resolution() utilities can be used to define,
 * verify, test, and characterize fixed-point formats for applications that
 * are being converted from floating-point or implemented in fixed-point.
 * @{
 */

/** \brief signed fixed-point range
 *
 * Given a signed fixed-point format \p Qm.f, where \a f represents the number
 * of fractional bits and \a m the number of integer bits, this routine
 * calculates the the number of integer bits \a m required to represent real
 * data types in the interval [\p x_min, \p x_max].
 *
 * The \p x_min and \p x_max interval endpoints are the smallest and largest
 * integer portions, respectively, of the numbers that must be represented in
 * fixed-point.
 *
 * \param   x_min   The smallest signed integer portion of a real type.
 * \param   x_max   The largest signed integer portion of a real type.
 *
 * \return  The required number of integer bits required for
 *          a fixed-point implementation.
 */
extern int fixed_range(double x_min, double x_max);

/** \brief signed fixed-point resolution
 *
 * Given a signed fixed-point format \p Qm.f, where \a f represents the number
 * of fractional bits and \a m the number of integer bits, this routine
 * calculates the the number of fractional bits \a f required to represent
 * real data types with resolution \f$ \epsilon = \frac{1}{2^{f}} \f$.  For
 * example, a signed variable \a A = 3.141 requires \epsilon <= 0.001.
 *
 * \param   epsilon     The required resolution of the fixed-point number.
 *
 * \return  The required number of fractional bits required for
 *          a fixed-point implementation.
 */
extern int fixed_resolution(double epsilon);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
