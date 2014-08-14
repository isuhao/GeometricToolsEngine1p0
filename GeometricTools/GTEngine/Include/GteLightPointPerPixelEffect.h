// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GTEngineDEF.h"
#include "GteVisualEffect.h"
#include "GteMatrix4x4.h"

namespace gte
{

class GTE_IMPEXP LightPointPerPixelEffect : public VisualEffect
{
public:
    struct Lighting
    {
        // This structure is organized to match the default HLSL packing.  The
        // '*' components are not used.
        Matrix4x4<float> wMatrix;
        Vector4<float> cameraModelPosition; // (x,y,z,1)
        Vector4<float> materialEmissive;    // (r,g,b,*)
        Vector4<float> materialAmbient;     // (r,g,b,*)
        Vector4<float> materialDiffuse;     // (r,g,b,a)
        Vector4<float> materialSpecular;    // (r,g,b,specularPower)
        Vector4<float> lightModelPosition;  // (x,y,z,1)
        Vector4<float> lightAmbient;        // (r,g,b,*)
        Vector4<float> lightDiffuse;        // (r,g,b,*)
        Vector4<float> lightSpecular;       // (r,g,b,*)
        Vector4<float> lightAttenuation;    // (*,*,*,w)
    };

    // Construction.
    LightPointPerPixelEffect(Lighting const& lighting);

    // Member access.
    inline void SetPVWMatrix(Matrix4x4<float> const& pvwMatrix);
    inline Matrix4x4<float> const& GetPVWMatrix() const;
    inline void SetLighting(Lighting const& lighting);
    inline Lighting const& GetLighting() const;

    // Required to bind and update resources.
    inline std::shared_ptr<ConstantBuffer> const& GetPVWMatrixConstant() const;
    inline std::shared_ptr<ConstantBuffer> const& GetLightingConstant() const;

    // Naming support, used in the DX11 debug layer.  The default name is "".
    // If you want the name to show up in the DX11 destruction messages when
    // the associated DX11GraphicsObject is destroyed, set the name to
    // something other than "".
    virtual void SetName(std::string const& name);

private:
    // Vertex shader parameters.
    std::shared_ptr<ConstantBuffer> mPVWMatrixConstant;
    std::shared_ptr<ConstantBuffer> mLightingConstant;

    // Convenience pointers.
    Matrix4x4<float>* mPVWMatrix;
    Lighting* mLighting;

    static std::string const msHLSLString;
};

#include "GteLightPointPerPixelEffect.inl"

}
