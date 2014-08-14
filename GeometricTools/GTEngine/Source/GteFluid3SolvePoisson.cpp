// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "GTEnginePCH.h"
#include "GteFluid3SolvePoisson.h"
#include "GteShaderFactory.h"
using namespace gte;

//----------------------------------------------------------------------------
Fluid3SolvePoisson::~Fluid3SolvePoisson()
{
}
//----------------------------------------------------------------------------
Fluid3SolvePoisson::Fluid3SolvePoisson(int xSize, int ySize, int zSize,
    int numXThreads, int numYThreads, int numZThreads,
    std::shared_ptr<ConstantBuffer> const& parameters, int numIterations)
    :
    mNumXGroups(xSize/numXThreads),
    mNumYGroups(ySize/numYThreads),
    mNumZGroups(zSize/numZThreads),
    mNumIterations(numIterations)
{
    HLSLDefiner definer;
    definer.SetInt("NUM_X_THREADS", numXThreads);
    definer.SetInt("NUM_Y_THREADS", numYThreads);
    definer.SetInt("NUM_Z_THREADS", numZThreads);

    // For zeroing mPoisson0 on the GPU.
    mZeroPoisson.reset(ShaderFactory::CreateCompute("ZeroPoisson",
        msHLSLZeroPoissonString, definer));
    LogAssert(mZeroPoisson != nullptr, "Cannot create compute shader.");

    // Create the shader for generating velocity from vortices.
    mSolvePoisson.reset(ShaderFactory::CreateCompute("SolvePoisson",
        msHLSLSolvePoissonString, definer));
    LogAssert(mSolvePoisson != nullptr, "Cannot create compute shader.");

    mPoisson0.reset(new Texture3(DF_R32_FLOAT, xSize, ySize, zSize));
    mPoisson0->SetUsage(Resource::SHADER_OUTPUT);

    mPoisson1.reset(new Texture3(DF_R32_FLOAT, xSize, ySize, zSize));
    mPoisson1->SetUsage(Resource::SHADER_OUTPUT);

    mZeroPoisson->Set("poisson", mPoisson0);
    mSolvePoisson->Set("Parameters", parameters);

    definer.Clear();
    definer.SetInt("USE_ZERO_X_FACE", 1);
    definer.SetInt("NUM_Y_THREADS", numYThreads);
    definer.SetInt("NUM_Z_THREADS", numZThreads);
    mWriteXFace.reset(ShaderFactory::CreateCompute("WriteXFace",
        msHLSLEnforcePoissonBoundaryString, definer, "WriteXFace"));

    definer.Clear();
    definer.SetInt("USE_ZERO_Y_FACE", 1);
    definer.SetInt("NUM_X_THREADS", numXThreads);
    definer.SetInt("NUM_Z_THREADS", numZThreads);
    mWriteYFace.reset(ShaderFactory::CreateCompute("WriteYFace",
        msHLSLEnforcePoissonBoundaryString, definer, "WriteYFace"));

    definer.Clear();
    definer.SetInt("USE_ZERO_Z_FACE", 1);
    definer.SetInt("NUM_X_THREADS", numXThreads);
    definer.SetInt("NUM_Y_THREADS", numYThreads);
    mWriteZFace.reset(ShaderFactory::CreateCompute("WriteZFace",
        msHLSLEnforcePoissonBoundaryString, definer, "WriteZFace"));
}
//----------------------------------------------------------------------------
std::shared_ptr<Texture3> const& Fluid3SolvePoisson::GetPoisson() const
{
    return mPoisson0;
}
//----------------------------------------------------------------------------
void Fluid3SolvePoisson::Execute(DX11Engine* engine,
    std::shared_ptr<Texture3> const& divergence)
{
    mSolvePoisson->Set("divergence", divergence);
    engine->Execute(mZeroPoisson, mNumXGroups, mNumYGroups, mNumZGroups);
    for (int i = 0; i < mNumIterations; ++i)
    {
        // Take one step of the Poisson solver.
        mSolvePoisson->Set("poisson", mPoisson0);
        mSolvePoisson->Set("outPoisson", mPoisson1);
        engine->Execute(mSolvePoisson, mNumXGroups, mNumYGroups, mNumZGroups);

        // Set the boundary to zero.
        mWriteXFace->Set("image", mPoisson1);
        engine->Execute(mWriteXFace, 1, mNumYGroups, mNumZGroups);
        mWriteYFace->Set("image", mPoisson1);
        engine->Execute(mWriteYFace, mNumXGroups, 1, mNumZGroups);
        mWriteZFace->Set("image", mPoisson1);
        engine->Execute(mWriteZFace, mNumXGroups, mNumYGroups, 1);

        std::swap(mPoisson0, mPoisson1);
    }
}
//----------------------------------------------------------------------------

