// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
inline void LightDirectionPerVertexEffect::SetPVWMatrix(
    Matrix4x4<float> const& pvwMatrix)
{
    *mPVWMatrix = pvwMatrix;
}
//----------------------------------------------------------------------------
inline Matrix4x4<float> const& LightDirectionPerVertexEffect::GetPVWMatrix()
    const
{
    return *mPVWMatrix;
}
//----------------------------------------------------------------------------
inline void LightDirectionPerVertexEffect::SetLighting(
    Lighting const& lighting)
{
    *mLighting = lighting;
}
//----------------------------------------------------------------------------
inline LightDirectionPerVertexEffect::Lighting const&
LightDirectionPerVertexEffect::GetLighting() const
{
    return *mLighting;
}
//----------------------------------------------------------------------------
inline std::shared_ptr<ConstantBuffer> const&
LightDirectionPerVertexEffect::GetPVWMatrixConstant() const
{
    return mPVWMatrixConstant;
}
//----------------------------------------------------------------------------
inline std::shared_ptr<ConstantBuffer> const&
LightDirectionPerVertexEffect::GetLightingConstant() const
{
    return mLightingConstant;
}
//----------------------------------------------------------------------------
