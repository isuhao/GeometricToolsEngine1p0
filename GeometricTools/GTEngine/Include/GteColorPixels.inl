// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
// PixelRGBA8
//----------------------------------------------------------------------------
inline PixelRGBA8::~PixelRGBA8 ()
{
}
//----------------------------------------------------------------------------
inline PixelRGBA8::PixelRGBA8 ()
{
    // Uninitialized;
}
//----------------------------------------------------------------------------
inline PixelRGBA8::PixelRGBA8 (PixelRGBA8 const& pixel)
{
    *this = pixel;
}
//----------------------------------------------------------------------------
inline PixelRGBA8::PixelRGBA8 (unsigned char inR, unsigned char inG,
    unsigned char inB, unsigned char inA)
    :
    r(inR), g(inG), b(inB), a(inA)
{
}
//----------------------------------------------------------------------------
inline PixelRGBA8& PixelRGBA8::operator= (PixelRGBA8 const& pixel)
{
    r = pixel.r;
    g = pixel.g;
    b = pixel.b;
    a = pixel.a;
    return *this;
}
//----------------------------------------------------------------------------
inline bool PixelRGBA8::operator== (PixelRGBA8 const& color) const
{
    return r == color.r && g == color.g && b == color.b && a == color.a;
}
//----------------------------------------------------------------------------
inline bool PixelRGBA8::operator!= (PixelRGBA8 const& color) const
{
    return r != color.r || g != color.g || b != color.b || a != color.a;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// PixelBGRA8
//----------------------------------------------------------------------------
inline PixelBGRA8::~PixelBGRA8 ()
{
}
//----------------------------------------------------------------------------
inline PixelBGRA8::PixelBGRA8 ()
{
    // Uninitialized;
}
//----------------------------------------------------------------------------
inline PixelBGRA8::PixelBGRA8 (PixelBGRA8 const& pixel)
{
    *this = pixel;
}
//----------------------------------------------------------------------------
inline PixelBGRA8::PixelBGRA8 (unsigned char inB, unsigned char inG,
    unsigned char inR, unsigned char inA)
    :
    b(inB), g(inG), r(inR), a(inA)
{
}
//----------------------------------------------------------------------------
inline PixelBGRA8& PixelBGRA8::operator= (PixelBGRA8 const& pixel)
{
    b = pixel.b;
    g = pixel.g;
    r = pixel.r;
    a = pixel.a;
    return *this;
}
//----------------------------------------------------------------------------
inline bool PixelBGRA8::operator== (PixelBGRA8 const& color) const
{
    return b == color.b && g == color.g && r == color.r && a == color.a;
}
//----------------------------------------------------------------------------
inline bool PixelBGRA8::operator!= (PixelBGRA8 const& color) const
{
    return b != color.b || g != color.g || r != color.r || a != color.a;
}
//----------------------------------------------------------------------------
