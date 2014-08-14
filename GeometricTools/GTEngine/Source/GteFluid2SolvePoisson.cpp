// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "GTEnginePCH.h"
#include "GteFluid2SolvePoisson.h"
#include "GteShaderFactory.h"
using namespace gte;

//----------------------------------------------------------------------------
Fluid2SolvePoisson::~Fluid2SolvePoisson()
{
}
//----------------------------------------------------------------------------
Fluid2SolvePoisson::Fluid2SolvePoisson(int xSize, int ySize, int numXThreads,
    int numYThreads, std::shared_ptr<ConstantBuffer> const& parameters,
    int numIterations)
    :
    mNumXGroups(xSize/numXThreads),
    mNumYGroups(ySize/numYThreads),
    mNumIterations(numIterations)
{
    HLSLDefiner definer;
    definer.SetInt("NUM_X_THREADS", numXThreads);
    definer.SetInt("NUM_Y_THREADS", numYThreads);

    // For zeroing mPoisson0 on the GPU.
    mZeroPoisson.reset(ShaderFactory::CreateCompute("ZeroPoisson",
        msHLSLZeroPoissonString, definer));
    LogAssert(mZeroPoisson != nullptr, "Cannot create compute shader.");

    // Create the shader for generating velocity from vortices.
    mSolvePoisson.reset(ShaderFactory::CreateCompute("SolvePoisson",
        msHLSLSolvePoissonString, definer));
    LogAssert(mSolvePoisson != nullptr, "Cannot create compute shader.");

    mPoisson0.reset(new Texture2(DF_R32_FLOAT, xSize, ySize));
    mPoisson0->SetUsage(Resource::SHADER_OUTPUT);

    mPoisson1.reset(new Texture2(DF_R32_FLOAT, xSize, ySize));
    mPoisson1->SetUsage(Resource::SHADER_OUTPUT);

    mZeroPoisson->Set("poisson", mPoisson0);
    mSolvePoisson->Set("Parameters", parameters);

    definer.Clear();
    definer.SetInt("USE_ZERO_X_EDGE", 1);
    definer.SetInt("NUM_Y_THREADS", numYThreads);
    mWriteXEdge.reset(ShaderFactory::CreateCompute("WriteXEdge",
        msHLSLEnforcePoissonBoundaryString, definer, "WriteXEdge"));

    definer.Clear();
    definer.SetInt("USE_ZERO_Y_EDGE", 1);
    definer.SetInt("NUM_X_THREADS", numXThreads);
    mWriteYEdge.reset(ShaderFactory::CreateCompute("WriteYEdge",
        msHLSLEnforcePoissonBoundaryString, definer, "WriteYEdge"));
}
//----------------------------------------------------------------------------
std::shared_ptr<Texture2> const& Fluid2SolvePoisson::GetPoisson() const
{
    return mPoisson0;
}
//----------------------------------------------------------------------------
void Fluid2SolvePoisson::Execute(DX11Engine* engine,
    std::shared_ptr<Texture2> const& divergence)
{
    mSolvePoisson->Set("divergence", divergence);
    engine->Execute(mZeroPoisson, mNumXGroups, mNumYGroups, 1);
    for (int i = 0; i < mNumIterations; ++i)
    {
        // Take one step of the Poisson solver.
        mSolvePoisson->Set("poisson", mPoisson0);
        mSolvePoisson->Set("outPoisson", mPoisson1);
        engine->Execute(mSolvePoisson, mNumXGroups, mNumYGroups, 1);

        // Set the boundary to zero.
        mWriteXEdge->Set("image", mPoisson1);
        engine->Execute(mWriteXEdge, 1, mNumYGroups, 1);
        mWriteYEdge->Set("image", mPoisson1);
        engine->Execute(mWriteYEdge, mNumXGroups, 1, 1);

        std::swap(mPoisson0, mPoisson1);
    }
}
//----------------------------------------------------------------------------

std::string const Fluid2SolvePoisson::msHLSLZeroPoissonString =
GTE_MAKE_HLSL_STRING(
RWTexture2D<float> poisson;

[numthreads(NUM_X_THREADS, NUM_Y_THREADS, 1)]
void CSMain(uint2 c : SV_DispatchThreadID)
{
    poisson[c] = 0.0f;
}
);

std::string const Fluid2SolvePoisson::msHLSLSolvePoissonString =
GTE_MAKE_HLSL_STRING(
cbuffer Parameters
{
    float4 spaceDelta;    // (dx, dy, 0, 0)
    float4 halfDivDelta;  // (0.5/dx, 0.5/dy, 0, 0)
    float4 timeDelta;     // (dt/dx, dt/dy, 0, dt)
    float4 viscosityX;    // (velVX, velVX, 0, denVX)
    float4 viscosityY;    // (velVX, velVY, 0, denVY)
    float4 epsilon;       // (epsilonX, epsilonY, 0, epsilon0)
};

Texture2D<float> divergence;
Texture2D<float> poisson;
RWTexture2D<float> outPoisson;

[numthreads(NUM_X_THREADS, NUM_Y_THREADS, 1)]
void CSMain(uint2 c : SV_DispatchThreadID)
{
    uint2 dim;
    divergence.GetDimensions(dim.x, dim.y);

    int x = int(c.x);
    int y = int(c.y);
    int xm = max(x - 1, 0);
    int xp = min(x + 1, dim.x - 1);
    int ym = max(y - 1, 0);
    int yp = min(y + 1, dim.y - 1);

    // Sample the divergence at (x,y).
    float div = divergence[int2(x, y)];

    // Sample Poisson values at (x,y), (x+dx,y), (x-dx,y), (x,y+dy), (x,y-dy).
    float poisPZ = poisson[int2(xp, y)];
    float poisMZ = poisson[int2(xm, y)];
    float poisZP = poisson[int2(x, yp)];
    float poisZM = poisson[int2(x, ym)];

    float4 temp = float4(poisPZ + poisMZ, poisZP + poisZM, 0.0f, div);
    outPoisson[c] = dot(epsilon, temp);
}
);

std::string const Fluid2SolvePoisson::msHLSLEnforcePoissonBoundaryString =
GTE_MAKE_HLSL_STRING(
#if USE_ZERO_X_EDGE
RWTexture2D<float> image;

[numthreads(1, NUM_Y_THREADS, 1)]
void WriteXEdge(uint2 c : SV_DispatchThreadID)
{
    uint2 dim;
    image.GetDimensions(dim.x, dim.y);
    image[uint2(0, c.y)] = 0.0f;
    image[uint2(dim.x - 1, c.y)] = 0.0f;
}
#endif

#if USE_ZERO_Y_EDGE
RWTexture2D<float> image;

[numthreads(NUM_X_THREADS, 1, 1)]
void WriteYEdge(uint2 c : SV_DispatchThreadID)
{
    uint2 dim;
    image.GetDimensions(dim.x, dim.y);
    image[uint2(c.x, 0)] = 0.0f;
    image[uint2(c.x, dim.y - 1)] = 0.0f;
}
#endif
);
