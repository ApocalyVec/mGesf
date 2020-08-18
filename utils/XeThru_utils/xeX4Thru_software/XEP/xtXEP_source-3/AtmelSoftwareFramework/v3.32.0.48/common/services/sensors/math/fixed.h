/**
 * \file
 *
 * \brief ASF Sensor API fixed point data type
 *
 * This header defines a fixed-point data type and associated arithmetic
 * operations.  The \p fixed type approximates fractional quantities using
 * a pair of integers, designated as the mantissa and exponent, such that
 * \f$ n2^{-q} \f$ given integer exponent \a q and integer mantissa \a n
 *
 * Because exponent \a q is a power of 2 it can also be understood as the
 * number of binary digits into \a n where the binary point is placed.  And
 * since mantissa \a n is a factor of a power of two, it is only necessary
 * to store the mantissa and the number of fractional bits.  This is also
 * known as \a Q number format.
 *
 * The mantissa is stored and operated on as a signed two's complement
 * number, where \a Q specifies the number of fractional bits  and, by
 * implication, the location of a virtual fixed decimal point within the
 * mantissa.
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

#ifndef _FIXED_MATH_H_
#define _FIXED_MATH_H_

#include "fixed_t.h"

#ifdef  __cplusplus

namespace math {

/** \brief Fixed point type */
class fixed {

private:
	/** \brief class private constants */
	enum { Q_DFLT = 15 };

	/** \name class private attributes */
	/** @{ */
	fixed_t val;
	int     Q;
	/** @} */

	/** \brief fixed point "Q" format conversion */
	inline fixed_t fixed_conv (const fixed & f) const
		{ return ::fixed_conv (f.val, f.Q, Q); }

	inline int QI () const { return 32 - Q; }  /* the number of integer bits */
	inline int QF () const { return Q; }       /* the number of fraction bits */

	inline double resolution () const { return 1.0 / (1L << Q); }

	inline double min () const { return -(1L << QI ()); }
	inline double max () const { return +(1L << QI ()) - resolution (); }

public:
	/** \name class construction and type conversion */
	/** @{ */
	fixed () : val (0), Q (Q_DFLT) {}
	fixed (int            n, int q = Q_DFLT) : val (long_to_fixed  (n,q)), Q (q) {}
	fixed (unsigned int   n, int q = Q_DFLT) : val (long_to_fixed  (n,q)), Q (q) {}
	fixed (short          n, int q = Q_DFLT) : val (long_to_fixed  (n,q)), Q (q) {}
	fixed (unsigned short n, int q = Q_DFLT) : val (long_to_fixed  (n,q)), Q (q) {}
	fixed (long           n, int q = Q_DFLT) : val (long_to_fixed  (n,q)), Q (q) {}
	fixed (unsigned long  n, int q = Q_DFLT) : val (long_to_fixed  (n,q)), Q (q) {}
	fixed (float          x, int q = Q_DFLT) : val (float_to_fixed (x,q)), Q (q) {}
	fixed (double         x, int q = Q_DFLT) : val (double_to_fixed(x,q)), Q (q) {}

	template <typename T>
	fixed & operator = (const T x) { val = fixed(x, Q).val; return *this; }

	inline operator bool ()           const { return val ? true : false; }
	inline operator int ()            const { return fixed_to_long   (val,Q); }
	inline operator unsigned int ()   const { return fixed_to_long   (val,Q); }
	inline operator short ()          const { return fixed_to_long   (val,Q); }
	inline operator unsigned short () const { return fixed_to_long   (val,Q); }
	inline operator long ()           const { return fixed_to_long   (val,Q); }
	inline operator unsigned long  () const { return fixed_to_long   (val,Q); }
	inline operator float ()          const { return fixed_to_float  (val,Q); }
	inline operator double ()         const { return fixed_to_double (val,Q); }
	/** @} */

	fixed ceil  () const;
	fixed floor () const;
	fixed sqrt  () const;

	/** \name class member operators (<fixed> op <fixed> operands) */
	/** @{ */
 	const fixed & operator += (const fixed & f)
		{ val = ::fixed_add (val, fixed_conv (f)); return *this; }

	const fixed & operator -= (const fixed & f)
		{ val = ::fixed_sub (val, fixed_conv (f)); return *this; }

	const fixed & operator *= (const fixed & f)
		{ val = ::fixed_mul (val, fixed_conv (f), Q); return *this; }

	const fixed & operator /= (const fixed & f)
		{ val = ::fixed_div (val, fixed_conv (f), Q); return *this; }

	const fixed operator + (const fixed & f) const
		{ return (fixed (*this) += f); }

	const fixed operator - (const fixed & f) const
		{ return (fixed (*this) -= f); }

	const fixed operator * (const fixed & f) const
		{ return (fixed(*this) *= f); }

	const fixed operator / (const fixed & f) const
		{ return (fixed(*this) /= f); }

