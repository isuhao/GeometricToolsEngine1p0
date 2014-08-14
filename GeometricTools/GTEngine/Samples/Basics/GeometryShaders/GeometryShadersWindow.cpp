// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "GeometryShadersWindow.h"

//----------------------------------------------------------------------------
GeometryShadersWindow::~GeometryShadersWindow()
{
}
//----------------------------------------------------------------------------
GeometryShadersWindow::GeometryShadersWindow(Parameters& parameters)
    :
    Window(parameters),
    mTextColor(0.0f, 0.0f, 0.0f, 1.0f),
    mClearColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    mGTPath = mEnvironment.GetVariable("GTE_PATH");
    if (mGTPath == "")
    {
        LogError("You must create the environment variable GTE_PATH.");
        parameters.created = false;
        return;
    }
    mGTPath += "/Samples/Basics/GeometryShaders/Shaders/";
    mEnvironment.Insert(mGTPath);

    if (mEnvironment.GetPath("RandomSquares.hlsl") == "")
    {
        LogError("Cannot find file RandomSquares.hlsl.");
        parameters.created = false;
        return;
    }

    if (mEnvironment.GetPath("RandomSquaresIndirect.hlsl") == "")
    {
        LogError("Cannot find file RandomSquaresIndirect.hlsl.");
        parameters.created = false;
        return;
    }

    if (!CreateScene())
    {
        LogError("A shader failed to compile.");
        parameters.created = false;
        return;
    }
    EnableObjectMotion();

    mEngine->SetClearColor(mClearColor);

    mCamera.SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    Vector4<float> camPosition(2.8f, 0.0f, 0.0f, 1.0f);
    Vector4<float> camDVector(-1.0f, 0.0f, 0.0f, 0.0f);
    Vector4<float> camUVector(0.0f, 0.0f, 1.0f, 0.0f);
    Vector4<float> camRVector = Cross(camDVector, camUVector);
    mCamera.SetFrame(camPosition, camDVector, camUVector, camRVector);
    EnableCameraMotion(0.01f, 0.001f, 2.0f, 2.0f);

#ifdef SAVE_RENDERING_TO_DISK
    mTarget.reset(new DrawTarget(1, DF_R8G8B8A8_UNORM, mXSize, mYSize));
    mTarget->GetRTTexture(0)->SetCopyType(Resource::COPY_STAGING_TO_CPU);
