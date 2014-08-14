// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "Fluids2DWindow.h"

//----------------------------------------------------------------------------
Fluids2DWindow::~Fluids2DWindow()
{
    delete mFluid;
}
//----------------------------------------------------------------------------
Fluids2DWindow::Fluids2DWindow(Parameters& parameters)
    :
    Window(parameters),
    mTextColor(1.0f, 1.0f, 0.0f, 1.0f)
{
    // Set the search path to find images to load.
    std::string gtpath = mEnvironment.GetVariable("GTE_PATH");
    if (gtpath == "")
    {
        LogError("You must create the environment variable GTE_PATH.");
        parameters.created = false;
        return;
    }
    mEnvironment.Insert(gtpath + "/Samples/Physics/Fluids2D/Shaders/");

    mFluid = new Fluid2(mEngine, GRID_SIZE, GRID_SIZE, 0.001f,
        0.0001f, 0.0001f);
    mFluid->Initialize();

    // Create the supporting objects for visualizing the fluid simulation.
    std::string path = mEnvironment.GetPath("DrawDensity.hlsl");
    mDrawDensityShader.reset(ShaderFactory::CreatePixel(path));

    std::shared_ptr<SamplerState> bilinearClampSampler(new SamplerState());
    bilinearClampSampler->filter = SamplerState::MIN_L_MAG_L_MIP_P;
    bilinearClampSampler->mode[0] = SamplerState::CLAMP;
    bilinearClampSampler->mode[1] = SamplerState::CLAMP;

    mDrawDensityShader->Set("state", mFluid->GetState());
    mDrawDensityShader->Set("bilinearClampSampler", bilinearClampSampler);

    mOverlay.reset(new OverlayEffect(mXSize, mYSize, GRID_SIZE, GRID_SIZE,
        mDrawDensityShader));

    mNoDepthState.reset(new DepthStencilState());
    mNoDepthState->depthEnable = false;
    mEngine->SetDepthStencilState(mNoDepthState);
    mNoCullingState.reset(new RasterizerState());
    mNoCullingState->cullMode = RasterizerState::CULL_NONE;
    mEngine->SetRasterizerState(mNoCullingState);

#if defined(SAVE_RENDERING_TO_DISK)
    mTarget.reset(new DrawTarget(1, DF_R8G8B8A8_UNORM, mXSize, mYSize));
    mTarget->GetRTTexture(0)->SetCopyType(Resource::COPY_STAGING_TO_CPU);
    mVideoFrame = 0;
#endif
}
//----------------------------------------------------------------------------
void Fluids2DWindow::OnIdle()
{
    MeasureTime();

    mFluid->DoSimulationStep();
    mEngine->Draw(mOverlay);
    DrawFrameRate(8, mYSize - 8, mTextColor);
    mEngine->DisplayColorBuffer(1);

#if defined(SAVE_RENDERING_TO_DISK)
    mEngine->Enable(mTarget);
    mEngine->ClearBuffers();
    mEngine->Draw(mOverlay);
    mEngine->Disable(mTarget);
    mEngine->CopyGpuToCpu(mTarget->GetRTTexture(0));
    WICFileIO::SaveToPNG(
        "Video/Smoke" + std::to_string(mVideoFrame) + ".png",
        mTarget->GetRTTexture(0).get());
    ++mVideoFrame;
#endif

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Fluids2DWindow::OnCharPress(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '0':
        mFluid->Initialize();
        return true;
    }

    return Window::OnCharPress(key, x, y);
}
//----------------------------------------------------------------------------
