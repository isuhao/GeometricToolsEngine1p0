// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "GTEnginePCH.h"
#include "GteImage.h"
using namespace gte;

//----------------------------------------------------------------------------
Image::~Image()
{
    delete[] mDimensions;
    delete[] mOffsets;
    delete[] mPixelMetaData;

    if (mOwnerRawPixels)
    {
        delete[] mRawPixels;
    }
}
//----------------------------------------------------------------------------
Image::Image()
    :
    mPixelType(""),
    mPixelSize(0),
    mNumDimensions(0),
    mDimensions(nullptr),
    mOffsets(nullptr),
    mNumPixels(0),
    mRawPixels(nullptr),
    mOwnerRawPixels(true),
    mImageMetaData(""),
    mPixelMetaData(nullptr),
    mDefaultPixelMetaData("")
{
}
//----------------------------------------------------------------------------
Image::Image(Image const& image)
    :
    mPixelType(image.mPixelType),
    mPixelSize(image.mPixelSize),
    mNumDimensions(image.mNumDimensions),
    mNumPixels(image.mNumPixels),
    mOwnerRawPixels(true),
    mImageMetaData(image.mImageMetaData),
    mDefaultPixelMetaData(image.mDefaultPixelMetaData)
{
    mDimensions = new int[mNumDimensions];
    mOffsets = new int[mNumDimensions];
    for (int i = 0; i < mNumDimensions; ++i)
    {
        mDimensions[i] = image.mDimensions[i];
        mOffsets[i] = image.mOffsets[i];
    }

    mRawPixels = new char[mNumPixels*mPixelSize];
    memcpy(mRawPixels, image.mRawPixels, mNumPixels*mPixelSize);

    if (image.mPixelMetaData)
    {
        mPixelMetaData = new std::string[mNumPixels];
        for (int i = 0; i < mNumPixels; ++i)
        {
            mPixelMetaData[i] = image.mPixelMetaData[i];
        }
    }
    else
    {
        mPixelMetaData = nullptr;
    }
}
//----------------------------------------------------------------------------
Image::Image(std::string const& pixelType, int pixelSize, int numDimensions,
    ...)
    :
    mPixelType(pixelType),
    mPixelSize(pixelSize),
    mNumDimensions(numDimensions),
    mDimensions(nullptr),
    mOffsets(nullptr),
    mNumPixels(0),
    mRawPixels(nullptr),
    mOwnerRawPixels(true),
    mImageMetaData(""),
    mPixelMetaData(nullptr),
    mDefaultPixelMetaData("")
{
    if (mNumDimensions > 0)
    {
        mDimensions = new int[mNumDimensions];
        mOffsets = new int[mNumDimensions];

        va_list arguments;
        va_start(arguments, numDimensions);
        mNumPixels = 1;
        for (int i = 0; i < mNumDimensions; ++i)
        {
            mDimensions[i] = va_arg(arguments, int);
            mNumPixels *= mDimensions[i];
        }
        va_end(arguments);

        mOffsets[0] = 1;
        for (int i = 1; i < mNumDimensions; ++i)
        {
            mOffsets[i] = mDimensions[i - 1]*mOffsets[i - 1];
        }

        mRawPixels = new char[mNumPixels*mPixelSize];
    }
}
//----------------------------------------------------------------------------
Image& Image::operator=(Image const& image)
{
    Copy(image);
    return *this;
}
//----------------------------------------------------------------------------
int Image::GetDimension(int i) const
{
#if defined(GTE_IMAGICS_ASSERT_ON_INVALID_INDEX)
    if (0 <= i && i < mNumDimensions)
    {
        return mDimensions[i];
    }
    LogError("Invalid index " + std::to_string(i) + ".");
    return 0;
#else
    return mDimensions[i];
#endif
}
//----------------------------------------------------------------------------
int Image::GetOffset(int i) const
{
#if defined(GTE_IMAGICS_ASSERT_ON_INVALID_INDEX)
    if (0 <= i && i < mNumDimensions)
    {
        return mOffsets[i];
    }
    LogError("Invalid index " + std::to_string(i) + ".");
    return 0;
#else
    return mOffsets[i];
#endif
}
//----------------------------------------------------------------------------
void Image::SetRawPixels(char* rawPixels)
{
    if (mOwnerRawPixels)
    {
        delete[] mRawPixels;
    }

    mRawPixels = rawPixels;
    mOwnerRawPixels = false;
}
//----------------------------------------------------------------------------
void Image::CreatePixelMetaData()
{
    if (!mPixelMetaData && mNumPixels > 0)
    {
        mPixelMetaData = new std::string[mNumPixels];
        return;
    }
    LogError("No metadata.");
}
//----------------------------------------------------------------------------
void Image::DestroyPixelMetaData()
{
    if (mPixelMetaData)
    {
        delete[] mPixelMetaData;
        mPixelMetaData = nullptr;
        return;
    }
    LogError("No metadata.");
}
//----------------------------------------------------------------------------
void Image::SetPixelMetaData(int i, std::string const& metadata)
{
#if defined(GTE_IMAGICS_ASSERT_ON_INVALID_INDEX)
    if (mPixelMetaData && 0 <= i && i < mNumPixels)
    {
        mPixelMetaData[i] = metadata;
        return;
    }
    LogError("No metadata or invalid index.");
#else
    mPixelMetaData[i] = metadata;
#endif
}
//----------------------------------------------------------------------------
std::string const& Image::GetPixelMetaData(int i) const
{
#if defined(GTE_IMAGICS_ASSERT_ON_INVALID_INDEX)
    if (mPixelMetaData && 0 <= i && i < mNumPixels)
    {
        return mPixelMetaData[i];
    }
    LogError("No metadata or invalid index.");
    return mDefaultPixelMetaData;
#else
    return mPixelMetaData[i];
#endif
}
//----------------------------------------------------------------------------
void Image::ClearPixelMetaData()
{
    if (mPixelMetaData)
    {
        for (int i = 0; i < mNumPixels; ++i)
        {
            mPixelMetaData[i] = "";
        }
    }
}
//----------------------------------------------------------------------------
int Image::GetIndex(int const* coord) const
{
    // assert:  coord is array of mNumDimensions elements
    int index = coord[0];
    for (int i = 1; i < mNumDimensions; ++i)
    {
        index += mOffsets[i]*coord[i];
    }
    return index;
}
//----------------------------------------------------------------------------
void Image::GetCoordinates(int index, int* coord) const
{
    // assert:  coord is array of mNumDimensions elements
    for (int i = 0; i < mNumDimensions; ++i)
    {
        coord[i] = index % mDimensions[i];
        index /= mDimensions[i];
    }
}
//----------------------------------------------------------------------------
void Image::ClearPixels()
{
    if (mRawPixels)
    {
        memset(mRawPixels, 0, mNumPixels*mPixelSize*sizeof(char));
    }

    if (mPixelMetaData)
    {
        for (int i = 0; i < mNumPixels; ++i)
        {
            mPixelMetaData[i] = "";
        }
    }
}
//----------------------------------------------------------------------------
bool Image::LoadHeader(std::ifstream& input, std::string& pixelType,
    int& pixelSize, int& numDimensions)
{
    // Read the length of the pixel type string.
    int length;
    if (input.read((char*)&length, sizeof(int)).bad())
    {
        LogError("Failed read length.");
        pixelType = "";
        pixelSize = 0;
        numDimensions = 0;
        return false;
    }

    // Read the pixel type string.
    char* temp = new char[length + 1];
    if (input.read(temp, length + 1).bad() || temp[length] != 0)
    {
        LogError("Failed read pixelType.");
        delete[] temp;
        pixelType = "";
        pixelSize = 0;
        numDimensions = 0;
        return false;
    }
    pixelType = std::string(temp);
    delete[] temp;

    // Read the pixel size.
    if (input.read((char*)&pixelSize, sizeof(int)).bad())
    {
        LogError("Failed read pixelSize.");
        pixelType = "";
        pixelSize = 0;
        numDimensions = 0;
        return false;
    }

    // Read the number of dimensions.
    if (input.read((char*)&numDimensions, sizeof(int)).bad())
    {
        LogError("Failed read numDimensions.");
        pixelType = "";
        pixelSize = 0;
        numDimensions = 0;
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool Image::Load(std::string const& name,
    std::vector<int> const* requiredNumDimensions,
    std::vector<std::string> const* requiredPixelTypes)
{
    CreateNullImage();

    std::ifstream input(name, std::ios::in | std::ios::binary);
    if (!input)
    {
        LogError("Failed to open file " + name + " for reading.");
        return false;
    }

    if (!LoadHeader(input, mPixelType, mPixelSize, mNumDimensions))
    {
        input.close();
        return false;
    }

    if (requiredNumDimensions)
    {
        int const numElements = (int)requiredNumDimensions->size();
        int i;
        for (i = 0; i < numElements; ++i)
        {
            if (mNumDimensions == (*requiredNumDimensions)[i])
            {
                break;
            }
        }
        if (i == numElements)
        {
            CreateNullImage();
            input.close();
            return false;
        }
    }

    if (requiredPixelTypes)
    {
        int const numElements = (int)requiredPixelTypes->size();
        int i;
        for (i = 0; i < numElements; ++i)
        {
            if (mPixelType == (*requiredPixelTypes)[i])
            {
                break;
            }
        }
        if (i == numElements)
        {
            CreateNullImage();
            input.close();
            return false;
        }
    }

    // Read the dimensions.
    mDimensions = new int[mNumDimensions];
    if (input.read((char*)mDimensions, mNumDimensions*sizeof(int)).bad())
    {
        LogError("Failed read dimensions.");
        CreateNullImage();
        input.close();
        return false;
    }

    // Read the offsets.
    mOffsets = new int[mNumDimensions];
    if (input.read((char*)mOffsets, mNumDimensions*sizeof(int)).bad())
    {
        LogError("Failed read offsets.");
        CreateNullImage();
        input.close();
        return false;
    }

    // Read the number of pixels.
    if (input.read((char*)&mNumPixels, sizeof(int)).bad())
    {
        LogError("Failed read number of pixels.");
        CreateNullImage();
        input.close();
        return false;
    }

    // Read the pixels.
    int numBytes = mNumPixels*mPixelSize;
    mRawPixels = new char[numBytes];
    if (input.read(mRawPixels, numBytes).bad())
    {
        LogError("Failed read pixels.");
        CreateNullImage();
        input.close();
        return false;
    }

    // Read the length of the image metadata string.
    int length;
    if (input.read((char*)&length, sizeof(int)).bad())
    {
        LogError("Failed read length(image metadata).");
        CreateNullImage();
        input.close();
        return false;
    }

    // Read the image metadata string.
    numBytes = length + 1;
    char* temp = new char[numBytes];
    if (input.read(temp, numBytes).bad() || temp[length] != 0)
    {
        LogError("Failed read image metadata.");
        delete[] temp;
        CreateNullImage();
        input.close();
        return false;
    }
    mImageMetaData = std::string(temp);
    delete[] temp;

    // Read the existence flag for pixel metadata.
    int existsPixelMetaData = 0;
    if (input.read((char*)&existsPixelMetaData, sizeof(int)).bad())
    {
        LogError("Failed read pixel metadata existence.");
        CreateNullImage();
        input.close();
        return false;
    }

    if (existsPixelMetaData)
    {
        CreatePixelMetaData();

        int maxNumBytes = 128;
        temp = new char[maxNumBytes];
        for (int i = 0; i < mNumPixels; ++i)
        {
            // Read the length of the pixel metadata string.
            if (input.read((char*)&length, sizeof(int)).bad())
            {
                LogError("Failed read length.");
                delete[] temp;
                CreateNullImage();
                input.close();
                return false;
            }

            // Resize the temporary storage if necessary.
            numBytes = length + 1;
            if (numBytes > maxNumBytes)
            {
                maxNumBytes = numBytes;
                delete[] temp;
                temp = new char[maxNumBytes];
            }

            // Read the pixel metadata string.
            if (input.read(temp, numBytes).bad() || temp[length] != 0)
            {
                LogError("Failed read pixel metadata.");
                delete[] temp;
                CreateNullImage();
                input.close();
                return false;
            }
            mPixelMetaData[i] = std::string(temp);
        }
    }

    input.close();
    return true;
}
//----------------------------------------------------------------------------
bool Image::Save(std::string const& name) const
{
    if (IsNullImage())
    {
        LogError("Cannot save a null image.");
        return false;
    }

    std::ofstream output(name, std::ios::out | std::ios::binary);
    if (!output)
    {
        LogError("Failed to open file " + name + " for writing.");
        return false;
    }

    // Write the length of the pixel type.
    int length = (int)mPixelType.length();
    if (output.write((char const*)&length, sizeof(int)).bad())
    {
        LogError("Failed write length(pixelType).");
        output.close();
        return false;
    }

    // Write the pixel type string.
    int numBytes = length + 1;
    if (output.write(mPixelType.c_str(), numBytes).bad())
    {
        LogError("Failed write pixelType.");
        output.close();
        return false;
    }

    // Write the pixel size.
    if (output.write((char const*)&mPixelSize, sizeof(int)).bad())
    {
        LogError("Failed write pixelSize.");
        output.close();
        return false;
    }

    // Write the number of dimensions.
    if (output.write((char const*)&mNumDimensions, sizeof(int)).bad())
    {
        LogError("Failed write numDimensions.");
        output.close();
        return false;
    }

    // Write the dimensions.
    numBytes = mNumDimensions*sizeof(int);
    if (output.write((char const*)mDimensions, numBytes).bad())
    {
        LogError("Failed write dimensions.");
        output.close();
        return false;
    }

    // Write the offsets.
    if (output.write((char const*)mOffsets, numBytes).bad())
    {
        LogError("Failed write offsets.");
        output.close();
        return false;
    }

    // Write the number of pixels.
    if (output.write((char const*)&mNumPixels, sizeof(int)).bad())
    {
        LogError("Failed write numPixels.");
        output.close();
        return false;
    }

    // Write the pixels.
    numBytes = mNumPixels*mPixelSize;
    if (output.write(mRawPixels, numBytes).bad())
    {
        LogError("Failed write pixels.");
        output.close();
        return false;
    }

    // Write the length of the image metadata string.
    length = (int)mImageMetaData.length();
    if (output.write((char const*)&length, sizeof(int)).bad())
    {
        LogError("Failed write length(image metadata).");
        output.close();
        return false;
    }

    // Write the image metadata string.
    numBytes = length + 1;
    if (output.write(mImageMetaData.c_str(), numBytes).bad())
    {
        LogError("Failed write image metadata.");
        output.close();
        return false;
    }

    // Write the existence flag for pixel metadata.
    int existsPixelMetaData = (mPixelMetaData ? 1 : 0);
    if (output.write((char const*)&existsPixelMetaData, sizeof(int)).bad())
    {
        LogError("Failed write pixel metadata existence.");
        output.close();
        return false;
    }

    if (mPixelMetaData)
    {
        for (int i = 0; i < mNumPixels; ++i)
        {
            // Write the length of the pixel metadata string.
            std::string const& metadata = mPixelMetaData[i];
            length = (int)metadata.length();
            if (output.write((char const*)&length, sizeof(int)).bad())
            {
                LogError("Failed write length(pixel metadata).");
                output.close();
                return false;
            }

            // Write the pixel metadata string.
            numBytes = length + 1;
            if (output.write(metadata.c_str(), numBytes).bad())
            {
                LogError("Failed write pixel metadata.");
                output.close();
                return false;
            }
        }
    }

    output.close();
    return true;
}
//----------------------------------------------------------------------------
void Image::CreateNullImage()
{
    delete[] mDimensions;
    delete[] mOffsets;
    delete[] mPixelMetaData;
    if (mOwnerRawPixels)
    {
        delete[] mRawPixels;
    }

    mPixelType = "";
    mPixelSize = 0;
    mNumDimensions = 0;
    mDimensions = nullptr;
    mOffsets = nullptr;
    mNumPixels = 0;
    mRawPixels = nullptr;
    mOwnerRawPixels = true;
    mImageMetaData = "";
    mPixelMetaData = nullptr;
    mDefaultPixelMetaData = "";
}
//----------------------------------------------------------------------------
bool Image::Resize(int numDimensions, ...)
{
    if (numDimensions < 0)
    {
        LogError("Invalid number of dimensions.");
        numDimensions = 0;
    }
    else if (numDimensions > 0 && numDimensions == mNumDimensions)
    {
        // Test for compatibility.  If they are, no resizing is necessary.
        va_list arguments;
        va_start(arguments, numDimensions);
        int i;
        for (i = 0; i < mNumDimensions; ++i)
        {
            int inputDimension = va_arg(arguments, int);
            if (mDimensions[i] != inputDimension)
            {
                break;
            }
        }
        va_end(arguments);
        if (i == mNumDimensions)
        {
            // The images are compatible.
            return false;
        }
    }

    std::string savePixelType = mPixelType;
    int savePixelSize = mPixelSize;
    bool existsPixelMetaData = (mPixelMetaData != nullptr);
    CreateNullImage();
    mPixelType = savePixelType;
    mPixelSize = savePixelSize;
    mNumDimensions = numDimensions;

    if (mNumDimensions > 0)
    {
        mDimensions = new int[mNumDimensions];
        mOffsets = new int[mNumDimensions];

        va_list arguments;
        va_start(arguments, numDimensions);
        mNumPixels = 1;
        int i;
        for (i = 0; i < mNumDimensions; ++i)
        {
            mDimensions[i] = va_arg(arguments, int);
            mNumPixels *= mDimensions[i];
        }
        va_end(arguments);

        mOffsets[0] = 1;
        for (i = 1; i < mNumDimensions; ++i)
        {
            mOffsets[i] = mDimensions[i - 1]*mOffsets[i - 1];
        }

        mRawPixels = new char[mNumPixels*mPixelSize];

        if (existsPixelMetaData)
        {
            mPixelMetaData = new std::string[mNumPixels];
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool Image::IsCompatible(Image const& image) const
{
    if (mNumDimensions != image.mNumDimensions
    ||  mPixelType != image.mPixelType)
    {
        return false;
    }

    for (int i = 0; i < mNumDimensions; ++i)
    {
        if (mDimensions[i] != image.mDimensions[i])
        {
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool Image::Copy(Image const& image)
{
    bool compatible = IsCompatible(image);
    if (!compatible)
    {
        CreateNullImage();

        mPixelType = image.mPixelType;
        mPixelSize = image.mPixelSize;
        mNumDimensions = image.mNumDimensions;
        mNumPixels = image.mNumPixels;
        mImageMetaData = image.mImageMetaData;
        mDefaultPixelMetaData = image.mDefaultPixelMetaData;

        if (mNumDimensions > 0)
        {
            mDimensions = new int[mNumDimensions];
            mOffsets = new int[mNumDimensions];
            for (int i = 0; i < mNumDimensions; ++i)
            {
                mDimensions[i] = image.mDimensions[i];
                mOffsets[i] = image.mOffsets[i];
            }

            mRawPixels = new char[mNumPixels*mPixelSize];

            if (image.mPixelMetaData)
            {
                mPixelMetaData = new std::string[mNumPixels];
            }
        }
        else
        {
            mDimensions = nullptr;
            mOffsets = nullptr;
            mRawPixels = nullptr;
            mPixelMetaData = nullptr;
        }
    }

    if (mRawPixels)
    {
        memcpy(mRawPixels, image.mRawPixels, mNumPixels*mPixelSize);
    }

    if (mPixelMetaData && image.mPixelMetaData)
    {
        for (int i = 0; i < mNumPixels; ++i)
        {
            mPixelMetaData[i] = image.mPixelMetaData[i];
        }
    }

    return compatible;
}
//----------------------------------------------------------------------------