std::string const Fluid3SolvePoisson::msHLSLZeroPoissonString =
GTE_MAKE_HLSL_STRING(
RWTexture3D<float> poisson;

[numthreads(NUM_X_THREADS, NUM_Y_THREADS, NUM_Z_THREADS)]
void CSMain(uint3 c : SV_DispatchThreadID)
{
    poisson[c.xyz] = 0.0f;
}
);

std::string const Fluid3SolvePoisson::msHLSLSolvePoissonString =
GTE_MAKE_HLSL_STRING(
cbuffer Parameters
{
    float4 spaceDelta;    // (dx, dy, dz, 0)
    float4 halfDivDelta;  // (0.5/dx, 0.5/dy, 0.5/dz, 0)
    float4 timeDelta;     // (dt/dx, dt/dy, dt/dz, dt)
    float4 viscosityX;    // (velVX, velVX, velVX, denVX)
    float4 viscosityY;    // (velVX, velVY, velVY, denVY)
    float4 viscosityZ;    // (velVZ, velVZ, velVZ, denVZ)
    float4 epsilon;       // (epsilonX, epsilonY, epsilonZ, epsilon0)
};

Texture3D<float> divergence;
Texture3D<float> poisson;
RWTexture3D<float> outPoisson;

[numthreads(NUM_X_THREADS, NUM_Y_THREADS, NUM_Z_THREADS)]
void CSMain(uint3 c : SV_DispatchThreadID)
{
    uint3 dim;
    divergence.GetDimensions(dim.x, dim.y, dim.z);

    int x = int(c.x);
    int y = int(c.y);
    int z = int(c.z);
    int xm = max(x - 1, 0);
    int xp = min(x + 1, dim.x - 1);
    int ym = max(y - 1, 0);
    int yp = min(y + 1, dim.y - 1);
    int zm = max(z - 1, 0);
    int zp = min(z + 1, dim.z - 1);

    // Sample the divergence at (x,y,z).
    float div = divergence[int3(x, y, z)];

    // Sample Poisson values at (x,y) and immediate neighbors.
    float poisPZZ = poisson[int3(xp, y, z)];
    float poisMZZ = poisson[int3(xm, y, z)];
    float poisZPZ = poisson[int3(x, yp, z)];
    float poisZMZ = poisson[int3(x, ym, z)];
    float poisZZP = poisson[int3(x, y, zp)];
    float poisZZM = poisson[int3(x, y, zm)];

    float4 temp = float4(
        poisPZZ + poisMZZ, poisZPZ + poisZMZ, poisZZP + poisZZM, div);
    outPoisson[c.xyz] = dot(epsilon, temp);
}
);

std::string const Fluid3SolvePoisson::msHLSLEnforcePoissonBoundaryString =
GTE_MAKE_HLSL_STRING(
#if USE_ZERO_X_FACE
RWTexture3D<float> image;

[numthreads(1, NUM_Y_THREADS, NUM_Z_THREADS)]
void WriteXFace(uint3 c : SV_DispatchThreadID)
{
    uint3 dim;
    image.GetDimensions(dim.x, dim.y, dim.z);
    image[uint3(0, c.y, c.z)] = 0.0f;
    image[uint3(dim.x - 1, c.y, c.z)] = 0.0f;
}
#endif

#if USE_ZERO_Y_FACE
RWTexture3D<float> image;

[numthreads(NUM_X_THREADS, 1, NUM_Z_THREADS)]
void WriteYFace(uint3 c : SV_DispatchThreadID)
{
    uint3 dim;
    image.GetDimensions(dim.x, dim.y, dim.z);
    image[uint3(c.x, 0, c.z)] = 0.0f;
    image[uint3(c.x, dim.y - 1, c.z)] = 0.0f;
}
#endif

#if USE_ZERO_Z_FACE
RWTexture3D<float> image;

[numthreads(NUM_X_THREADS, NUM_Y_THREADS, 1)]
void WriteZFace(uint3 c : SV_DispatchThreadID)
{
    uint3 dim;
    image.GetDimensions(dim.x, dim.y, dim.z);
    image[uint3(c.x, c.y, 0)] = 0.0f;
    image[uint3(c.x, c.y, dim.z - 1)] = 0.0f;
}
#endif
);
