// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "BlownGlassWindow.h"

//----------------------------------------------------------------------------
BlownGlassWindow::~BlownGlassWindow()
{
    Unregister(mMesh->worldTransform);
    delete mFluid;
}
//----------------------------------------------------------------------------
BlownGlassWindow::BlownGlassWindow(Parameters& parameters)
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
    mEnvironment.Insert(gtpath + "/Samples/Graphics/BlownGlass/Shaders/");
    mEnvironment.Insert(gtpath + "/Samples/Graphics/BlownGlass/Geometry/");

    mCamera.SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    Vector4<float> camPosition(2.5f, 0.0f, 0.0f, 1.0f);
    Vector4<float> camDVector(-1.0f, 0.0f, 0.0f, 0.0f);
    Vector4<float> camUVector(0.0f, 0.0f, 1.0f, 0.0f);
    Vector4<float> camRVector = Cross(camDVector, camUVector);
    mCamera.SetFrame(camPosition, camDVector, camUVector, camRVector);
    EnableCameraMotion(0.01f, 0.001f, 2.0f, 2.0f);

    // Use blending for the visualization.
    std::shared_ptr<BlendState> bstate(new BlendState());
    bstate->target[0].enable = true;
    bstate->target[0].srcColor = BlendState::BM_SRC_ALPHA;
    bstate->target[0].dstColor = BlendState::BM_INV_SRC_ALPHA;
    bstate->target[0].srcAlpha = BlendState::BM_SRC_ALPHA;
    bstate->target[0].dstAlpha = BlendState::BM_INV_SRC_ALPHA;
    mEngine->SetBlendState(bstate);

    // The alpha channel must be zero for the blending of density to work
    // correctly through the fluid region.
    mEngine->SetClearColor(Vector4<float>(1.0f, 1.0f, 1.0f, 0.0f));

    // Disable face culling.
    std::shared_ptr<RasterizerState> rstate(new RasterizerState());
    rstate->cullMode = RasterizerState::CULL_NONE;
    mEngine->SetRasterizerState(rstate);

    // Read the depth buffer but do not write to it.
    std::shared_ptr<DepthStencilState> dstate(new DepthStencilState());
    dstate->writeMask = DepthStencilState::MASK_ZERO;
    mEngine->SetDepthStencilState(dstate);

    CreateScene();
    EnableObjectMotion();
}
//----------------------------------------------------------------------------
void BlownGlassWindow::OnIdle()
{
    MeasureTime();
    MoveCamera();
    mFluid->DoSimulationStep();
    mEngine->ClearBuffers();
    mEngine->Draw(mMesh);
    DrawFrameRate(8, mYSize-8, mTextColor);
    mEngine->DisplayColorBuffer(1);
    UpdateFrameCount();
}
//----------------------------------------------------------------------------
void BlownGlassWindow::CreateScene()
{
    // Create the fluid simulator.
    mFluid = new Fluid3(mEngine, GRID_SIZE, GRID_SIZE, GRID_SIZE, 0.002f);
    mFluid->Initialize();

    // Create the vertex shader for visualization.
    std::string path = mEnvironment.GetPath("VolumeRender.hlsl");
    std::shared_ptr<VertexShader> vshader(ShaderFactory::CreateVertex(path));
    std::shared_ptr<ConstantBuffer> pvwMatrix(new ConstantBuffer(
        sizeof(Matrix4x4<float>), true));
    *pvwMatrix->Get<Matrix4x4<float>>() = Matrix4x4<float>::Identity();
    vshader->Set("PVWMatrix", pvwMatrix);

    // Create the pixel shader for visualization.
    std::shared_ptr<PixelShader> pshader(ShaderFactory::CreatePixel(path));
    std::shared_ptr<SamplerState> sampler(new SamplerState());
    sampler->filter = SamplerState::MIN_L_MAG_L_MIP_P;
    sampler->mode[0] = SamplerState::CLAMP;
    sampler->mode[1] = SamplerState::CLAMP;
    sampler->mode[2] = SamplerState::CLAMP;
    pshader->Set("volumeTexture", mFluid->GetState());
    pshader->Set("trilinearClampSampler", sampler);

    std::shared_ptr<VisualEffect> effect(new VisualEffect(vshader, pshader));

    // Load the level-surface mesh obtained from the SurfaceExtraction sample.
    VertexFormat vformat;
    vformat.Bind(VA_POSITION, DF_R32G32B32_FLOAT, 0);
    unsigned int numVertices = 82832;
    std::shared_ptr<VertexBuffer> vbuffer(new VertexBuffer(vformat,
        numVertices));
    path = mEnvironment.GetPath("Vertices82832.raw");
    std::ifstream input(path, std::ios::in | std::ios::binary);
    input.read(vbuffer->GetData(), vbuffer->GetNumBytes());
    input.close();

    unsigned int numTriangles = 41388;
    std::shared_ptr<IndexBuffer> ibuffer(new IndexBuffer(IP_TRIMESH,
        numTriangles, sizeof(unsigned int)));
    path = mEnvironment.GetPath("Indices41388.raw");
    input.open(path, std::ios::in | std::ios::binary);
    input.read(ibuffer->GetData(), ibuffer->GetNumBytes());
    input.close();

    mMesh.reset(new Visual(vbuffer, ibuffer, effect));
    mMesh->localTransform.SetTranslation(-1.0f, -1.0f, -1.0f);
    mMesh->Update();

    // Automatic update of transforms for virtual trackball.
    Register(mMesh->worldTransform, pvwMatrix);
}
//----------------------------------------------------------------------------
