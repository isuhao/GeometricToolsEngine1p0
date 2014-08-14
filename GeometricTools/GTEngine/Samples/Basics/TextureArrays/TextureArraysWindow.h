// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include <GTEngine.h>
using namespace gte;

class TextureArraysWindow : public Window
{
public:
    virtual ~TextureArraysWindow();
    TextureArraysWindow(Parameters& parameters);

    virtual void OnIdle();

private:
    std::shared_ptr<Visual> mSquare;
    std::shared_ptr<VisualEffect> mT2ArrayEffect;
    std::shared_ptr<ConstantBuffer> mPVWMatrixBuffer;
};
