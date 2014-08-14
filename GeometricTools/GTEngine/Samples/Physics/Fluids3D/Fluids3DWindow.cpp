// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "Fluids3DWindow.h"

//----------------------------------------------------------------------------
Fluids3DWindow::~Fluids3DWindow()
{
    delete mFluid;
}
//----------------------------------------------------------------------------
Fluids3DWindow::Fluids3DWindow(Parameters& parameters)
    :
    Window(parameters),
    mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    // Set the search path to find images to load.
    std::string gtpath = mEnvironment.GetVariable("GTE_PATH");
    if (gtpath == "")
    {
        LogError("You must create the environment variable GTE_PATH.");
        parameters.created = false;
        return;
    }
    mEnvironment.Insert(gtpath + "/Samples/Physics/Fluids3D/Shaders/");

    mCamera.SetFrustum(60.0f, GetAspectRatio(), 0.01f, 100.0f);
    Vector4<float> camPosition(0.0f, 0.0f, -2.25f, 1.0f);
    Vector4<float> camDVector(0.0f, 0.0f, 1.0f, 0.0f);
    Vector4<float> camUVector(0.0f, 1.0f, 0.0f, 0.0f);
    Vector4<float> camRVector = Cross(camDVector, camUVector);
    mCamera.SetFrame(camPosition, camDVector, camUVector, camRVector);
    EnableCameraMotion(0.01f, 0.001f, 2.0f, 2.0f);

    mFluid = new Fluid3(mEngine, GRID_SIZE, GRID_SIZE, GRID_SIZE, 0.002f);
    mFluid->Initialize();

    // Use blending for the visualization.
    mAlphaState.reset(new BlendState());
    mAlphaState->target[0].enable = true;
    mAlphaState->target[0].srcColor = BlendState::BM_SRC_ALPHA;
    mAlphaState->target[0].dstColor = BlendState::BM_INV_SRC_ALPHA;
    mAlphaState->target[0].srcAlpha = BlendState::BM_SRC_ALPHA;
    mAlphaState->target[0].dstAlpha = BlendState::BM_INV_SRC_ALPHA;
    mEngine->SetBlendState(mAlphaState);

    // The alpha channel must be zero for the blending of density to work
    // correctly through the fluid region.
    mEngine->SetClearColor(Vector4<float>(1.0f, 1.0f, 1.0f, 0.0f));

    // The geometric proxies for volume rendering are concentric boxes.  They
    // are drawn from inside to outside for correctly sorted drawing, so depth
    // buffering is not needed.
    mNoDepthState.reset(new DepthStencilState());
    mNoDepthState->depthEnable = false;
    mEngine->SetDepthStencilState(mNoDepthState);

    mTrilinearClampSampler.reset(new SamplerState());
    mTrilinearClampSampler->filter = SamplerState::MIN_L_MAG_L_MIP_P;
    mTrilinearClampSampler->mode[0] = SamplerState::CLAMP;
    mTrilinearClampSampler->mode[1] = SamplerState::CLAMP;
    mTrilinearClampSampler->mode[2] = SamplerState::CLAMP;

    CreateNestedBoxes();
    EnableObjectMotion();
}
//----------------------------------------------------------------------------
void Fluids3DWindow::OnIdle()
{
    MeasureTime();

    MoveCamera();
    UpdateConstants();

    mFluid->DoSimulationStep();

    mEngine->ClearBuffers();
    for (auto visual : mVisible)
    {
        mEngine->Draw(visual);
    }
    DrawFrameRate(8, mYSize - 8, mTextColor);
    mEngine->DisplayColorBuffer(1);

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Fluids3DWindow::OnCharPress(unsigned char key, int x, int y)
{
    if (key == '0')
    {
        mFluid->Initialize();
        return true;
    }

    return Window::OnCharPress(key, x, y);
}
//----------------------------------------------------------------------------
void Fluids3DWindow::CreateNestedBoxes()
{
    mPVWMatrixBuffer.reset(new ConstantBuffer(sizeof(Matrix4x4<float>),
        true));
    Matrix4x4<float>& pvwMatrix =
        *mPVWMatrixBuffer->Get<Matrix4x4<float>>();
    pvwMatrix = Matrix4x4<float>::Identity();

    std::string path = mEnvironment.GetPath("VolumeRender.hlsl");
    std::shared_ptr<VertexShader> vshader(ShaderFactory::CreateVertex(path));
    LogAssert(vshader != nullptr, "Cannot create shader.");
    vshader->Set("PVWMatrix", mPVWMatrixBuffer);

    std::shared_ptr<PixelShader> pshader(ShaderFactory::CreatePixel(path));
    LogAssert(pshader != nullptr, "Cannot create shader.");
    pshader->Set("volumeTexture", mFluid->GetState());
    pshader->Set("trilinearClampSampler", mTrilinearClampSampler);

    std::shared_ptr<VisualEffect> effect(new VisualEffect(vshader, pshader));

    struct Vertex { Vector3<float> position, tcoord; };
    VertexFormat vformat;
    vformat.Bind(VA_POSITION, DF_R32G32B32_FLOAT, 0);
    vformat.Bind(VA_TEXCOORD, DF_R32G32B32_FLOAT, 0);

    MeshFactory mf;
    mf.SetVertexFormat(vformat);
    int const numBoxes = 128;
    for (int i = 1; i <= numBoxes; ++i)
    {
        float extent = 0.5f*i/(numBoxes - 1.0f);
        std::shared_ptr<Visual> visual(mf.CreateBox(extent, extent, extent));
        VertexBuffer* vbuffer = visual->GetVertexBuffer().get();
        Vertex* vertex = vbuffer->Get<Vertex>();
        for (unsigned int j = 0; j < vbuffer->GetNumElements(); ++j, ++vertex)
        {
            Vector3<float>& tcd = vertex->tcoord;
            Vector3<float> pos = vertex->position;
            Vector4<float> tmp(pos[0] + 0.5f, pos[1] + 0.5f, pos[2] + 0.5f,
                0.0f);
            for (int k = 0; k < 3; ++k)
            {
                tcd[k] = 0.5f*(tmp[k] + 1.0f);
            }
        }

        visual->SetEffect(effect);
        mVisible.push_back(visual);
    }
}
//----------------------------------------------------------------------------
void Fluids3DWindow::UpdateConstants()
{
    Matrix4x4<float> pvMatrix = mCamera.GetProjectionViewMatrix();
    Matrix4x4<float>& pvwMatrix = *mPVWMatrixBuffer->Get<Matrix4x4<float>>();

#if defined(GTE_USE_MAT_VEC)
    pvwMatrix = pvMatrix*mObjectTransform;
#else
    pvwMatrix = mObjectTransform*pvMatrix;
#endif
    mEngine->Update(mPVWMatrixBuffer);
}
//----------------------------------------------------------------------------