	bool operator == (const fixed & f) const
		{ return (val == fixed_conv (f)); }

	bool operator != (const fixed & f) const
		{ return ! (f == *this); }

	bool operator < (const fixed & f) const
		{ return (val < fixed_conv (f)); }

	bool operator > (const fixed & f) const
		{ return (val > fixed_conv (f)); }

	bool operator >= (const fixed & f) const
		{ return (val >= fixed_conv (f)); }

	bool operator <= (const fixed & f) const
		{ return (val <= fixed_conv (f)); }
	/** @} */

	/** \name class member operators (<fixed> unary operators) */
	/** @{ */
	bool operator ! () const { return 0 == val; }

	const fixed operator + () const { return fixed (*this); }
	const fixed operator - () const { return fixed (-double(*this), Q); }

	const fixed & operator ++ () { *this += 1; return *this; }
	const fixed & operator -- () { *this -= 1; return *this; }
	/** @} */

	/** \name class member operators (<fixed> op type <T> operands) */
	/** @{ */
	template <typename T> const fixed & operator += (const T & n)
		{ val = ::fixed_add (val, fixed(n, Q).val); return *this; }
	template <typename T> const fixed & operator -= (const T & n)
		{ val = ::fixed_sub (val, fixed(n, Q).val); return *this; }
	template <typename T> const fixed & operator *= (const T & n)
		{ val = ::fixed_mul (val, fixed(n, Q).val, Q); return *this; }
	template <typename T> const fixed & operator /= (const T & n)
		{ val = ::fixed_div (val, fixed(n, Q).val, Q); return *this; }

	template <typename T> const fixed operator +  (const T & n) const
		{ return (fixed(*this) += n); }
	template <typename T> const fixed operator -  (const T & n) const
		{ return (fixed(*this) -= n); }
	template <typename T> const fixed operator *  (const T & n) const
		{ return (fixed(*this) *= n); }
	template <typename T> const fixed operator /  (const T & n) const
		{ return (fixed(*this) /= n); }

	template <typename T> bool operator == (const T & n) const
		{ return (*this == fixed(n)); }
	template <typename T> bool operator != (const T & n) const
		{ return (*this != fixed(n)); }

	template <typename T> bool operator <  (const T & n) const
		{ return (*this <  fixed(n)); }
	template <typename T> bool operator >  (const T & n) const
		{ return (*this >  fixed(n)); }
	template <typename T> bool operator >= (const T & n) const
		{ return (*this >= fixed(n)); }
	template <typename T> bool operator <= (const T & n) const
		{ return (*this <= fixed(n)); }
	/** @} */

	/** \name class friend operators (type <T> op <fixed> operands) */
	/** @{ */
	template <typename T> friend const T & operator += (T & a, const fixed & b)
		{ a = a + T (b); return a; }
	template <typename T> friend const T & operator -= (T & a, const fixed & b)
		{ a = a - T (b); return a; }
	template <typename T> friend const T & operator *= (T & a, const fixed & b)
		{ a = a * T (b); return a; }
	template <typename T> friend const T & operator /= (T & a, const fixed & b)
		{ a = a / T (b); return a; }

	template <typename T> friend const fixed operator + (const T a, const fixed & b)
		{ return fixed(a, b.Q) + b; }
	template <typename T> friend const fixed operator - (const T a, const fixed & b)
		{ return fixed(a, b.Q) - b; }
	template <typename T> friend const fixed operator * (const T a, const fixed & b)
		{ return fixed(a, b.Q) * b; }
	template <typename T> friend const fixed operator / (const T a, const fixed & b)
		{ return fixed(a, b.Q) / b; }

	template <typename T> friend bool operator == (const T a, const fixed & b)
		{ return fixed(a, b.Q) == b; }
	template <typename T> friend bool operator != (const T a, const fixed & b)
		{ return fixed(a, b.Q) != b; }
	template <typename T> friend bool operator <  (const T a, const fixed & b)
		{ return fixed(a, b.Q)  < b; }
	template <typename T> friend bool operator >  (const T a, const fixed & b)
		{ return fixed(a, b.Q)  > b; }
	template <typename T> friend bool operator >= (const T a, const fixed & b)
		{ return fixed(a, b.Q) >= b; }
	template <typename T> friend bool operator <= (const T a, const fixed & b)
		{ return fixed(a, b.Q) <= b; }
	/** @} */
};

}

/** \brief round up value */
inline math::fixed ceil (math::fixed const & f) { return f.ceil (); }

/** \brief round down value */
inline math::fixed floor (math::fixed const & f) { return f.floor (); }

/** \brief absolute value */
inline math::fixed fabs (math::fixed const & f) { return (f < 0) ? -f : f; }

/** \brief square root */
inline math::fixed sqrt (math::fixed const & f) { return f.sqrt (); }

#endif

#endif