#endif
}
//----------------------------------------------------------------------------
void GeometryShadersWindow::OnIdle()
{
    MeasureTime();

    MoveCamera();
    UpdateConstants();

    mEngine->ClearBuffers();
    mEngine->Draw(mMesh);
    DrawFrameRate(8, mYSize-8, mTextColor);
    mEngine->DisplayColorBuffer(0);

#ifdef SAVE_RENDERING_TO_DISK
    mEngine->Enable(mTarget);
    mEngine->ClearBuffers();
    mEngine->Draw(mMesh);
    mEngine->Disable(mTarget);
    mEngine->CopyGpuToCpu(mTarget->GetRTTexture(0));
    WICFileIO::SaveToPNG("GeometryShaders.png",
        mTarget->GetRTTexture(0).get());
#endif

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool GeometryShadersWindow::CreateScene()
{
    // Create particles used by direct and indirect drawing.
    struct Vertex
    {
        Vector3<float> position;
        Vector3<float> color;
        float size;
    };

    // Use a Mersenne twister engine for random numbers.
    std::mt19937 mte;
    std::uniform_real_distribution<float> symr(-1.0f, 1.0f);
    std::uniform_real_distribution<float> unir(0.0f, 1.0f);
    std::uniform_real_distribution<float> posr(0.01f, 0.1f);

    int const numParticles = 128;
    std::vector<Vertex> particles(numParticles);
    for (auto& particle : particles)
    {
        particle.position = Vector3<float>(symr(mte), symr(mte), symr(mte));
        particle.color = Vector3<float>(unir(mte), unir(mte), unir(mte));
        particle.size = posr(mte);
    }

    // Create the constant buffer used by direct and indirect drawing.
    mMatrices.reset(new ConstantBuffer(2*sizeof(Matrix4x4<float>), true));
    Matrix4x4<float>* matrices = mMatrices->Get<Matrix4x4<float>>();
    matrices[0] = Matrix4x4<float>::Identity();
    matrices[1] = Matrix4x4<float>::Identity();
    mVWMatrix = &matrices[0];
    mPMatrix = &matrices[1];

#ifdef USE_DRAW_DIRECT
    // Create a mesh for direct drawing.
    VertexFormat vformat;
    vformat.Bind(VA_POSITION, DF_R32G32B32_FLOAT, 0);
    vformat.Bind(VA_COLOR, DF_R32G32B32_FLOAT, 0);
    vformat.Bind(VA_TEXCOORD, DF_R32_FLOAT, 0);
    std::shared_ptr<VertexBuffer> vbuffer(new VertexBuffer(vformat,
        numParticles));
    memcpy(vbuffer->GetData(), &particles[0], numParticles*sizeof(Vertex));

    std::shared_ptr<VertexShader> vshader(
        ShaderFactory::CreateVertex(
        mEnvironment.GetPath("RandomSquares.hlsl")));
    if (!vshader)
    {
        LogError("Failed to compile vshader for RandomSquares.hlsl.");
        return false;
    }

    std::shared_ptr<GeometryShader> gshader(
        ShaderFactory::CreateGeometry(
        mEnvironment.GetPath("RandomSquares.hlsl")));
    if (!gshader)
    {
        LogError("Failed to compile gshader for RandomSquares.hlsl.");
        return false;
    }

    std::shared_ptr<PixelShader> pshader(
        ShaderFactory::CreatePixel(
        mEnvironment.GetPath("RandomSquares.hlsl")));
    if (!pshader)
    {
        LogError("Failed to compile pshader for RandomSquares.hlsl.");
        return false;
    }

    gshader->Set("Matrices", mMatrices);
#else
    // Create a mesh for indirect drawing.
    std::shared_ptr<VertexBuffer> vbuffer(new VertexBuffer(numParticles));

    std::shared_ptr<VertexShader> vshader(
        ShaderFactory::CreateVertex(
        mEnvironment.GetPath("RandomSquaresIndirect.hlsl")));
    if (!vshader)
    {
        LogError("Failed to compile vshader for RandomSquaresIndirect.hlsl.");
        return false;
    }

    std::shared_ptr<GeometryShader> gshader(
        ShaderFactory::CreateGeometry(
        mEnvironment.GetPath("RandomSquaresIndirect.hlsl")));
    if (!gshader)
    {
        LogError("Failed to compile gshader for RandomSquaresIndirect.hlsl.");
        return false;
    }

    std::shared_ptr<PixelShader> pshader(
        ShaderFactory::CreatePixel(
        mEnvironment.GetPath("RandomSquaresIndirect.hlsl")));
    if (!pshader)
    {
        LogError("Failed to compile pshader for RandomSquaresIndirect.hlsl.");
        return false;
    }

    mParticles.reset(new StructuredBuffer(numParticles, sizeof(Vertex)));
    memcpy(mParticles->GetData(), &particles[0], numParticles*sizeof(Vertex));
    gshader->Set("Matrices", mMatrices);
    gshader->Set("particles", mParticles);
#endif
    std::shared_ptr<IndexBuffer> ibuffer(new IndexBuffer(IP_POLYPOINT,
        numParticles));
    std::shared_ptr<VisualEffect> effect(new VisualEffect(vshader, pshader,
        gshader));
    mMesh.reset(new Visual(vbuffer, ibuffer, effect));
    return true;
}
//----------------------------------------------------------------------------
void GeometryShadersWindow::UpdateConstants()
{
#if defined(GTE_USE_MAT_VEC)
    *mVWMatrix = mCamera.GetViewMatrix()*mObjectTransform;
#else
    *mVWMatrix = mObjectTransform*mCamera.GetViewMatrix();
#endif

    *mPMatrix = mCamera.GetProjectionMatrix();
    mEngine->Update(mMatrices);
}
//----------------------------------------------------------------------------
