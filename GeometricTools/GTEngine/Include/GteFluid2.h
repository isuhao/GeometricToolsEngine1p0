// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GTEngineDEF.h"
#include "GteFluid2AdjustVelocity.h"
#include "GteFluid2ComputeDivergence.h"
#include "GteFluid2EnforceStateBoundary.h"
#include "GteFluid2InitializeSource.h"
#include "GteFluid2InitializeState.h"
#include "GteFluid2SolvePoisson.h"
#include "GteFluid2UpdateState.h"

namespace gte
{

class GTE_IMPEXP Fluid2
{
public:
    // Construction and destruction.  The (x,y) grid covers [0,1]^2.
    ~Fluid2();
    Fluid2(gte::DX11Engine* engine, int xSize, int ySize, float dt,
        float densityViscosity, float velocityViscosity);

    void Initialize();
    void DoSimulationStep();
    std::shared_ptr<gte::Texture2> const& GetState () const;

private:
    // Constructor inputs.
    gte::DX11Engine* mEngine;
    int mXSize, mYSize;
    float mDt;

    // Current simulation time.
    float mTime;

    std::shared_ptr<gte::ConstantBuffer> mParameters;
    std::shared_ptr<gte::Fluid2InitializeSource> mInitializeSource;
    std::shared_ptr<gte::Fluid2InitializeState> mInitializeState;
    std::shared_ptr<gte::Fluid2EnforceStateBoundary> mEnforceStateBoundary;
    std::shared_ptr<gte::Fluid2UpdateState> mUpdateState;
    std::shared_ptr<gte::Fluid2ComputeDivergence> mComputeDivergence;
    std::shared_ptr<gte::Fluid2SolvePoisson> mSolvePoisson;
    std::shared_ptr<gte::Fluid2AdjustVelocity> mAdjustVelocity;

    std::shared_ptr<gte::Texture2> mSourceTexture;
    std::shared_ptr<gte::Texture2> mStateTm1Texture;
    std::shared_ptr<gte::Texture2> mStateTTexture;
    std::shared_ptr<gte::Texture2> mStateTp1Texture;
    std::shared_ptr<gte::Texture2> mDivergenceTexture;
    std::shared_ptr<gte::Texture2> mPoissonTexture;
};

}
