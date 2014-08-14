// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename PixelType>
Image2<PixelType>::~Image2 ()
{
    DeallocatePointers();
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image2<PixelType>::Image2 ()
    :
    Image(typeid(PixelType).name(), (int)sizeof(PixelType), 2, 0, 0),
    mPixels(nullptr)
{
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image2<PixelType>::Image2 (Image2 const& image)
    :
    Image(image),
    mPixels(nullptr)
{
    AllocatePointers();
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image2<PixelType>::Image2 (int dimension0, int dimension1)
    :
    Image(typeid(PixelType).name(), (int)sizeof(PixelType), 2, dimension0,
        dimension1),
    mPixels(nullptr)
{
    AllocatePointers();
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image2<PixelType>& Image2<PixelType>::operator= (Image2 const& image)
{
    bool compatible = Copy(image);
    if (!compatible)
    {
        AllocatePointers();
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename PixelType>
void Image2<PixelType>::SetRawPixels (char* rawPixels)
{
    Image::SetRawPixels(rawPixels);
    AllocatePointers();
}
//----------------------------------------------------------------------------
template <typename PixelType> inline
int Image2<PixelType>::GetIndex (int x, int y) const
{
#if defined(GTE_IMAGICS_ASSERT_ON_INVALID_INDEX)
    if (0 <= x && x < mDimensions[0] && 0 <= y && y < mDimensions[1])
    {
        return x + mDimensions[0]*y;
    }
    LogError("Invalid coordinates (" + std::to_string(x) + "," +
        std::to_string(y) + ").");
    return 0;
#else
    return x + mDimensions[0]*y;
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType> inline
int Image2<PixelType>::GetIndex (std::array<int,2> const& coord) const
{
#if defined(GTE_IMAGICS_ASSERT_ON_INVALID_INDEX)
    if (0 <= coord[0] && coord[0] < mDimensions[0]
    &&  0 <= coord[1] && coord[1] < mDimensions[1])
    {
        return coord[0] + mDimensions[0]*coord[1];
    }
    LogError("Invalid coordinates (" + std::to_string(coord[0]) + "," +
        std::to_string(coord[1]) + ").");
    return 0;
#else
    return coord[0] + mDimensions[0]*coord[1];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType> inline
void Image2<PixelType>::GetCoordinates (int index, int& x, int& y) const
{
#if defined(GTE_IMAGICS_ASSERT_ON_INVALID_INDEX)
    if (0 <= index && index < mNumPixels)
    {
        x = index % mDimensions[0];
        y = index / mDimensions[0];
    }
    else
    {
        LogError("Invalid index " + std::to_string(index) + ".");
        x = 0;
        y = 0;
    }
#else
    x = index % mDimensions[0];
    y = index / mDimensions[0];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType> inline
std::array<int,2> Image2<PixelType>::GetCoordinates (int index) const
{
    std::array<int,2> coord;
#if defined(GTE_IMAGICS_ASSERT_ON_INVALID_INDEX)
    if (0 <= index && index < mNumPixels)
    {
        coord[0] = index % mDimensions[0];
        coord[1] = index / mDimensions[0];
    }
    else
    {
        LogError("Invalid index " + std::to_string(index) + ".");
        coord[0] = 0;
        coord[1] = 0;
    }
#else
    coord[0] = index % mDimensions[0];
    coord[1] = index / mDimensions[0];
#endif
    return coord;
}
//----------------------------------------------------------------------------
template <typename PixelType> inline
PixelType* Image2<PixelType>::GetPixels1D () const
{
    if (mPixels)
    {
        return mPixels[0];
    }
    LogError("Pixels do not exist.");
    return 0;
}
//----------------------------------------------------------------------------
template <typename PixelType> inline
PixelType& Image2<PixelType>::operator[] (int i)
{
#if defined(GTE_IMAGICS_ASSERT_ON_INVALID_INDEX)
    if (mPixels != 0 && 0 <= i && i < mNumPixels)
    {
        return mPixels[0][i];
    }
    LogError("No pixels or invalid index " + std::to_string(i) + ".");
    return mInvalidPixel;
#else
    return mPixels[0][i];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType> inline
PixelType const& Image2<PixelType>::operator[] (int i) const
{
#if defined(GTE_IMAGICS_ASSERT_ON_INVALID_INDEX)
    if (mPixels != 0 && 0 <= i && i < mNumPixels)
    {
        return mPixels[0][i];
    }
    LogError("No pixels or invalid index " + std::to_string(i) + ".");
    return mInvalidPixel;
#else
    return mPixels[0][i];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
PixelType& Image2<PixelType>::Get (int i)
{
    if (mPixels)
    {
        if (i < 0)
        {
            i = 0;
        }
        else if (i >= mNumPixels)
        {
            i = mNumPixels - 1;
        }
        return mPixels[0][i];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
PixelType const& Image2<PixelType>::Get (int i) const
{
    if (mPixels)
    {
        if (i < 0)
        {
            i = 0;
        }
        else if (i >= mNumPixels)
        {
            i = mNumPixels - 1;
        }
        return mPixels[0][i];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType> inline
PixelType** Image2<PixelType>::GetPixels2D () const
{
    LogAssert(mPixels, "Pixels do not exist.");
    return mPixels;
}
//----------------------------------------------------------------------------
template <typename PixelType> inline
PixelType& Image2<PixelType>::operator() (int x, int y)
{
#if defined(GTE_IMAGICS_ASSERT_ON_INVALID_INDEX)
    if (mPixels != 0
    &&  0 <= x && x < mDimensions[0]
    &&  0 <= y && y < mDimensions[1])
    {
        return mPixels[y][x];
    }
    LogError("Invalid coordinates (" + std::to_string(x) + "," +
        std::to_string(y) + ").");
    return mInvalidPixel;
#else
    return mPixels[y][x];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType> inline
PixelType const& Image2<PixelType>::operator() (int x, int y) const
{
#if defined(GTE_IMAGICS_ASSERT_ON_INVALID_INDEX)
    if (mPixels != 0
    &&  0 <= x && x < mDimensions[0]
    &&  0 <= y && y < mDimensions[1])
    {
        return mPixels[y][x];
    }
    LogError("Invalid coordinates (" + std::to_string(x) + "," +
        std::to_string(y) + ").");
    return mInvalidPixel;
#else
    return mPixels[y][x];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType> inline
PixelType& Image2<PixelType>::operator() (std::array<int,2> const& coord)
{
#if defined(GTE_IMAGICS_ASSERT_ON_INVALID_INDEX)
    if (mPixels != 0
    &&  0 <= coord[0] && coord[0] < mDimensions[0]
    &&  0 <= coord[1] && coord[1] < mDimensions[1])
    {
        return mPixels[coord[1]][coord[0]];
    }
    LogError("Invalid coordinates (" + std::to_string(coord[0]) + "," +
        std::to_string(coord[1]) + ").");
    return mInvalidPixel;
#else
    return mPixels[coord[1]][coord[0]];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType> inline
PixelType const& Image2<PixelType>::operator() (
    std::array<int,2> const& coord) const
{
#if defined(GTE_IMAGICS_ASSERT_ON_INVALID_INDEX)
    if (mPixels != 0
    &&  0 <= coord[0] && coord[0] < mDimensions[0]
    &&  0 <= coord[1] && coord[1] < mDimensions[1])
    {
        return mPixels[coord[1]][coord[0]];
    }
    LogError("Invalid coordinates (" + std::to_string(coord[0]) + "," +
        std::to_string(coord[1]) + ").");
    return mInvalidPixel;
#else
    return mPixels[coord[1]][coord[0]];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
PixelType& Image2<PixelType>::Get (int x, int y)
{
    if (mPixels)
    {
        // Clamp to valid (x,y).
        if (x < 0)
        {
            x = 0;
        }
        else if (x >= mDimensions[0])
        {
            x = mDimensions[0] - 1;
        }

        if (y < 0)
        {
            y = 0;
        }
        else if (y >= mDimensions[1])
        {
            y = mDimensions[1] - 1;
        }

        return mPixels[y][x];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
PixelType const& Image2<PixelType>::Get (int x, int y) const
{
    if (mPixels)
    {
        // Clamp to valid (x,y).
        if (x < 0)
        {
            x = 0;
        }
        else if (x >= mDimensions[0])
        {
            x = mDimensions[0] - 1;
        }

        if (y < 0)
        {
            y = 0;
        }
        else if (y >= mDimensions[1])
        {
            y = mDimensions[1] - 1;
        }

        return mPixels[y][x];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
PixelType& Image2<PixelType>::Get (std::array<int,2> coord)
{
    if (mPixels)
    {
        // Clamp to valid (x,y).
        for (int i = 0; i < 2; ++i)
        {
            if (coord[i] < 0)
            {
                coord[i] = 0;
            }
            else if (coord[i] >= mDimensions[i])
            {
                coord[i] = mDimensions[i] - 1;
            }
        }

        return mPixels[coord[1]][coord[0]];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
PixelType const& Image2<PixelType>::Get (std::array<int,2> coord) const
{
    if (mPixels)
    {
        // Clamp to valid (x,y).
        for (int i = 0; i < 2; ++i)
        {
            if (coord[i] < 0)
            {
                coord[i] = 0;
            }
            else if (coord[i] >= mDimensions[i])
            {
                coord[i] = mDimensions[i] - 1;
            }
        }

        return mPixels[coord[1]][coord[0]];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
bool Image2<PixelType>::Resize (int dimension0, int dimension1)
{
    if (Image::Resize(2, dimension0, dimension1))
    {
        AllocatePointers();
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename PixelType>
void Image2<PixelType>::SetAllPixels (PixelType const& value)
{
    if (mPixels)
    {
        for (int i = 0; i < mNumPixels; ++i)
        {
            mPixels[0][i] = value;
        }
    }
}
//----------------------------------------------------------------------------
template <typename PixelType>
bool Image2<PixelType>::Load (std::string const& filename)
{
    std::vector<int> numDimensions(1);
    numDimensions[0] = mNumDimensions;

    std::vector<std::string> pixelTypes(1);
    pixelTypes[0] = mPixelType;

    if (Image::Load(filename, &numDimensions, &pixelTypes))
    {
        AllocatePointers();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename PixelType>
void Image2<PixelType>::AllocatePointers ()
{
    if (mPixels)
    {
        delete[] mPixels;
    }

    mPixels = new PixelType*[mDimensions[1]];
    mPixels[0] = reinterpret_cast<PixelType*>(mRawPixels);
    for (int i1 = 1; i1 < mDimensions[1]; ++i1)
    {
        int j0 = mDimensions[0]*i1;  // = mDimensions[0]*(i1 + j1), j1 = 0
        mPixels[i1] = &mPixels[0][j0];
    }
}
//----------------------------------------------------------------------------
template <typename PixelType>
void Image2<PixelType>::DeallocatePointers ()
{
    delete[] mPixels;
    mPixels = nullptr;
}
//----------------------------------------------------------------------------
