// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "CpuMassSpringVolume.h"

//----------------------------------------------------------------------------
CpuMassSpringVolume::~CpuMassSpringVolume()
{
}
//----------------------------------------------------------------------------
CpuMassSpringVolume::CpuMassSpringVolume(int numColumns, int numRows,
    int numSlices, float step, float viscosity, Environment&, bool& created)
    :
    mNumColumns(numColumns),
    mNumRows(numRows),
    mNumSlices(numSlices),
    mNumSliceElements(numColumns * numRows),
    mStep(step),
    mHalfStep(step / 2.0f),
    mSixthStep(step / 6.0f),
    mViscosity(viscosity),
    mMass(numColumns, numRows, numSlices),
    mInvMass(numColumns, numRows, numSlices),
    mPosition(numColumns, numRows, numSlices),
    mVelocity(numColumns, numRows, numSlices),
    mConstantC(numColumns, numRows, numSlices),
    mLengthC(numColumns, numRows, numSlices),
    mConstantR(numColumns, numRows, numSlices),
    mLengthR(numColumns, numRows, numSlices),
    mConstantS(numColumns, numRows, numSlices),
    mLengthS(numColumns, numRows, numSlices),
    mPTmp(numColumns, numRows, numSlices),
    mPAllTmp(numColumns, numRows, numSlices),
    mVTmp(numColumns, numRows, numSlices),
    mVAllTmp(numColumns, numRows, numSlices)
{
    created = true;
}
//----------------------------------------------------------------------------
void CpuMassSpringVolume::SetMass(int c, int r, int s, float mass)
{
    if (0.0f < mass && mass < std::numeric_limits<float>::max())
    {
        mMass(c, r, s) = mass;
        mInvMass(c, r, s) = 1.0f / mass;
    }
    else
    {
        mMass(c, r, s) = std::numeric_limits<float>::max();
        mInvMass(c, r, s) = 0.0f;
    }
}
//----------------------------------------------------------------------------
void CpuMassSpringVolume::SetPosition(int c, int r, int s,
    Vector3<float> const& position)
{
    mPosition(c, r, s) = position;
}
//----------------------------------------------------------------------------
void CpuMassSpringVolume::SetVelocity(int c, int r, int s,
    Vector3<float> const& velocity)
{
    mVelocity(c, r, s) = velocity;
}
//----------------------------------------------------------------------------
void CpuMassSpringVolume::SetConstantC(int c, int r, int s, float v)
{
    mConstantC(c, r, s) = v;
}
//----------------------------------------------------------------------------
void CpuMassSpringVolume::SetLengthC(int c, int r, int s, float v)
{
    mLengthC(c, r, s) = v;
}
//----------------------------------------------------------------------------
void CpuMassSpringVolume::SetConstantR(int c, int r, int s, float v)
{
    mConstantR(c, r, s) = v;
}
//----------------------------------------------------------------------------
void CpuMassSpringVolume::SetLengthR(int c, int r, int s, float v)
{
    mLengthR(c, r, s) = v;
}
//----------------------------------------------------------------------------
void CpuMassSpringVolume::SetConstantS(int c, int r, int s, float v)
{
    mConstantS(c, r, s) = v;
}
//----------------------------------------------------------------------------
void CpuMassSpringVolume::SetLengthS(int c, int r, int s, float v)
{
    mLengthS(c, r, s) = v;
}
//----------------------------------------------------------------------------
Vector3<float> CpuMassSpringVolume::GetPosition(int c, int r, int s) const
{
    return mPosition(c, r, s);
}
//----------------------------------------------------------------------------
Image3<Vector3<float>>& CpuMassSpringVolume::GetPosition()
{
    return mPosition;
}
//----------------------------------------------------------------------------
void CpuMassSpringVolume::Update(float time)
{
    // Runge-Kutta fourth-order solver.
    float halfTime = time + mHalfStep;
    float fullTime = time + mStep;

    // Compute the first step.
    int c, r, s, i;
    for (s = 0, i = 0; s < mNumSlices; ++s)
    {
        for (r = 0; r < mNumRows; ++r)
        {
            for (c = 0; c < mNumColumns; ++c, ++i)
            {
                if (mInvMass[i] > 0.0f)
                {
                    mPAllTmp[i].d1 = mVelocity[i];
                    mVAllTmp[i].d1 = Acceleration(i, c, r, s, time,
                        mPosition, mVelocity);
                }
            }
        }
    }
    for (s = 0, i = 0; s < mNumSlices; ++s)
    {
        for (r = 0; r < mNumRows; ++r)
        {
            for (c = 0; c < mNumColumns; ++c, ++i)
            {
                if (mInvMass[i] > 0.0f)
                {
                    mPTmp[i] = mPosition[i] + mHalfStep*mPAllTmp[i].d1;
                    mVTmp[i] = mVelocity[i] + mHalfStep*mVAllTmp[i].d1;
                }
                else
                {
                    mPTmp[i] = mPosition[i];
                    mVTmp[i] = Vector3<float>::Zero();
                }
            }
        }
    }

    // Compute the second step.
    for (s = 0, i = 0; s < mNumSlices; ++s)
    {
        for (r = 0; r < mNumRows; ++r)
        {
            for (c = 0; c < mNumColumns; ++c, ++i)
            {
                if (mInvMass[i] > 0.0f)
                {
                    mPAllTmp[i].d2 = mVelocity[i];
                    mVAllTmp[i].d2 = Acceleration(i, c, r, s, halfTime,
                        mPTmp, mVTmp);
                }
            }
        }
    }
    for (s = 0, i = 0; s < mNumSlices; ++s)
    {
        for (r = 0; r < mNumRows; ++r)
        {
            for (c = 0; c < mNumColumns; ++c, ++i)
            {
                if (mInvMass[i] > 0.0f)
                {
                    mPTmp[i] = mPosition[i] + mHalfStep*mPAllTmp[i].d2;
                    mVTmp[i] = mVelocity[i] + mHalfStep*mVAllTmp[i].d2;
                }
                else
                {
                    mPTmp[i] = mPosition[i];
                    mVTmp[i] = Vector3<float>::Zero();
                }
            }
        }
    }

    // Compute the third step.
    for (s = 0, i = 0; s < mNumSlices; ++s)
    {
        for (r = 0; r < mNumRows; ++r)
        {
            for (c = 0; c < mNumColumns; ++c, ++i)
            {
                if (mInvMass[i] > 0.0f)
                {
                    mPAllTmp[i].d3 = mVelocity[i];
                    mVAllTmp[i].d3 = Acceleration(i, c, r, s, halfTime,
                        mPTmp, mVTmp);
                }
            }
        }
    }
    for (s = 0, i = 0; s < mNumSlices; ++s)
    {
        for (r = 0; r < mNumRows; ++r)
        {
            for (c = 0; c < mNumColumns; ++c, ++i)
            {
                if (mInvMass[i] > 0.0f)
                {
                    mPTmp[i] = mPosition[i] + mStep*mPAllTmp[i].d3;
                    mVTmp[i] = mVelocity[i] + mStep*mVAllTmp[i].d3;
                }
                else
                {
                    mPTmp[i] = mPosition[i];
                    mVTmp[i] = Vector3<float>::Zero();
                }
            }
        }
    }

    // Compute the fourth step.
    for (s = 0, i = 0; s < mNumSlices; ++s)
    {
        for (r = 0; r < mNumRows; ++r)
        {
            for (c = 0; c < mNumColumns; ++c, ++i)
            {
                if (mInvMass[i] > 0.0f)
                {
                    mPAllTmp[i].d4 = mVelocity[i];
                    mVAllTmp[i].d4 = Acceleration(i, c, r, s, fullTime,
                        mPTmp, mVTmp);
                }
            }
        }
    }
    for (s = 0, i = 0; s < mNumSlices; ++s)
    {
        for (r = 0; r < mNumRows; ++r)
        {
            for (c = 0; c < mNumColumns; ++c, ++i)
            {
                if (mInvMass[i] > 0.0f)
                {
                    mPosition[i] += mSixthStep*(mPAllTmp[i].d1 +
                        2.0f*(mPAllTmp[i].d2 + mPAllTmp[i].d3) +
                        mPAllTmp[i].d4);
                    mVelocity[i] += mSixthStep*(mVAllTmp[i].d1 +
                        2.0f*(mVAllTmp[i].d2 + mVAllTmp[i].d3) +
                        mVAllTmp[i].d4);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
Vector3<float> CpuMassSpringVolume::Acceleration(int i, int c, int r, int s,
    float, Image3<Vector3<float>> const& position,
    Image3<Vector3<float>> const& velocity)
{
    // Compute spring forces on position i.  The 'position' and 'velocity'
    // inputs are not necessarily mPosition and mVelocity, because the
    // differential euation solver evaluates the acceleration function at
    // intermediate times.  The face, edge, and corner points of the volume of
    // masses must be handled separately, because each has fewer than eight
    // springs attached to it.

    Vector3<float> diff, force;
    float ratio;
    int prev, next;

    Vector3<float> acceleration = -mViscosity*velocity[i];

    if (c > 0)
    {
        prev = i - 1;  // index to previous column
        diff = position[prev] - position[i];
        ratio = mLengthC[prev] / Length(diff);
        force = mConstantC[prev] * (1.0f - ratio)*diff;
        acceleration += mInvMass[i] * force;
    }

    if (c < mNumColumns - 1)
    {
        next = i + 1;  // index to next column
        diff = position[next] - position[i];
        ratio = mLengthC[i] / Length(diff);
        force = mConstantC[i] * (1.0f - ratio)*diff;
        acceleration += mInvMass[i] * force;
    }

    if (r > 0)
    {
        prev = i - mNumColumns;  // index to previous row
        diff = position[prev] - position[i];
        ratio = mLengthR[prev] / Length(diff);
        force = mConstantR[prev] * (1.0f - ratio)*diff;
        acceleration += mInvMass[i] * force;
    }

    if (r < mNumRows - 1)
    {
        next = i + mNumColumns;  // index to next row
        diff = position[next] - position[i];
        ratio = mLengthR[i] / Length(diff);
        force = mConstantR[i] * (1.0f - ratio)*diff;
        acceleration += mInvMass[i] * force;
    }

    if (s > 0)
    {
        prev = i - mNumSliceElements;  // index to previous slice
        diff = position[prev] - position[i];
        ratio = mLengthS[prev] / Length(diff);
        force = mConstantS[prev] * (1.0f - ratio)*diff;
        acceleration += mInvMass[i] * force;
    }

    if (s < mNumSlices - 1)
    {
        next = i + mNumSliceElements;  // index to next slice
        diff = position[next] - position[i];
        ratio = mLengthS[i] / Length(diff);
        force = mConstantS[i] * (1.0f - ratio)*diff;
        acceleration += mInvMass[i] * force;
    }

    return acceleration;
}
//----------------------------------------------------------------------------
