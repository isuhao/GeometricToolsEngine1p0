// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <int N, typename Real>
TCBSplineCurve<N, Real>::~TCBSplineCurve()
{
}
//----------------------------------------------------------------------------
template <int N, typename Real>
TCBSplineCurve<N, Real>::TCBSplineCurve(int numPoints,
    Vector<N, Real> const* points, Real const* times, Real const* tension,
    Real const* continuity, Real const* bias)
    :
    mNumPoints(numPoints),
    mConstructed(false)
{
    if (numPoints < 2 || !points || !times)
    {
        LogError("Invalid input.");
        return;
    }

    mPoints.resize(numPoints);
    mTimes.resize(numPoints);
    mTension.resize(numPoints);
    mContinuity.resize(numPoints);
    mBias.resize(numPoints);
    std::copy(points, points + numPoints, mPoints.begin());
    std::copy(times, times + numPoints, mTimes.begin());
    std::copy(tension, tension + numPoints, mTension.begin());
    std::copy(continuity, continuity + numPoints, mContinuity.begin());
    std::copy(bias, bias + numPoints, mBias.begin());

    int numSegments = numPoints - 1;
    mA.resize(numSegments);
    mB.resize(numSegments);
    mC.resize(numSegments);
    mD.resize(numSegments);

    // For now, treat the first point as if it occurred twice.
    ComputePoly(0, 0, 1, 2);

    for (int i = 1; i < numSegments - 1; ++i)
    {
        ComputePoly(i - 1, i, i + 1, i + 2);
    }

    // For now, treat the last point as if it occurred twice.
    ComputePoly(numSegments - 2, numSegments - 1, numSegments, numSegments);

    mConstructed = true;
}
//----------------------------------------------------------------------------
template <int N, typename Real> inline
TCBSplineCurve<N, Real>::operator bool() const
{
    return mConstructed;
}
//----------------------------------------------------------------------------
template <int N, typename Real> inline
int TCBSplineCurve<N, Real>::GetNumPoints() const
{
    return mNumPoints;
}
//----------------------------------------------------------------------------
template <int N, typename Real> inline
Vector<N, Real> const* TCBSplineCurve<N, Real>::GetPoints() const
{
    return &mPoints[0];
}
//----------------------------------------------------------------------------
template <int N, typename Real> inline
Real const* TCBSplineCurve<N, Real>::GetTimes() const
{
    return &mTimes[0];
}
//----------------------------------------------------------------------------
template <int N, typename Real> inline
Real const* TCBSplineCurve<N, Real>::GetTensions() const
{
    return &mTension[0];
}
//----------------------------------------------------------------------------
template <int N, typename Real> inline
Real const* TCBSplineCurve<N, Real>::GetContinuities() const
{
    return &mContinuity[0];
}
//----------------------------------------------------------------------------
template <int N, typename Real> inline
Real const* TCBSplineCurve<N, Real>::GetBiases() const
{
    return &mBias[0];
}
//----------------------------------------------------------------------------
template <int N, typename Real>
void TCBSplineCurve<N, Real>::Evaluate(Real t, unsigned int maxOrder,
    Vector<N, Real> values[4]) const
{
    if (!mConstructed)
    {
        // Errors were already generated during construction.
        for (unsigned int order = 0; order < 4; ++order)
        {
            values[order].MakeZero();
        }
        return;
    }

    int key;
    Real dt;
    GetKeyInfo(t, key, dt);
    dt /= (mTimes[key + 1] - mTimes[key]);

    // Compute position.
    values[0] = mA[key] + dt * (mB[key] + dt * (mC[key] + dt * mD[key]));
    if (maxOrder >= 1)
    {
        // Compute first derivative.
        values[1] = mB[key] + dt * (mC[key] * ((Real)2) + mD[key] *
            (((Real)3) * dt));
        if (maxOrder >= 2)
        {
            // Compute second derivative.
            values[2] = mC[key] * ((Real)2) + mD[key] * (((Real)6) * dt);
            if (maxOrder == 3)
            {
                values[3] = ((Real)6) * mD[key];
            }
            else
            {
                values[3].MakeZero();
            }
        }
    }
}
//----------------------------------------------------------------------------
template <int N, typename Real>
void TCBSplineCurve<N, Real>::ComputePoly(int i0, int i1, int i2, int i3)
{
    Vector<N, Real> diff = mPoints[i2] - mPoints[i1];
    Real dt = mTimes[i2] - mTimes[i1];

    // Build multipliers at P1.
    Real oneMinusT0 = (Real)1 - mTension[i1];
    Real oneMinusC0 = (Real)1 - mContinuity[i1];
    Real onePlusC0 = (Real)1 + mContinuity[i1];
    Real oneMinusB0 = (Real)1 - mBias[i1];
    Real onePlusB0 = (Real)1 + mBias[i1];
    Real adj0 = ((Real)2)*dt / (mTimes[i2] - mTimes[i0]);
    Real out0 = ((Real)0.5)*adj0*oneMinusT0*onePlusC0*onePlusB0;
    Real out1 = ((Real)0.5)*adj0*oneMinusT0*oneMinusC0*oneMinusB0;

    // Build outgoing tangent at P1.
    Vector<N, Real> tOut = out1*diff + out0*(mPoints[i1] - mPoints[i0]);

    // Build multipliers at point P2.
    Real oneMinusT1 = (Real)1 - mTension[i2];
    Real oneMinusC1 = (Real)1 - mContinuity[i2];
    Real onePlusC1 = (Real)1 + mContinuity[i2];
    Real oneMinusB1 = (Real)1 - mBias[i2];
    Real onePlusB1 = (Real)1 + mBias[i2];
    Real adj1 = ((Real)2)*dt / (mTimes[i3] - mTimes[i1]);
    Real in0 = ((Real)0.5)*adj1*oneMinusT1*oneMinusC1*onePlusB1;
    Real in1 = ((Real)0.5)*adj1*oneMinusT1*onePlusC1*oneMinusB1;

    // Build incoming tangent at P2.
    Vector<N, Real> tIn = in1*(mPoints[i3] - mPoints[i2]) + in0*diff;

    mA[i1] = mPoints[i1];
    mB[i1] = tOut;
    mC[i1] = ((Real)3)*diff - ((Real)2)*tOut - tIn;
    mD[i1] = ((Real)-2)*diff + tOut + tIn;
}
//----------------------------------------------------------------------------
template <int N, typename Real>
void TCBSplineCurve<N, Real>::GetKeyInfo(Real t, int& key, Real& dt) const
{
    int numSegments = mNumPoints - 1;
    if (t <= mTimes[0])
    {
        key = 0;
        dt = (Real)0;
    }
    else if (t >= mTimes[numSegments])
    {
        key = numSegments - 1;
        dt = mTimes[numSegments] - mTimes[numSegments - 1];
    }
    else
    {
        for (int i = 0; i < numSegments; ++i)
        {
            if (t < mTimes[i + 1])
            {
                key = i;
                dt = t - mTimes[i];
                break;
            }
        }
    }
}
//----------------------------------------------------------------------------
