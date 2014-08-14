// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "Delaunay3DWindow.h"

//----------------------------------------------------------------------------
Delaunay3DWindow::~Delaunay3DWindow()
{
    Unregister(mSphere->worldTransform);
    if (mSolidTetra[0])
    {
        Unregister(mSolidTetra[0]->worldTransform);
    }
    else
    {
        Unregister(mWireTetra[0]->worldTransform);
    }
}
//----------------------------------------------------------------------------
Delaunay3DWindow::Delaunay3DWindow(Parameters& parameters)
    :
    Window(parameters),
    mTextColor(0.0f, 0.0f, 0.0f, 1.0f),
    mLightGray(0.75f, 0.75f, 0.75f, 1.0f)
{
    std::string gtPath = mEnvironment.GetVariable("GTE_PATH");
    if (gtPath == "")
    {
        LogError("You must create the environment variable GTE_PATH.");
        parameters.created = false;
        return;
    }
    gtPath += "/Samples/Geometrics/Delaunay3D/";
    mEnvironment.Insert(gtPath);

    if (!CreateScene())
    {
        parameters.created = false;
        return;
    }

    mNoCullState.reset(new RasterizerState());
    mNoCullState->cullMode = RasterizerState::CULL_NONE;

    mNoCullWireState.reset(new RasterizerState());
    mNoCullWireState->cullMode = RasterizerState::CULL_NONE;
    mNoCullWireState->fillMode = RasterizerState::FILL_WIREFRAME;

    mBlendState.reset(new BlendState());
    mBlendState->target[0].enable = true;
    mBlendState->target[0].srcColor = BlendState::BM_SRC_ALPHA;
    mBlendState->target[0].dstColor = BlendState::BM_INV_SRC_ALPHA;
    mBlendState->target[0].srcAlpha = BlendState::BM_SRC_ALPHA;
    mBlendState->target[0].dstAlpha = BlendState::BM_INV_SRC_ALPHA;

    mCamera.SetFrustum(60.0f, GetAspectRatio(), 1.0f, 5000.0f);
    Vector4<float> camPosition(0.0f, 0.0f, -4.0f, 1.0f);
    Vector4<float> camDVector(0.0f, 0.0f, 1.0f, 0.0f);
    Vector4<float> camUVector(0.0f, 1.0f, 0.0f, 0.0f);
    Vector4<float> camRVector = Cross(camDVector, camUVector);
    mCamera.SetFrame(camPosition, camDVector, camUVector, camRVector);
    EnableCameraMotion(0.1f, 0.01f, 2.0f, 2.0f);

    EnableObjectMotion();
}
//----------------------------------------------------------------------------
void Delaunay3DWindow::OnIdle()
{
    MeasureTime();
    MoveCamera();
    mEngine->ClearBuffers();

    mEngine->SetRasterizerState(mNoCullWireState);
    for (auto const& tetra : mWireTetra)
    {
        if (tetra)
        {
            mEngine->Draw(tetra);
        }
    }

    mEngine->SetBlendState(mBlendState);
    mEngine->SetRasterizerState(mNoCullState);
    for (auto const& tetra : mSolidTetra)
    {
        if (tetra)
        {
            mEngine->Draw(tetra);
        }
    }
    mEngine->SetDefaultBlendState();

    mEngine->Draw(mSphere);

    DrawFrameRate(8, mYSize - 8, mTextColor);
    mEngine->DisplayColorBuffer(0);
    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Delaunay3DWindow::OnCharPress(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 's':
    case 'S':
        DoSearch();
        return true;
    }
    return Window::OnCharPress(key, x, y);
}
//----------------------------------------------------------------------------
bool Delaunay3DWindow::CreateScene()
{
#if 1
    // Randomly generated points in the cube [-1,1]^3.  (0,0,0) is added to
    // ensure the hull contains the origin and the virtual trackball display
    // appears centered.
    std::mt19937 mte;
    std::uniform_real_distribution<float> rnd(-1.0f, 1.0f);
    mVertices.resize(128);
    for (auto& v : mVertices)
    {
        for (int j = 0; j < 3; ++j)
        {
            v[j] = rnd(mte);
        }
    }
    mVertices[0] = Vector3<float>::Zero();
#endif

#if 0
    // A cube with 3x3x3 points.
    mVertices.resize(27);
    for (int z = 0, i = 0; z < 3; ++z)
    {
        float fz = z - 1.0f;
        for (int y = 0; y < 3; ++y)
        {
            float fy = y - 1.0f;
            for (int x = 0; x < 3; ++x, ++i)
            {
                float fx = x - 1.0f;
                mVertices[i] = Vector3<float>(fx, fy, fz);
            }
        }
    }
#endif

#if 0
    // Some pathological examples (needle-like configurations).
    //std::string filename = "data1.txt";
    std::string filename = "data2.txt";
    //std::string filename = "data3.txt";

    std::string path = mEnvironment.GetPath(filename);
    if (path == "")
    {
        LogError("Cannot find file " + filename + ".");
        return false;
    }

    std::ifstream input(path);
    int numVertices;
    input >> numVertices;
    mVertices.resize(numVertices);
    for (int i = 0; i < numVertices; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            input >> mVertices[i][j];
        }
    }
