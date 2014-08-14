// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GTEngineDEF.h"

namespace gte
{

class GTE_IMPEXP PixelRGBA8
{
public:
    // Construction and destruction.  The default constructor does not
    // initialize its data.
    inline ~PixelRGBA8 ();
    inline PixelRGBA8 ();
    inline PixelRGBA8 (PixelRGBA8 const& pixel);
    inline PixelRGBA8 (unsigned char inR, unsigned char inG,
        unsigned char inB, unsigned char inA);

    // Assignment.
    inline PixelRGBA8& operator= (PixelRGBA8 const& pixel);

    // Comparisons.
    inline bool operator== (PixelRGBA8 const& color) const;
    inline bool operator!= (PixelRGBA8 const& color) const;

    unsigned char r, g, b, a;
};


class GTE_IMPEXP PixelBGRA8
{
public:
    // Construction and destruction.  The default constructor does not
    // initialize its data.
    inline ~PixelBGRA8 ();
    inline PixelBGRA8 ();
    inline PixelBGRA8 (PixelBGRA8 const& pixel);
    inline PixelBGRA8 (unsigned char inB, unsigned char inG,
        unsigned char inR, unsigned char inA);

    // Assignment.
    inline PixelBGRA8& operator= (PixelBGRA8 const& pixel);

    // Comparisons.
    inline bool operator== (PixelBGRA8 const& color) const;
    inline bool operator!= (PixelBGRA8 const& color) const;

    unsigned char b, g, r, a;
};

#include "GteColorPixels.inl"

}
