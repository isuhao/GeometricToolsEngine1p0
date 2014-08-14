// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GTEngineDEF.h"
#include "GteFluid3AdjustVelocity.h"
#include "GteFluid3ComputeDivergence.h"
#include "GteFluid3EnforceStateBoundary.h"
#include "GteFluid3InitializeSource.h"
#include "GteFluid3InitializeState.h"
#include "GteFluid3SolvePoisson.h"
#include "GteFluid3UpdateState.h"

namespace gte
{

class GTE_IMPEXP Fluid3
{
public:
    // Construction and destruction.  The (x,y,z) grid covers [0,1]^3.
    ~Fluid3();
    Fluid3(DX11Engine* engine, int xSize, int ySize, int zSize, float dt);

    void Initialize();
    void DoSimulationStep();
    std::shared_ptr<Texture3> const& GetState() const;

private:
    // Constructor inputs.
    DX11Engine* mEngine;
    int mXSize, mYSize, mZSize;
    float mDt;

    // Current simulation time.
    float mTime;

    std::shared_ptr<ConstantBuffer> mParameters;
    std::shared_ptr<Fluid3InitializeSource> mInitializeSource;
    std::shared_ptr<Fluid3InitializeState> mInitializeState;
    std::shared_ptr<Fluid3EnforceStateBoundary> mEnforceStateBoundary;
    std::shared_ptr<Fluid3UpdateState> mUpdateState;
    std::shared_ptr<Fluid3ComputeDivergence> mComputeDivergence;
    std::shared_ptr<Fluid3SolvePoisson> mSolvePoisson;
    std::shared_ptr<Fluid3AdjustVelocity> mAdjustVelocity;

    std::shared_ptr<Texture3> mSourceTexture;
    std::shared_ptr<Texture3> mStateTm1Texture;
    std::shared_ptr<Texture3> mStateTTexture;
    std::shared_ptr<Texture3> mStateTp1Texture;
    std::shared_ptr<Texture3> mDivergenceTexture;
    std::shared_ptr<Texture3> mPoissonTexture;
};

}
