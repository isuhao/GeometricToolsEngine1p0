// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "GTEnginePCH.h"
#include "GteDX11GraphicsObject.h"
using namespace gte;

//----------------------------------------------------------------------------
DX11GraphicsObject::~DX11GraphicsObject()
{
    if (mGTObject && mGTObject->IsDrawingState())
    {
        // Sampler, blend, depth-stencil, and rasterizer states have only a
        // finite number of possibilities in DX11.  If you create a state
        // whose settings duplicate one already in existence, DX11 gives you
        // a pointer to the existing one, incrementing the reference count
        // internally.  GTE does not track the duplicates, so we cannot
        // assert that the reference count is zero.
        if (mDXObject)
        {
            mDXObject->Release();
        }
    }
    else
    {
        FinalRelease(mDXObject);
    }
}
//----------------------------------------------------------------------------
DX11GraphicsObject::DX11GraphicsObject(GraphicsObject const* gtObject)
    :
    mGTObject(const_cast<GraphicsObject*>(gtObject)), // Conceptual constness.
    mDXObject(nullptr)
{
}
//----------------------------------------------------------------------------
GraphicsObject* DX11GraphicsObject::GetGraphicsObject() const
{
    return mGTObject;
}
//----------------------------------------------------------------------------
ID3D11DeviceChild* DX11GraphicsObject::GetDXDeviceChild() const
{
    return mDXObject;
}
//----------------------------------------------------------------------------
void DX11GraphicsObject::SetName(std::string const& name)
{
    mName = name;
    HRESULT hr = SetPrivateName(mDXObject, mName);
    CHECK_HR_RETURN_NONE("Failed to set private name");
}
//----------------------------------------------------------------------------
std::string const& DX11GraphicsObject::GetName() const
{
    return mName;
}
//----------------------------------------------------------------------------
