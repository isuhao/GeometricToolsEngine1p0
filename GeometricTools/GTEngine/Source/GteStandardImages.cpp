// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "GTEnginePCH.h"
#include "GteImage1.h"
#include "GteImage2.h"
#include "GteImage3.h"
#include "GteColorPixels.h"
using namespace gte;

// Explicit instantiation of image classes for standard native types.

template class Image1<int>;
template class Image1<float>;
template class Image1<double>;
template class Image1<PixelRGBA8>;
template class Image1<PixelBGRA8>;

template class Image2<int>;
template class Image2<float>;
template class Image2<double>;
template class Image2<PixelRGBA8>;
template class Image2<PixelBGRA8>;

template class Image3<int>;
template class Image3<float>;
template class Image3<double>;
template class Image3<PixelRGBA8>;
template class Image3<PixelBGRA8>;