#endif

    Vector3<float> vmin, vmax;
    ComputeExtremes((int)mVertices.size(), &mVertices[0], vmin, vmax);
    for (int j = 0; j < 3; ++j)
    {
        mRandom[j] = std::uniform_real_distribution<float>(vmin[j], vmax[j]);
    }

    mDelaunay(static_cast<int>(mVertices.size()), &mVertices[0], 0.001f);
    mInfo.initialTetrahedron = -1;
    mInfo.finalTetrahedron = 0;

    mWireTetra.resize(mDelaunay.GetNumTetrahedra());
    mSolidTetra.resize(mDelaunay.GetNumTetrahedra());

    CreateSphere();
    mVCEffect.reset(new VertexColorEffect());
    for (int j = 0; j < mDelaunay.GetNumTetrahedra(); ++j)
    {
        CreateTetra(j);
    }
    Register(mWireTetra[0]->worldTransform,
        mVCEffect->GetPVWMatrixConstant());

    return true;
}
//----------------------------------------------------------------------------
void Delaunay3DWindow::CreateSphere()
{
    VertexFormat vformat;
    vformat.Bind(VA_POSITION, DF_R32G32B32_FLOAT, 0);
    vformat.Bind(VA_COLOR, DF_R32G32B32_FLOAT, 0);

    MeshFactory mf;
    mf.SetVertexFormat(vformat);

    mSphere = mf.CreateSphere(8, 8, 0.025f);
    std::shared_ptr<VertexColorEffect> effect(new VertexColorEffect());
    mSphere->SetEffect(effect);

    // Move the sphere offscreen initially.
    mSphere->localTransform.SetTranslation(0.0f, 0.0f, -1000.0f);
    mSphere->Update();
    Register(mSphere->worldTransform, effect->GetPVWMatrixConstant());
}
//----------------------------------------------------------------------------
void Delaunay3DWindow::CreateTetra(int index)
{
    std::vector<int> const& dindices = mDelaunay.GetIndices();

    VertexFormat vformat;
    vformat.Bind(VA_POSITION, DF_R32G32B32_FLOAT, 0);
    vformat.Bind(VA_COLOR, DF_R32G32B32A32_FLOAT, 0);

    std::shared_ptr<VertexBuffer> vbuffer(new VertexBuffer(vformat,
        static_cast<unsigned int>(mVertices.size())));
    vbuffer->SetUsage(Resource::DYNAMIC_UPDATE);
    Vertex* vertex = vbuffer->Get<Vertex>();
    for (int j = 0; j < 4; ++j)
    {
        vertex[j].position = mVertices[dindices[4 * index + j]];
        vertex[j].color = mLightGray;
    }

    std::shared_ptr<IndexBuffer> ibuffer(new IndexBuffer(IP_TRIMESH, 4,
        sizeof(unsigned int)));
    unsigned int* indices = ibuffer->Get<unsigned int>();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 3;  indices[5] = 1;
    indices[6] = 0;  indices[7] = 2;  indices[8] = 3;
    indices[9] = 3;  indices[10] = 2;  indices[11] = 1;

    mWireTetra[index].reset(new Visual(vbuffer, ibuffer, mVCEffect));
}
//----------------------------------------------------------------------------
void Delaunay3DWindow::SetAllTetraWire()
{
    for (size_t i = 0; i < mSolidTetra.size(); ++i)
    {
        if (mSolidTetra[i])
        {
            mWireTetra[i] = mSolidTetra[i];
            mSolidTetra[i] = nullptr;
        }

        auto vbuffer = mWireTetra[i]->GetVertexBuffer();
        Vertex* vertex = vbuffer->Get<Vertex>();
        for (int j = 0; j < 4; ++j)
        {
            vertex[j].color = mLightGray;
        }
        mEngine->Update(vbuffer);
    }
}
//----------------------------------------------------------------------------
void Delaunay3DWindow::SetTetraSolid(int index, Vector4<float> const& color)
{
    if (mWireTetra[index])
    {
        mSolidTetra[index] = mWireTetra[index];
        mWireTetra[index] = nullptr;
    }

    auto vbuffer = mSolidTetra[index]->GetVertexBuffer();
    Vertex* vertex = vbuffer->Get<Vertex>();
    for (int j = 0; j < 4; ++j)
    {
        vertex[j].color = color;
    }
    mEngine->Update(vbuffer);
}
//----------------------------------------------------------------------------
void Delaunay3DWindow::SetLastTetraSolid(Vector4<float> const& color,
    Vector4<float> const& oppositeColor)
{
    int index = mInfo.finalTetrahedron;
    int vOpposite = mInfo.finalV[3];

    if (mWireTetra[index])
    {
        mSolidTetra[index] = mWireTetra[index];
        mWireTetra[index] = nullptr;
    }

    auto vbuffer = mSolidTetra[index]->GetVertexBuffer();
    Vertex* vertex = vbuffer->Get<Vertex>();
    for (int j = 0; j < 4; ++j)
    {
        if (j != vOpposite)
        {
            vertex[j].color = color;
        }
        else
        {
            vertex[j].color = oppositeColor;
        }
    }
    mEngine->Update(vbuffer);
}
//----------------------------------------------------------------------------
void Delaunay3DWindow::DoSearch()
{
    SetAllTetraWire();

    // Generate random point in AABB of data set.
    Vector3<float> point;
    for (int j = 0; j < 3; ++j)
    {
        point[j] = mRandom[j](mRandomGenerator);
    }

    // Move sphere to this location.
    mSphere->localTransform.SetTranslation(point);
    mSphere->Update();
    UpdateRegisteredTransforms();

    mInfo.initialTetrahedron = mInfo.finalTetrahedron;
    int found = mDelaunay.GetContainingTetrahedron(point, mInfo);
    if (found >= 0)
    {
        mInfo.initialTetrahedron = mInfo.finalTetrahedron;

        // Make all tetra on the path solid.
        for (int i = 0; i < mInfo.numPath; ++i)
        {
            int index = mInfo.path[i];
            float red, blue;
            if (mInfo.numPath > 1)
            {
                red = i / (mInfo.numPath - 1.0f);
                blue = 1.0f - red;
            }
            else
            {
                red = 1.0f;
                blue = 0.0f;
            }
            SetTetraSolid(index, Vector4<float>(red, 0.0f, blue, 0.5f));
        }
    }
    else
    {
        // The point is outside the convex hull.  Change the wireframe
        // color for the last visited face in the search path.
        SetLastTetraSolid(Vector4<float>(0.0f, 1.0f, 0.0f, 0.5f),
            Vector4<float>(0.0f, 0.25f, 0.0f, 0.5f));
    }
}
//----------------------------------------------------------------------------