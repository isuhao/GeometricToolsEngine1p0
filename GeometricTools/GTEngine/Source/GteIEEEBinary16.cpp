// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "GTEnginePCH.h"
#include "GteIEEEBinary16.h"

namespace gte
{
//----------------------------------------------------------------------------
IEEEBinary16::~IEEEBinary16()
{
}
//----------------------------------------------------------------------------
IEEEBinary16::IEEEBinary16()
    :
    IEEEBinary<_Float16, uint16_t, 16, 11>()
{
        // uninitialized
    }
//----------------------------------------------------------------------------
IEEEBinary16::IEEEBinary16(IEEEBinary16 const& object)
    :
    IEEEBinary<_Float16, uint16_t, 16, 11>(object)
{
    }
//----------------------------------------------------------------------------
IEEEBinary16::IEEEBinary16(float number)
    :
    IEEEBinary<_Float16, uint16_t, 16, 11>()
{
        union { float n; uint32_t e; } temp = { number };
        encoding = Convert32To16(temp.e);
    }
//----------------------------------------------------------------------------
IEEEBinary16::IEEEBinary16(uint16_t encoding)
    :
    IEEEBinary<_Float16, uint16_t, 16, 11>(encoding)
{
    }
//----------------------------------------------------------------------------
float IEEEBinary16::Convert() const
{
    union { uint32_t e; float n; } temp = { Convert16To32(encoding) };
    return temp.n;
}
//----------------------------------------------------------------------------
IEEEBinary16& IEEEBinary16::operator= (IEEEBinary16 const& object)
{
    IEEEBinary<_Float16, uint16_t, 16, 11>::operator=(object);
    return *this;
}
//----------------------------------------------------------------------------
uint16_t IEEEBinary16::Convert32To16(uint32_t encoding)
{
    // Extract the channels for the binary32 number.
    uint32_t sign32 = (encoding & F32_SIGN_MASK);
    uint32_t biased32 =
        ((encoding & F32_BIASED_EXPONENT_MASK) >> F32_NUM_TRAILING_BITS);
    uint32_t trailing32 = (encoding & F32_TRAILING_MASK);
    uint32_t nonneg32 = (encoding & F32_NOT_SIGN_MASK);

    // Generate the channels for the IEEEBinary16 number.
    uint16_t sign16 = static_cast<uint16_t>(sign32 >> DIFF_NUM_ENCODING_BITS);
    uint16_t biased16, trailing16;
    uint32_t intpart, frcpart;

    if (biased32 == 0)
    {
        // nonneg32 is 32-zero or 32-subnormal, nearest is 16-zero.
        return sign16;
    }

    if (biased32 < F32_MAX_BIASED_EXPONENT)
    {
        // nonneg32 is 32-normal.
        if (nonneg32 <= F16_AVR_MIN_SUBNORMAL_ZERO)
        {
            // nonneg32 <= 2^{-25}, nearest is 16-zero.
            return sign16;
        }

        if (nonneg32 <= F16_MIN_SUBNORMAL)
        {
            // 2^{-25} < nonneg32 <= 2^{-24}, nearest is 16-min-subnormal.
            return sign16 | IEEEBinary16::MIN_SUBNORMAL;
        }

        if (nonneg32 < F16_MIN_NORMAL)
        {
            // 2^{-24} < nonneg32 < 2^{-14}, round to nearest
            // 16-subnormal with ties to even.  Note that biased16 is zero.
            intpart =
                ((trailing32 & INT_PART_MASK) >> DIFF_NUM_TRAILING_BITS);
            frcpart = (trailing32 & FRC_PART_MASK);
            if (frcpart > FRC_HALF || (frcpart == FRC_HALF && (intpart & 1)))
            {
                // If there is a carry into the exponent, the nearest is
                // actually 16-min-normal 1.0*2^{-14}, so the high-order bit
                // of trailing16 makes biased16 equal to 1 and the result is
                // correct.
                ++intpart;
            }
            trailing16 = static_cast<uint16_t>(
                intpart << IEEEBinary16::NUM_TRAILING_BITS);
            return sign16 | trailing16;
        }

        if (nonneg32 <= F16_MAX_NORMAL)
        {
            // 2^{-14} <= nonneg32 <= 1.1111111111*2^{15}, round to nearest
            // 16-normal with ties to even.
            biased16 = static_cast<uint16_t>((biased32 - F32_EXPONENT_BIAS +
                IEEEBinary16::EXPONENT_BIAS)
                << IEEEBinary16::NUM_TRAILING_BITS);
            intpart =
                ((trailing32 & INT_PART_MASK) >> DIFF_NUM_TRAILING_BITS);
            frcpart = (trailing32 & FRC_PART_MASK);
            if (frcpart > FRC_HALF || (frcpart == FRC_HALF && (intpart & 1)))
            {
                // If there is a carry into the exponent, the addition of
                // trailing16 to biased16 (rather than or-ing) produces the
                // correct result.
                ++intpart;
            }
            trailing16 = static_cast<uint16_t>(
                intpart << IEEEBinary16::NUM_TRAILING_BITS);
            return sign16 | (biased16 + trailing16);
        }

        if (nonneg32 < F16_AVR_MAX_NORMAL_INFINITY)
        {
            // 1.1111111111*2^{15} < nonneg32 < (MAX_NORMAL+INFINITY)/2, so
            // the number is closest to 16-max-normal.
            return sign16 | IEEEBinary16::MAX_NORMAL;
        }

        // nonneg32 >= (MAX_NORMAL+INFINITY)/2, so convert to 16-infinite.
        return sign16 | IEEEBinary16::POS_INFINITY;
    }

    if (trailing32 == 0)
    {
        // The number is 32-infinite.  Convert to 16-infinite.
        return sign16 | IEEEBinary16::POS_INFINITY;
    }

    // The number is 32-NaN.  Convert to 16-NaN with 16-payload the high-order
    // 9 bits of the 32-payload.  The code also grabs the 32-quietNaN mask
    // bit.
    uint16_t maskPayload = static_cast<uint16_t>(
        (trailing32 & 0x007FE000u) >> 13);
    return sign16 | IEEEBinary16::EXPONENT_MASK | maskPayload;
}
//----------------------------------------------------------------------------
uint32_t IEEEBinary16::Convert16To32(uint16_t encoding)
{
    // Extract the channels for the IEEEBinary16 number.
    uint16_t sign16 = (encoding & IEEEBinary16::SIGN_MASK);
    uint16_t biased16 = ((encoding & IEEEBinary16::EXPONENT_MASK)
        >> IEEEBinary16::NUM_TRAILING_BITS);
    uint16_t trailing16 = (encoding & IEEEBinary16::TRAILING_MASK);

    // Generate the channels for the binary32 number.
    uint32_t sign32 = static_cast<uint32_t>(sign16 << DIFF_NUM_ENCODING_BITS);
    uint32_t biased32, trailing32;

    if (biased16 == 0)
    {
        if (trailing16 == 0)
        {
            // The number is 16-zero.  Convert to 32-zero.
            return sign32;
        }
        else
        {
            // The number is 16-subnormal.  Convert to 32-normal.
            trailing32 = static_cast<uint32_t>(trailing16);
            int32_t leading = GetLeadingBit(trailing32);
            int32_t shift = 23 - leading;
            biased32 = static_cast<uint32_t>(F32_EXPONENT_BIAS - 1 - shift);
            trailing32 = (trailing32 << shift) & F32_TRAILING_MASK;
            return sign32 | (biased32 << F32_NUM_TRAILING_BITS) | trailing32;
        }
    }

    if (biased16 < IEEEBinary16::MAX_BIASED_EXPONENT)
    {
        // The number is 16-normal.  Convert to 32-normal.
        biased32 = static_cast<uint32_t>(biased16 - IEEEBinary16::EXPONENT_BIAS +
            F32_EXPONENT_BIAS);
        trailing32 = (static_cast<uint32_t>(
            trailing16) << DIFF_NUM_TRAILING_BITS);
        return sign32 | (biased32 << F32_NUM_TRAILING_BITS) | trailing32;
    }

    if (trailing16 == 0)
    {
        // The number is 16-infinite.  Convert to 32-infinite.
        return sign32 | F32_BIASED_EXPONENT_MASK;
    }

    // The number is 16-NaN.  Convert to 32-NaN with 16-payload embedded in
    // the high-order 9 bits of the 32-payload.  The code also copies the
    // 16-quietNaN mask bit.
    uint32_t maskPayload =
        ((trailing16 & IEEEBinary16::TRAILING_MASK) << DIFF_PAYLOAD_SHIFT);
    return sign32 | F32_BIASED_EXPONENT_MASK | maskPayload;
}
//----------------------------------------------------------------------------
IEEEBinary16 operator- (IEEEBinary16 x)
{
    uint16_t result = static_cast<uint16_t>(x) ^ IEEEBinary16::SIGN_MASK;
    return result;
}
//----------------------------------------------------------------------------
float operator+ (IEEEBinary16 x, IEEEBinary16 y)
{
    return static_cast<float>(x)+static_cast<float>(y);
}
//----------------------------------------------------------------------------
float operator- (IEEEBinary16 x, IEEEBinary16 y)
{
    return static_cast<float>(x)-static_cast<float>(y);
}
//----------------------------------------------------------------------------
float operator* (IEEEBinary16 x, IEEEBinary16 y)
{
    return static_cast<float>(x)* static_cast<float>(y);
}
//----------------------------------------------------------------------------
float operator/ (IEEEBinary16 x, IEEEBinary16 y)
{
    return static_cast<float>(x) / static_cast<float>(y);
}
//----------------------------------------------------------------------------
float operator+ (IEEEBinary16 x, float y)
{
    return static_cast<float>(x)+y;
}
//----------------------------------------------------------------------------
float operator- (IEEEBinary16 x, float y)
{
    return static_cast<float>(x)-y;
}
//----------------------------------------------------------------------------
float operator* (IEEEBinary16 x, float y)
{
    return static_cast<float>(x)* y;
}
//----------------------------------------------------------------------------
float operator/ (IEEEBinary16 x, float y)
{
    return static_cast<float>(x) / y;
}
//----------------------------------------------------------------------------
float operator+ (float x, IEEEBinary16 y)
{
    return x + static_cast<float>(y);
}
//----------------------------------------------------------------------------
float operator- (float x, IEEEBinary16 y)
{
    return x - static_cast<float>(y);
}
//----------------------------------------------------------------------------
float operator* (float x, IEEEBinary16 y)
{
    return x * static_cast<float>(y);
}
//----------------------------------------------------------------------------
float operator/ (float x, IEEEBinary16 y)
{
    return x / static_cast<float>(y);
}
//----------------------------------------------------------------------------
IEEEBinary16& operator+= (IEEEBinary16& x, IEEEBinary16 y)
{
    x = static_cast<float>(x)+static_cast<float>(y);
    return x;
}
//----------------------------------------------------------------------------
IEEEBinary16& operator-= (IEEEBinary16& x, IEEEBinary16 y)
{
    x = static_cast<float>(x)-static_cast<float>(y);
    return x;
}
//----------------------------------------------------------------------------
IEEEBinary16& operator*= (IEEEBinary16& x, IEEEBinary16 y)
{
    x = static_cast<float>(x)* static_cast<float>(y);
    return x;
}
//----------------------------------------------------------------------------
IEEEBinary16& operator/= (IEEEBinary16& x, IEEEBinary16 y)
{
    x = static_cast<float>(x) / static_cast<float>(y);
    return x;
}
//----------------------------------------------------------------------------
IEEEBinary16& operator+= (IEEEBinary16& x, float y)
{
    x = static_cast<float>(x)+y;
    return x;
}
//----------------------------------------------------------------------------
IEEEBinary16& operator-= (IEEEBinary16& x, float y)
{
    x = static_cast<float>(x)-y;
    return x;
}
//----------------------------------------------------------------------------
IEEEBinary16& operator*= (IEEEBinary16& x, float y)
{
    x = static_cast<float>(x)* y;
    return x;
}
//----------------------------------------------------------------------------
IEEEBinary16& operator/= (IEEEBinary16& x, float y)
{
    x = static_cast<float>(x) / y;
    return x;
}
//----------------------------------------------------------------------------
#if 0
IEEEBinary16 fabs(IEEEBinary16 x)
{
    uint16_t result = static_cast<uint16_t>(x)& IEEEBinary16::NOT_SIGN_MASK;
    return result;
}
//----------------------------------------------------------------------------
IEEEBinary16 sqrt(IEEEBinary16 x)
{
    return ::sqrt(static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 exp(IEEEBinary16 x)
{
    return ::exp(static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 log(IEEEBinary16 x)
{
    return ::log(static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 log10(IEEEBinary16 x)
{
    return ::log10(static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 pow(IEEEBinary16 x, IEEEBinary16 y)
{
    // TODO.  If x < 0, y must be an even integer.
    return ::pow(static_cast<float>(x), static_cast<float>(y));
}
//----------------------------------------------------------------------------
IEEEBinary16 sin(IEEEBinary16 x)
{
    return ::sin(static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 cos(IEEEBinary16 x)
{
    return ::cos(static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 tan(IEEEBinary16 x)
{
    return ::tan(static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 asin(IEEEBinary16 x)
{
    return ::asin(static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 acos(IEEEBinary16 x)
{
    return ::acos(static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 atan(IEEEBinary16 x)
{
    return ::atan(static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 atan2(IEEEBinary16 y, IEEEBinary16 x)
{
    return ::atan2(static_cast<float>(y), static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 sinh(IEEEBinary16 x)
{
    return ::sinh(static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 cosh(IEEEBinary16 x)
{
    return ::cosh(static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 tanh(IEEEBinary16 x)
{
    return ::tanh(static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 copysign(IEEEBinary16 x, IEEEBinary16 y)
{
    uint16_t nonnegative =
        static_cast<uint16_t>(x)& IEEEBinary16::NOT_SIGN_MASK;
    uint16_t sign = static_cast<uint16_t>(y)& IEEEBinary16::SIGN_MASK;
    return static_cast<uint16_t>(sign | nonnegative);
}
//----------------------------------------------------------------------------
IEEEBinary16 expm1(IEEEBinary16 x)
{
    return exp(static_cast<float>(x)) - 1.0f;
}
//----------------------------------------------------------------------------
IEEEBinary16 exp2(IEEEBinary16 x)
{
    return pow(2.0f, static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 exp2m1(IEEEBinary16 x)
{
    return pow(2.0f, static_cast<float>(x)) - 1.0f;
}
//----------------------------------------------------------------------------
IEEEBinary16 exp10(IEEEBinary16 x)
{
    return pow(10.0f, static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 exp10m1(IEEEBinary16 x)
{
    return pow(10.0f, static_cast<float>(x)) - 1.0f;
}
//----------------------------------------------------------------------------
IEEEBinary16 log2(IEEEBinary16 x)
{
    union { uint32_t encoding; float number; } invLn2 = { 0x3FB8AA3Bu };
    return log(static_cast<float>(x))*invLn2.number;
}
//----------------------------------------------------------------------------
IEEEBinary16 logp1(IEEEBinary16 x)
{
    return log(1.0f + static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 log2p1(IEEEBinary16 x)
{
    union { uint32_t encoding; float number; } invLn2 = { 0x3FB8AA3Bu };
    return log(1.0f + static_cast<float>(x))*invLn2.number;
}
//----------------------------------------------------------------------------
IEEEBinary16 log10p1(IEEEBinary16 x)
{
    return log10(1.0f + static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 hypot(IEEEBinary16 x, IEEEBinary16 y)
{
    float fx = static_cast<float>(x);
    float fy = static_cast<float>(y);
    return sqrt(fx*fx + fy*fy);
}
//----------------------------------------------------------------------------
IEEEBinary16 invsqrt(IEEEBinary16 x)
{
    return 1.0f / sqrt(static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 compound(IEEEBinary16 x, int32_t n)
{
    return pow(1.0f + static_cast<float>(x), static_cast<float>(n));
}
//----------------------------------------------------------------------------
IEEEBinary16 rootn(IEEEBinary16 x, int32_t n)
{
    return pow(static_cast<float>(x), 1.0f / static_cast<float>(n));
}
//----------------------------------------------------------------------------
IEEEBinary16 pown(IEEEBinary16 x, int32_t n)
{
    return pow(static_cast<float>(x), static_cast<float>(n));
}
//----------------------------------------------------------------------------
IEEEBinary16 powr(IEEEBinary16 x, IEEEBinary16 y)
{
    // x >= 0 is required
    return pow(static_cast<float>(x), static_cast<float>(y));
}
//----------------------------------------------------------------------------
IEEEBinary16 sinpi(IEEEBinary16 x)
{
    union { uint32_t encoding; float number; } pi = { 0x40490FDBu };
    return sin(pi.number*static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 cospi(IEEEBinary16 x)
{
    union { uint32_t encoding; float number; } pi = { 0x40490FDBu };
    return cos(pi.number*static_cast<float>(x));
}
//----------------------------------------------------------------------------
IEEEBinary16 atanpi(IEEEBinary16 x)
{
    union { uint32_t encoding; float number; } pi = { 0x40490FDBu };
    union { uint32_t encoding; float number; } invpi = { 0x3EA2F983u };
    return atan(pi.number*static_cast<float>(x))*invpi.number;
}
//----------------------------------------------------------------------------
IEEEBinary16 atan2pi(IEEEBinary16 y, IEEEBinary16 x)
{
    union { uint32_t encoding; float number; } invpi = { 0x3EA2F983u };
    return atan2(static_cast<float>(y), static_cast<float>(x))*invpi.number;
}
//----------------------------------------------------------------------------
IEEEBinary16 asinh(IEEEBinary16 x)
{
    float fx = static_cast<float>(x);
    return log(fx + sqrt(1.0f + fx*fx));
}
//----------------------------------------------------------------------------
IEEEBinary16 acosh(IEEEBinary16 x)
{
    // Return the branch ln(x+sqrt(x*x-1)) for x >= 1.  The other branch is
    // ln(x-sqrt(x*x-1)).
    float fx = static_cast<float>(x);
    return log(fx + sqrt(fx*fx - 1.0f));
}
//----------------------------------------------------------------------------
IEEEBinary16 atanh(IEEEBinary16 x)
{
    float fx = static_cast<float>(x);  // must be in (-1,1)
    return 0.5f*log((1.0f + fx) / (1.0f - fx));
}
//----------------------------------------------------------------------------
#endif
}
