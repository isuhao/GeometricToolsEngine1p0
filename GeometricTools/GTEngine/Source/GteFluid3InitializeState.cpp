// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "GTEnginePCH.h"
#include "GteFluid3InitializeState.h"
#include "GteShaderFactory.h"
using namespace gte;

//----------------------------------------------------------------------------
Fluid3InitializeState::~Fluid3InitializeState()
{
}
//----------------------------------------------------------------------------
Fluid3InitializeState::Fluid3InitializeState(int xSize, int ySize, int zSize,
    int numXThreads, int numYThreads, int numZThreads)
    :
    mNumXGroups(xSize/numXThreads),
    mNumYGroups(ySize/numYThreads),
    mNumZGroups(zSize/numZThreads)
{
    // Use a Mersenne twister engine for random numbers.
    std::mt19937 mte;
    std::uniform_real_distribution<float> unirnd(0.0f, 1.0f);

    // Initial density values are randomly generated.
    mDensity.reset(new Texture3(DF_R32_FLOAT, xSize, ySize, zSize));
    float* data = mDensity->Get<float>();
    for (unsigned int i = 0; i < mDensity->GetNumElements(); ++i, ++data)
    {
        *data = unirnd(mte);
    }

    // Initial velocity values are zero.
    mVelocity.reset(new Texture3(DF_R32G32B32A32_FLOAT, xSize, ySize, zSize));
    mVelocity->SetUsage(Resource::SHADER_OUTPUT);
    memset(mVelocity->GetData(), 0, mVelocity->GetNumBytes());

    mStateTm1.reset(new Texture3(DF_R32G32B32A32_FLOAT, xSize, ySize, zSize));
    mStateTm1->SetUsage(Resource::SHADER_OUTPUT);

    mStateT.reset(new Texture3(DF_R32G32B32A32_FLOAT, xSize, ySize, zSize));
    mStateT->SetUsage(Resource::SHADER_OUTPUT);

    // Create the shader for generating velocity from vortices.
    HLSLDefiner definer;
    definer.SetInt("NUM_X_THREADS", numXThreads);
    definer.SetInt("NUM_Y_THREADS", numYThreads);
    definer.SetInt("NUM_Z_THREADS", numZThreads);
    mInitializeState.reset(ShaderFactory::CreateCompute("InitializeState",
        msHLSLInitializeStateString, definer));
    LogAssert(mInitializeState != nullptr, "Cannot create compute shader.");
    mInitializeState->Set("density", mDensity);
    mInitializeState->Set("velocity", mVelocity);
    mInitializeState->Set("stateTm1", mStateTm1);
    mInitializeState->Set("stateT", mStateT);
}
//----------------------------------------------------------------------------
std::shared_ptr<Texture3> const& Fluid3InitializeState::GetStateTm1() const
{
    return mStateTm1;
}
//----------------------------------------------------------------------------
std::shared_ptr<Texture3> const& Fluid3InitializeState::GetStateT() const
{
    return mStateT;
}
//----------------------------------------------------------------------------
void Fluid3InitializeState::Execute(DX11Engine* engine)
{
    engine->Execute(mInitializeState, mNumXGroups, mNumYGroups, mNumZGroups);
}
//----------------------------------------------------------------------------

std::string const Fluid3InitializeState::msHLSLInitializeStateString =
GTE_MAKE_HLSL_STRING(
Texture3D<float> density;
Texture3D<float4> velocity;
RWTexture3D<float4> stateTm1;
RWTexture3D<float4> stateT;

[numthreads(NUM_X_THREADS, NUM_Y_THREADS, NUM_Z_THREADS)]
void CSMain(uint3 c : SV_DispatchThreadID)
{
    float4 initial = float4(velocity[c.xyz].xyz, density[c.xyz]);
    stateTm1[c.xyz] = initial;
    stateT[c.xyz] = initial;
}
);

