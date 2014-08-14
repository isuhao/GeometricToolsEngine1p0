// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GteImage.h"
#include "GteLogger.h"
#include <array>

namespace gte
{

// The PixelType must be "plain old data" (POD) and cannot have side effects
// from construction or destruction.

template <typename PixelType>
class Image3 : public Image
{
public:
    // The default constructor creates a null image.  You can resize the image
    // later with an explicit Resize call, an assignment, or by loading the
    // image from disk.
    virtual ~Image3 ();
    Image3 ();

    // Copy the input image using the assignment operator.
    Image3 (Image3 const& image);

    // The input dimensions must be positive; otherwise, a null image is
    // created.
    Image3 (int dimension0, int dimension1, int dimension2);

    // If the input image is compatible with 'this', a copy of the input
    // image data occurs.  If the input image is not compatible, 'this' is
    // recreated to be a copy of 'image'.
    Image3& operator= (Image3 const& image);

    // The input array must have the correct number of pixels as determined by
    // the image parameters.  Use at your own risk, because we cannot verify
    // the compatibility.
    virtual void SetRawPixels (char* rawPixels);

    // Conversion between 1-dimensional indices and 2-dimensional coordinates.
    inline int GetIndex (int x, int y, int z) const;
    inline int GetIndex (std::array<int,3> const& coord) const;
    inline void GetCoordinates (int index, int& x, int& y, int& z) const;
    inline std::array<int,3> GetCoordinates (int index) const;

    // Access the data as a 1-dimensional array.  The operator[] functions
    // test for valid i in debug configurations and assert on invalid i.  The
    // Get() functions test for valid i and clamp when invalid (debug and
    // release); these functions cannot fail.
    inline PixelType* GetPixels1D () const;
    inline PixelType& operator[] (int i);
    inline PixelType const& operator[] (int i) const;
    PixelType& Get (int i);
    PixelType const& Get (int i) const;

    // Access the data as a 3-dimensional array.  Pixel (x,y,z) is accessed
    // as "pixels3D[z][y][x]".  The operator() functions test for valid
    // (x,y,z) in debug configurations and assert on invalid (x,y,z).  The
    // Get() functions test for valid (x,y,z) and clamp when invalid (debug
    // and release); these functions cannot fail.
    inline PixelType*** GetPixels3D () const;
    inline PixelType& operator() (int x, int y, int z);
    inline PixelType const& operator() (int x, int y, int z) const;
    inline PixelType& operator() (std::array<int,3> const& coord);
    inline PixelType const& operator() (std::array<int,3> const& coord) const;
    inline PixelType& Get (int x, int y, int z);
    inline PixelType const& Get (int x, int y, int z) const;
    inline PixelType& Get (std::array<int,3> coord);
    inline PixelType const& Get (std::array<int,3> coord) const;

    // Resize an image.  All data is lost from the original image.  The
    // function is convenient for taking a default-constructed image and
    // setting its dimension once it is known.  This avoids an irrelevant
    // memory copy that occurs if instead you were to use the statement
    // image = Image1<PixelType>(dimension0, dimension1, dimension2).  The
    // return value is 'true' whenever the image is resized (reallocations
    // occurred).
    bool Resize (int dimension0, int dimension1, int dimension2);

    // Set all pixels to the specified value.
    void SetAllPixels (PixelType const& value);

    // The required dimensions and pixel type are that of the current image
    // object.
    bool Load (std::string const& filename);

private:
    void AllocatePointers ();
    void DeallocatePointers ();

    // Typed pointers to Image::mRawPixels.
    PixelType*** mPixels;

    // Uninitialized, used in the Get(int) calls.
    PixelType mInvalidPixel;
};

#include "GteImage3.inl"

}
