// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "GTEnginePCH.h"
#include "GtePicker.h"
#include "GteNode.h"
#include "GteVisual.h"
#include "GteIntrLine3Triangle3.h"
using namespace gte;

PickRecord const Picker::msInvalid;

//----------------------------------------------------------------------------
Picker::~Picker ()
{
}
//----------------------------------------------------------------------------
Picker::Picker ()
    :
    mOrigin(Vector4<float>::Origin()),
    mDirection(Vector4<float>::Zero()),
    mTMin(0.0f),
    mTMax(0.0f)
{
}
//----------------------------------------------------------------------------
void Picker::Execute (Spatial* scene, Vector4<float> const& origin,
    Vector4<float> const& direction, float tmin, float tmax)
{
#ifdef _DEBUG
    if (tmin == -std::numeric_limits<float>::max())
    {
        LogAssert(tmax == std::numeric_limits<float>::max(),
            "Invalid inputs.");
    }
    else
    {
        LogAssert(tmin == 0.0f && tmax > 0.0f, "Invalid inputs.");
    }
#endif

    mOrigin = origin;
    mDirection = direction;
    mTMin = tmin;
    mTMax = tmax;
    records.clear();
    ExecuteRecursive(scene);
}
//----------------------------------------------------------------------------
PickRecord const& Picker::GetClosestToZero () const
{
    if (records.size() > 0)
    {
        auto iter = records.begin(), end = records.end(), candidate = iter;
        float closest = std::abs(iter->t);
        for (/**/; iter != end; ++iter)
        {
            float tmp = std::abs(iter->t);
            if (tmp < closest)
            {
                closest = tmp;
                candidate = iter;
            }
        }
        return *candidate;
    }
    else
    {
        return msInvalid;
    }
}
//----------------------------------------------------------------------------
const PickRecord& Picker::GetClosestNonnegative () const
{
    if (records.size() > 0)
    {
        // Get first nonnegative value.
        auto iter = records.begin(), end = records.end(), candidate = iter;
        float closest = std::numeric_limits<float>::max();
        for (/**/; iter != end; ++iter)
        {
            if (iter->t >= 0.0f)
            {
                closest = iter->t;
                candidate = iter;
                break;
            }
        }

        if (iter != end)
        {
            for (++iter; iter != end; ++iter)
            {
                if (0.0f <= iter->t && iter->t < closest)
                {
                    closest = iter->t;
                    candidate = iter;
                }
            }
            return *candidate;
        }
        else
        {
            // All values are negative.
            return msInvalid;
        }
    }
    else
    {
        return msInvalid;
    }
}
//----------------------------------------------------------------------------
const PickRecord& Picker::GetClosestNonpositive () const
{
    if (records.size() > 0)
    {
        // Get first nonpositive value.
        auto iter = records.begin(), end = records.end(), candidate = iter;
        float closest = -std::numeric_limits<float>::max();
        for (/**/; iter != end; ++iter)
        {
            if (iter->t <= 0.0f)
            {
                closest = iter->t;
                candidate = iter;
                break;
            }
        }

        if (iter != end)
        {
            for (++iter; iter != end; ++iter)
            {
                if (closest < iter->t && iter->t <= 0.0f)
                {
                    closest = iter->t;
                    candidate = iter;
                }
            }
            return *candidate;
        }
        else
        {
            // All values are positive.
            return msInvalid;
        }
    }
    else
    {
        return msInvalid;
    }
}
//----------------------------------------------------------------------------
void Picker::ExecuteRecursive (Spatial* object)
{
    Visual* visual = dynamic_cast<Visual*>(object);
    if (visual)
    {
        if (visual->worldBound.TestIntersection(mOrigin, mDirection, mTMin,
            mTMax))
        {
            // Convert the linear component to model-space coordinates.
            Matrix4x4<float> invWorldMatrix = Inverse(visual->worldTransform);
            Line3<float> line;
            Vector4<float> temp;
#if defined (GTE_USE_MAT_VEC)
            temp = invWorldMatrix*mOrigin;
            line.origin = Vector3<float>(temp[0], temp[1], temp[2]);
            temp = invWorldMatrix*mDirection;
            line.direction = Vector3<float>(temp[0], temp[1], temp[2]);
#else
            temp = mOrigin*invWorldMatrix;
            line.origin = Vector3<float>(temp[0], temp[1], temp[2]);
            temp = mDirection*invWorldMatrix;
            line.direction = Vector3<float>(temp[0], temp[1], temp[2]);
#endif

            // Get the position data.
            VertexBuffer* vbuffer = visual->GetVertexBuffer().get();
            std::set<DFType> required;
            required.insert(DF_R32G32B32_FLOAT);
            required.insert(DF_R32G32B32A32_FLOAT);
            char const* positions = vbuffer->GetChannel(VA_POSITION, 0,
                required);
            if (!positions)
            {
                LogError("Expecting positions.");
                return;
            }

            // Get triangle primitives.
            IndexBuffer* ibuffer = visual->GetIndexBuffer().get();
            IPType primitiveType = ibuffer->GetPrimitiveType();
            if ((primitiveType & IP_HAS_TRIANGLES) == 0)
            {
                // Picking is not defined for point or segment primitives.
                // Intersection testing for such primitives with a line is
                // ill conditioned.  Add distance queries for these.
                return;
            }

            // Compute intersections with the model-space triangles.
            unsigned int const stride = vbuffer->GetElementSize();
            unsigned int const numTriangles = ibuffer->GetNumPrimitives();
            bool isIndexed = ibuffer->IsIndexed();
            for (unsigned int i = 0; i < numTriangles; ++i)
            {
                // Get the vertex indices for the triangle.
                unsigned int v0, v1, v2;
                if (isIndexed)
                {
                    ibuffer->GetTriangle(i, v0, v1,v2);
                }
                else if (primitiveType == IP_TRIMESH)
                {
                    v0 = 3*i;
                    v1 = v0 + 1;
                    v2 = v0 + 2;
                }
                else  // primitiveType == IP_TRISTRIP
                {
                    int offset = (i & 1);
                    v0 = i + offset;
                    v1 = i + 1 + offset;
                    v2 = i + 2 - offset;
                }

                // Get the vertex positions.
                float const* p0 = (float const*)(positions + v0*stride);
                float const* p1 = (float const*)(positions + v1*stride);
                float const* p2 = (float const*)(positions + v2*stride);

                // Create the query triangle in model space.
                Triangle3<float> triangle(
                    Vector3<float>(p0[0], p0[1], p0[2]),
                    Vector3<float>(p1[0], p1[1], p1[2]),
                    Vector3<float>(p2[0], p2[1], p2[2]));

                // Compute line-triangle intersection.
                FIQuery<float, Line3<float>, Triangle3<float>> findQuery;
                auto result = findQuery(line, triangle);
                if (result.intersect
                    && mTMin <= result.parameter
                    && result.parameter <= mTMax)
                {
                    PickRecord record;
                    record.intersected = visual;
                    record.t = result.parameter;
                    record.triangle = i;
                    record.bary[0] = result.triangleBary[0];
                    record.bary[1] = result.triangleBary[1];
                    record.bary[2] = result.triangleBary[2];
                    records.push_back(record);
                }
            }
        }
        return;
    }

    Node* node = dynamic_cast<Node*>(object);
    if (node)
    {
        if (node->worldBound.TestIntersection(mOrigin, mDirection, mTMin,
            mTMax))
        {
            int const numChildren = node->GetNumChildren();
            for (int i = 0; i < numChildren; ++i)
            {
                std::shared_ptr<Spatial> child = node->GetChild(i);
                if (child)
                {
                    ExecuteRecursive(child.get());
                }
            }
        }
        return;
    }

    // We should not get here when the scene graph has only Spatial, Node,
    // and Visual.  However, in case someone adds a new Spatial-derived
    // type later, let's trap it.
    LogError("Invalid object type.");
}
//----------------------------------------------------------------------------
