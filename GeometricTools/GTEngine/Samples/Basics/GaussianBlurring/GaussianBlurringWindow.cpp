// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "GaussianBlurringWindow.h"

//----------------------------------------------------------------------------
GaussianBlurringWindow::~GaussianBlurringWindow()
{
}
//----------------------------------------------------------------------------
GaussianBlurringWindow::GaussianBlurringWindow(Parameters& parameters)
    :
    Window(parameters),
    mTextColor(1.0f, 1.0f, 0.0f, 1.0f),
    mNumXThreads(8),
    mNumYThreads(8),
    mNumXGroups(mXSize / mNumXThreads),  // 1024/8 = 128
    mNumYGroups(mYSize / mNumYThreads),  // 768/8 = 96
    mPass(0)
{
    // Set the search path to find images to load.
    Environment env;
    std::string gtpath = env.GetVariable("GTE_PATH");
    if (gtpath == "")
    {
        LogError("You must create the environment variable GTE_PATH.");
        parameters.created = false;
        return;
    }
    env.Insert(gtpath + "/Samples/Basics/GaussianBlurring/Shaders/");
    env.Insert(gtpath + "/Samples/#Data/");

    for (int i = 0; i < 2; ++i)
    {
        mImage[i].reset(new Texture2(DF_R32G32B32A32_FLOAT, mXSize, mYSize));
        mImage[i]->SetUsage(Resource::SHADER_OUTPUT);
    }

    std::string path = env.GetPath("MedicineBag.jpg");
    if (path == "")
    {
        LogError("Cannot find MedicineBag.jpg.");
        parameters.created = false;
        return;
    }
    Texture2* original = WICFileIO::Load(path, false);
    unsigned int const* src = original->Get<unsigned int>();
    float* trg = mImage[0]->Get<float>();
    for (int j = 0; j < mXSize*mYSize; ++j)
    {
        unsigned int rgba = *src++;
        *trg++ = (rgba & 0x000000FF) / 255.0f;
        *trg++ = ((rgba & 0x0000FF00) >> 8) / 255.0f;
        *trg++ = ((rgba & 0x00FF0000) >> 16) / 255.0f;
        *trg++ = 1.0f;
    }
    delete original;

    HLSLDefiner definer;
    definer.SetInt("NUM_X_THREADS", mNumXThreads);
    definer.SetInt("NUM_Y_THREADS", mNumYThreads);
    path = env.GetPath("GaussianBlur3x3.hlsl");
    unsigned int flags =
        D3DCOMPILE_ENABLE_STRICTNESS |
        D3DCOMPILE_IEEE_STRICTNESS |
        D3DCOMPILE_DEBUG |
        D3DCOMPILE_SKIP_OPTIMIZATION;
    mGaussianBlurShader.reset(ShaderFactory::CreateCompute(path, definer,
        "CSMain", "cs_5_0", flags));
    if (!mGaussianBlurShader)
    {
        LogError("Cannot find GaussianBlur3x3.hlsl.");
        parameters.created = false;
        return;
    }
    mGaussianBlurShader->Set("input", mImage[0]);
    mGaussianBlurShader->Set("output", mImage[1]);

    mOverlay.reset(new OverlayEffect(mXSize, mYSize, mXSize, mYSize,
        SamplerState::MIN_P_MAG_P_MIP_P, SamplerState::CLAMP,
        SamplerState::CLAMP, true));
    mOverlay->SetTexture(mImage[1]);

#ifdef SAVE_RENDERING_TO_DISK
    mTarget.reset(new DrawTarget(1, DF_R8G8B8A8_UNORM, mXSize, mYSize));
    mTarget->GetRTTexture(0)->SetCopyType(Resource::COPY_STAGING_TO_CPU);
#endif
}
//----------------------------------------------------------------------------
void GaussianBlurringWindow::OnIdle()
{
    MeasureTime();

#ifdef SAVE_RENDERING_TO_DISK
    if (mPass == 0 || mPass == 100 || mPass == 1000 || mPass == 10000)
    {
        mOverlay->SetTexture(mImage[0]);
        mEngine->SetClearColor(Vector4<float>(0.0f, 0.0f, 0.0f, 1.0f));
        mEngine->ClearBuffers();
        mEngine->Enable(mTarget);
        mEngine->Draw(mOverlay);
        mEngine->Disable(mTarget);
        mEngine->CopyGpuToCpu(mTarget->GetRTTexture(0));
        WICFileIO::SaveToPNG("Gauss" + std::to_string(mPass) + ".png",
            mTarget->GetRTTexture(0).get());
        mOverlay->SetTexture(mImage[1]);
    }
#endif

    mEngine->Execute(mGaussianBlurShader, mNumXGroups, mNumYGroups, 1);
    mEngine->Draw(mOverlay);
    std::swap(mImage[0], mImage[1]);
    mGaussianBlurShader->Set("input", mImage[0]);
    mGaussianBlurShader->Set("output", mImage[1]);
    mOverlay->SetTexture(mImage[1]);
    ++mPass;

    DrawFrameRate(8, mYSize-8, mTextColor);
    mEngine->DisplayColorBuffer(0);

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
