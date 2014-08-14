// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "GTEnginePCH.h"
#include "GteLightPointPerVertexEffect.h"
#include "GteShaderFactory.h"
using namespace gte;

//----------------------------------------------------------------------------
LightPointPerVertexEffect::LightPointPerVertexEffect(Lighting const& lighting)
{
    // Create the shader constants.
    mPVWMatrixConstant.reset(new ConstantBuffer(sizeof(Matrix4x4<float>),
        true));
    mPVWMatrix = mPVWMatrixConstant->Get<Matrix4x4<float>>();
    *mPVWMatrix = Matrix4x4<float>::Identity();

    mLightingConstant.reset(new ConstantBuffer(sizeof(Lighting), true));
    mLighting = mLightingConstant->Get<Lighting>();
    *mLighting = lighting;

    // Create the light ambient effect shaders.
    mVShader.reset(ShaderFactory::CreateVertex("VSMain", msHLSLString));
    if (mVShader)
    {
        mVShader->Set("PVWMatrix", mPVWMatrixConstant);
        mVShader->Set("Lighting", mLightingConstant);
        mPShader.reset(ShaderFactory::CreatePixel("PSMain", msHLSLString));
        if (!mPShader)
        {
            LogError("Unable to create pixel shader.");
        }
    }
    else
    {
        LogError("Unable to create vertex shader.");
    }
}
//----------------------------------------------------------------------------
void LightPointPerVertexEffect::SetName(std::string const& name)
{
    VisualEffect::SetName(name);
    mPVWMatrixConstant->SetName(name);
    mLightingConstant->SetName(name);
}
//----------------------------------------------------------------------------

std::string const LightPointPerVertexEffect::msHLSLString =
GTE_MAKE_HLSL_STRING(
cbuffer PVWMatrix
{
    float4x4 pvwMatrix;
};

cbuffer Lighting
{
    float4x4 wMatrix;
    float4 cameraModelPosition;
    float4 materialEmissive;
    float4 materialAmbient;
    float4 materialDiffuse;
    float4 materialSpecular;
    float4 lightModelPosition;
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
    float4 lightAttenuation;
};

struct VS_INPUT
{
    float3 modelPosition : POSITION;
    float3 modelNormal : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 vertexColor : COLOR0;
    float4 clipPosition : SV_POSITION;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;

    float3 modelLightDiff = input.modelPosition - lightModelPosition.xyz;
    float3 vertexDirection = normalize(modelLightDiff);
    float NDotL = -dot(input.modelNormal, vertexDirection);
    float3 viewVector = normalize(cameraModelPosition.xyz - input.modelPosition);
    float3 halfVector = normalize(viewVector - vertexDirection);
    float NDotH = dot(input.modelNormal, halfVector);
    float4 lighting = lit(NDotL, NDotH, materialSpecular.a);

#ifdef GTE_USE_MAT_VEC
    float4 worldLightDiff = mul(wMatrix, float4(modelLightDiff, 0.0f));
#else
    float4 worldLightDiff = mul(float4(modelLightDiff, 0.0f), wMatrix);
#endif
    float distance = length(worldLightDiff.xyz);
    float attenuation = lightAttenuation.w/(lightAttenuation.x + distance *
        (lightAttenuation.y + distance*lightAttenuation.z));

    float3 color = materialAmbient.rgb*lightAmbient.rgb +
        lighting.y*materialDiffuse.rgb*lightDiffuse.rgb +
        lighting.z*materialSpecular.rgb*lightSpecular.rgb;

    output.vertexColor.rgb = materialEmissive.rgb + attenuation*color;
    output.vertexColor.a = materialDiffuse.a;
#ifdef GTE_USE_MAT_VEC
    output.clipPosition = mul(pvwMatrix, float4(input.modelPosition, 1.0f));
#else
    output.clipPosition = mul(float4(input.modelPosition, 1.0f), pvwMatrix);
#endif
    return output;
}

struct PS_INPUT
{
    float4 vertexColor : COLOR0;
};

struct PS_OUTPUT
{
    float4 pixelColor0 : SV_TARGET0;
};

PS_OUTPUT PSMain(PS_INPUT input)
{
    PS_OUTPUT output;
    output.pixelColor0 = input.vertexColor;
    return output;
}
);
