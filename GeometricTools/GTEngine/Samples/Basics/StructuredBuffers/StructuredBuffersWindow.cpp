// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "StructuredBuffersWindow.h"

//----------------------------------------------------------------------------
StructuredBuffersWindow::~StructuredBuffersWindow()
{
    Unregister(mSquare->worldTransform);
    delete mDrawnPixelsTexture;
}
//----------------------------------------------------------------------------
StructuredBuffersWindow::StructuredBuffersWindow(Parameters& parameters)
    :
    Window(parameters),
    mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
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
    env.Insert(gtpath + "/Samples/Basics/StructuredBuffers/Shaders/");
    env.Insert(gtpath + "/Samples/#Data/");
    if (env.GetPath("StructuredBuffers.hlsl") == "")
    {
        LogError("Cannot find file StructuredBuffers.hlsl.");
        parameters.created = false;
        return;
    }
    if (env.GetPath("StoneWall.png") == "")
    {
        LogError("Cannot find file StoneWall.png.");
        parameters.created = false;
        return;
    }

    mCamera.SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    Vector4<float> camPosition(0.0f, 0.0f, 1.25f, 1.0f);
    Vector4<float> camDVector(0.0f, 0.0f, -1.0f, 0.0f);
    Vector4<float> camUVector(0.0f, 1.0f, 0.0f, 0.0f);
    Vector4<float> camRVector = Cross(camDVector, camUVector);
    mCamera.SetFrame(camPosition, camDVector, camUVector, camRVector);
    EnableCameraMotion(0.001f, 0.001f, 2.0f, 2.0f);

    // Create a vertex buffer for a single triangle.  The PNG is stored in
    // left-handed coordinates.  The texture coordinates are chosen to reflect
    // the texture in the y-direction.
    struct Vertex
    {
        Vector3<float> position;
        Vector2<float> tcoord;
    };
    VertexFormat vformat;
    vformat.Bind(VA_POSITION, DF_R32G32B32_FLOAT, 0);
    vformat.Bind(VA_TEXCOORD, DF_R32G32_FLOAT, 0);
    std::shared_ptr<VertexBuffer> vbuffer(new VertexBuffer(vformat, 4));
    Vertex* vertex = vbuffer->Get<Vertex>();
    vertex[0].position = Vector3<float>(0.0f, 0.0f, 0.0f);
    vertex[0].tcoord = Vector2<float>(0.0f, 1.0f);
    vertex[1].position = Vector3<float>(1.0f, 0.0f, 0.0f);
    vertex[1].tcoord = Vector2<float>(1.0f, 1.0f);
    vertex[2].position = Vector3<float>(0.0f, 1.0f, 0.0f);
    vertex[2].tcoord = Vector2<float>(0.0f, 0.0f);
    vertex[3].position = Vector3<float>(1.0f, 1.0f, 0.0f);
    vertex[3].tcoord = Vector2<float>(1.0f, 0.0f);

    // Create an indexless buffer for a triangle mesh with two triangles.
    std::shared_ptr<IndexBuffer> ibuffer(new IndexBuffer(IP_TRISTRIP, 2));

    // Create the vertex shader and associated resources.
    HLSLDefiner definer;
    definer.SetInt("WINDOW_WIDTH", mXSize);
    std::shared_ptr<VertexShader> vshader(ShaderFactory::CreateVertex(
        env.GetPath("StructuredBuffers.hlsl"), definer));
    if (!vshader)
    {
        LogError("Failed to compile vshader of StructuredBuffers.hlsl.");
        parameters.created = false;
        return;
    }

    mPVWMatrix.reset(new ConstantBuffer(sizeof(Matrix4x4<float>), true));
    vshader->Set("PVWMatrix", mPVWMatrix);

    // Create the pixel shader and associated resources.
    std::shared_ptr<PixelShader> pshader(ShaderFactory::CreatePixel(
        env.GetPath("StructuredBuffers.hlsl"), definer));
    if (!pshader)
    {
        LogError("Failed to compile pshader of StructuredBuffers.hlsl.");
        parameters.created = false;
        return;
    }

    std::string path = env.GetPath("StoneWall.png");
    std::shared_ptr<Texture2> baseTexture(WICFileIO::Load(path, false));
    pshader->Set("baseTexture", baseTexture);

    std::shared_ptr<SamplerState> baseSampler(new SamplerState());
    baseSampler->filter = SamplerState::MIN_L_MAG_L_MIP_P;
    baseSampler->mode[0] = SamplerState::CLAMP;
    baseSampler->mode[1] = SamplerState::CLAMP;
    pshader->Set("baseSampler", baseSampler);

    mDrawnPixels.reset(new StructuredBuffer(mXSize*mYSize,
        sizeof(Vector4<float>)));
    mDrawnPixels->SetUsage(Resource::SHADER_OUTPUT);
    mDrawnPixels->SetCopyType(Resource::COPY_BIDIRECTIONAL);
    memset(mDrawnPixels->GetData(), 0, mDrawnPixels->GetNumBytes());
    pshader->Set("drawnPixels", mDrawnPixels);

    // The structured buffer is written in the pixel shader.  This texture
    // will receive a copy of it so that we can write the results to disk
    // as a PNG file.
    mDrawnPixelsTexture = new Texture2(DF_R8G8B8A8_UNORM, mXSize, mYSize);

    // Create the geometric object for drawing.  Translate it so that its
    // center of mass is at the origin.  This supports virtual trackball
    // motion about the object "center".
    std::shared_ptr<VisualEffect> effect(new VisualEffect(vshader, pshader));
    mSquare.reset(new Visual(vbuffer, ibuffer, effect));
    mSquare->localTransform.SetTranslation(-0.5f, -0.5f, 0.0f);
    mSquare->Update();

    // Enable the virtual trackball to rotate the square.
    Register(mSquare->worldTransform, mPVWMatrix);
    EnableObjectMotion();
}
//----------------------------------------------------------------------------
void StructuredBuffersWindow::OnIdle()
{
    MeasureTime();

    MoveCamera();

    memset(mDrawnPixels->GetData(), 0, mDrawnPixels->GetNumBytes());
    mEngine->CopyCpuToGpu(mDrawnPixels);

    mEngine->ClearBuffers();
    mEngine->Draw(mSquare);

    mEngine->CopyGpuToCpu(mDrawnPixels);
    Vector4<float>* src = mDrawnPixels->Get<Vector4<float>>();
    unsigned int* trg = mDrawnPixelsTexture->Get<unsigned int>();
    for (int i = 0; i < mXSize*mYSize; ++i)
    {
        unsigned int r = static_cast<unsigned char>(255.0f*src[i][0]);
        unsigned int g = static_cast<unsigned char>(255.0f*src[i][1]);
        unsigned int b = static_cast<unsigned char>(255.0f*src[i][2]);
        trg[i] = r | (g << 8) | (b << 16) | (0xFF << 24);
    }
    WICFileIO::SaveToPNG("DrawnPixels.png", mDrawnPixelsTexture);

    DrawFrameRate(8, mYSize - 8, mTextColor);
    mEngine->DisplayColorBuffer(0);

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
