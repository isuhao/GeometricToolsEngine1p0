// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include <GTEngine.h>
using namespace gte;

#define USE_DRAW_DIRECT
//#define SAVE_RENDERING_TO_DISK

class GeometryShadersWindow : public Window
{
public:
    virtual ~GeometryShadersWindow();
    GeometryShadersWindow(Parameters& parameters);

    virtual void OnIdle();

private:
    bool CreateScene();
    void UpdateConstants();

    Vector4<float> mTextColor, mClearColor;
    Environment mEnvironment;
    std::string mGTPath;
    std::shared_ptr<ConstantBuffer> mMatrices;
    Matrix4x4<float>* mVWMatrix;
    Matrix4x4<float>* mPMatrix;
    std::shared_ptr<Visual> mMesh;

#ifndef USE_DRAW_DIRECT
    std::shared_ptr<StructuredBuffer> mParticles;
#endif

#ifdef SAVE_RENDERING_TO_DISK
    std::shared_ptr<DrawTarget> mTarget;
#endif
};
