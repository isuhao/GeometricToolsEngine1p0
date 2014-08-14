// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <int N, typename Real>
NaturalSplineCurve<N, Real>::~NaturalSplineCurve()
{
}
//----------------------------------------------------------------------------
template <int N, typename Real>
NaturalSplineCurve<N, Real>::NaturalSplineCurve(bool isFree, int numPoints,
    Vector<N, Real> const* points, Real const* times)
    :
    mConstructed(false)
{
    if (numPoints < 2 || !points || !times)
    {
        LogError("Invalid input.");
        return;
    }

    mTimes.resize(numPoints);
    mA.resize(numPoints);
    std::copy(times, times + numPoints, mTimes.begin());
    std::copy(points, points + numPoints, mA.begin());

    if (isFree)
    {
        CreateFree();
    }
    else
    {
        CreateClosed();
    }

    mConstructed = true;
}
//----------------------------------------------------------------------------
template <int N, typename Real>
NaturalSplineCurve<N, Real>::NaturalSplineCurve(int numPoints,
    Vector<N, Real> const* points, Real const* times,
    Vector<N, Real> const& derivative0, Vector<N, Real> const& derivative1)
    :
    mConstructed(false)
{
    if (numPoints < 2 || !points || !times)
    {
        LogError("Invalid input.");
        return;
    }

    mTimes.resize(numPoints);
    mA.resize(numPoints);
    std::copy(times, times + numPoints, mTimes.begin());
    std::copy(points, points + numPoints, mA.begin());

    CreateClamped(derivative0, derivative1);
    mConstructed = true;
}
//----------------------------------------------------------------------------
template <int N, typename Real> inline
NaturalSplineCurve<N, Real>::operator bool() const
{
    return mConstructed;
}
//----------------------------------------------------------------------------
template <int N, typename Real> inline
int NaturalSplineCurve<N, Real>::GetNumPoints() const
{
    return static_cast<int>(mA.size());
}
//----------------------------------------------------------------------------
template <int N, typename Real> inline
Vector<N, Real> const* NaturalSplineCurve<N, Real>::GetPoints() const
{
    return &mA[0];
}
//----------------------------------------------------------------------------
template <int N, typename Real> inline
Real const* NaturalSplineCurve<N, Real>::GetTimes() const
{
    return &mTimes[0];
}
//----------------------------------------------------------------------------
template <int N, typename Real>
void NaturalSplineCurve<N, Real>::Evaluate(Real t, unsigned int maxOrder,
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

    // Compute position.
    values[0] = mA[key] + dt * (mB[key] + dt * (mC[key] + dt * mD[key]));
    if (maxOrder >= 1)
    {
        // Compute first derivative.
        values[1] = mB[key] + dt * (((Real)2)  *mC[key] +
            ((Real)3) * dt * mD[key]);
        if (maxOrder >= 2)
        {
            // Compute second derivative.
            values[2] = ((Real)2)  *mC[key] + ((Real)6) * dt * mD[key];
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
void NaturalSplineCurve<N, Real>::CreateFree()
{
    int numSegments = GetNumPoints() - 1;
    std::vector<Real> dt(numSegments);
    for (int i = 0; i < numSegments; ++i)
    {
        dt[i] = mTimes[i + 1] - mTimes[i];
    }

    std::vector<Real> d2t(numSegments);
    for (int i = 1; i < numSegments; ++i)
    {
        d2t[i] = mTimes[i + 1] - mTimes[i - 1];
    }

    std::vector<Vector<N, Real>> alpha(numSegments);
    for (int i = 1; i < numSegments; ++i)
    {
        Vector<N, Real> numer = ((Real)3)*(dt[i - 1] * mA[i + 1] -
            d2t[i] * mA[i] + dt[i] * mA[i - 1]);
        Real invDenom = ((Real)1) / (dt[i - 1] * dt[i]);
        alpha[i] = invDenom * numer;
    }

    std::vector<Real> ell(numSegments + 1);
    std::vector<Real> mu(numSegments);
    std::vector<Vector<N, Real>> z(numSegments + 1);
    Real inv;

    ell[0] = (Real)1;
    mu[0] = (Real)0;
    z[0].MakeZero();
    for (int i = 1; i < numSegments; ++i)
    {
        ell[i] = ((Real)2) * d2t[i] - dt[i - 1] * mu[i - 1];
        inv = ((Real)1) / ell[i];
        mu[i] = inv * dt[i];
        z[i] = inv * (alpha[i] - dt[i - 1] * z[i - 1]);
    }
    ell[numSegments] = (Real)1;
    z[numSegments].MakeZero();

    mB.resize(numSegments);
    mC.resize(numSegments + 1);
    mD.resize(numSegments);

    Real oneThird = ((Real)1) / (Real)3;
    mC[numSegments].MakeZero();
    for (int i = numSegments - 1; i >= 0; --i)
    {
        mC[i] = z[i] - mu[i] * mC[i + 1];
        inv = ((Real)1) / dt[i];
        mB[i] = inv * (mA[i + 1] - mA[i]) - oneThird * dt[i] * (mC[i + 1] +
            ((Real)2) * mC[i]);
        mD[i] = oneThird * inv * (mC[i + 1] - mC[i]);
    }
}
//----------------------------------------------------------------------------
template <int N, typename Real>
void NaturalSplineCurve<N, Real>::CreateClosed()
{
    // TODO:  A general linear system solver is used here.  The matrix
    // corresponding to this case is actually "cyclic banded", so a faster
    // linear solver can be used.  The current linear system code does not
    // have such a solver.

    int numSegments = GetNumPoints() - 1;
    std::vector<Real> dt(numSegments);
    for (int i = 0; i < numSegments; ++i)
    {
        dt[i] = mTimes[i + 1] - mTimes[i];
    }

    // Construct matrix of system.
    GMatrix<Real> mat(numSegments + 1, numSegments + 1);
    mat(0, 0) = (Real)1;
    mat(0, numSegments) = (Real)-1;
    for (int i = 1; i <= numSegments - 1; ++i)
    {
        mat(i, i - 1) = dt[i - 1];
        mat(i, i) = ((Real)2) * (dt[i - 1] + dt[i]);
        mat(i, i + 1) = dt[i];
    }
    mat(numSegments, numSegments - 1) = dt[numSegments - 1];
    mat(numSegments, 0) = ((Real)2) * (dt[numSegments - 1] + dt[0]);
    mat(numSegments, 1) = dt[0];

    // Construct right-hand side of system.
    mC.resize(numSegments + 1);
    mC[0].MakeZero();
    Real inv0, inv1;
    for (int i = 1; i <= numSegments - 1; ++i)
    {
        inv0 = ((Real)1) / dt[i];
        inv1 = ((Real)1) / dt[i - 1];
        mC[i] = ((Real)3) * (inv0 * (mA[i + 1] - mA[i]) -
            inv1*(mA[i] - mA[i - 1]));
    }
    inv0 = ((Real)1) / dt[0];
    inv1 = ((Real)1) / dt[numSegments - 1];
    mC[numSegments] = ((Real)3) * (inv0 * (mA[1] - mA[0]) -
        inv1 * (mA[0] - mA[numSegments - 1]));

    // Solve the linear systems.
    GMatrix<Real> invMat = Inverse(mat);
    GVector<Real> input(numSegments + 1);
    GVector<Real> output(numSegments + 1);

    for (int i = 0; i <= numSegments; ++i)
    {
        input[i] = mC[i][0];
    }
    output = invMat * input;
    for (int i = 0; i <= numSegments; ++i)
    {
        mC[i][0] = output[i];
    }

    for (int i = 0; i <= numSegments; ++i)
    {
        input[i] = mC[i][1];
    }
    output = invMat * input;
    for (int i = 0; i <= numSegments; ++i)
    {
        mC[i][1] = output[i];
    }

    for (int i = 0; i <= numSegments; ++i)
    {
        input[i] = mC[i][2];
    }
    output = invMat * input;
    for (int i = 0; i <= numSegments; ++i)
    {
        mC[i][2] = output[i];
    }
    // End linear system solving.

    Real oneThird = ((Real)1) / (Real)3;
    mB.resize(numSegments);
    mD.resize(numSegments);
    for (int i = 0; i < numSegments; ++i)
    {
        inv0 = ((Real)1) / dt[i];
        mB[i] = inv0 * (mA[i + 1] - mA[i]) - oneThird * (mC[i + 1] +
            ((Real)2) * mC[i]) * dt[i];
        mD[i] = oneThird * inv0 * (mC[i + 1] - mC[i]);
    }
}
//----------------------------------------------------------------------------
template <int N, typename Real>
void NaturalSplineCurve<N, Real>::CreateClamped(
    Vector<N, Real> const& derivative0, Vector<N, Real> const& derivative1)
{
    int numSegments = GetNumPoints() - 1;
    std::vector<Real> dt(numSegments);
    for (int i = 0; i < numSegments; ++i)
    {
        dt[i] = mTimes[i + 1] - mTimes[i];
    }

    std::vector<Real> d2t(numSegments);
    for (int i = 1; i < numSegments; ++i)
    {
        d2t[i] = mTimes[i + 1] - mTimes[i - 1];
    }

    std::vector<Vector<N, Real>> alpha(numSegments + 1);
    Real inv = ((Real)1) / dt[0];
    alpha[0] = ((Real)3) * (inv * (mA[1] - mA[0]) - derivative0);
    inv = ((Real)1) / dt[numSegments - 1];
    alpha[numSegments] = ((Real)3)*(derivative1 -
        inv * (mA[numSegments] - mA[numSegments - 1]));
    for (int i = 1; i < numSegments; ++i)
    {
        Vector<N, Real> numer = ((Real)3)*(dt[i - 1] * mA[i + 1] -
            d2t[i] * mA[i] + dt[i] * mA[i - 1]);
        Real invDenom = ((Real)1) / (dt[i - 1] * dt[i]);
        alpha[i] = invDenom*numer;
    }

    std::vector<Real> ell(numSegments + 1);
    std::vector<Real> mu(numSegments);
    std::vector<Vector<N, Real>> z(numSegments + 1);

    ell[0] = ((Real)2) * dt[0];
    mu[0] = (Real)0.5;
    inv = ((Real)1) / ell[0];
    z[0] = inv * alpha[0];

    for (int i = 1; i < numSegments; ++i)
    {
        ell[i] = ((Real)2) * d2t[i] - dt[i - 1] * mu[i - 1];
        inv = ((Real)1) / ell[i];
        mu[i] = inv * dt[i];
        z[i] = inv * (alpha[i] - dt[i - 1] * z[i - 1]);
    }
    ell[numSegments] = dt[numSegments - 1] *
        (((Real)2) - mu[numSegments - 1]);
    inv = ((Real)1) / ell[numSegments];
    z[numSegments] = inv * (alpha[numSegments] - dt[numSegments - 1] *
        z[numSegments - 1]);

    mB.resize(numSegments);
    mC.resize(numSegments + 1);
    mD.resize(numSegments);

    Real oneThird = ((Real)1) / (Real)3;
    mC[numSegments] = z[numSegments];
    for (int i = numSegments - 1; i >= 0; --i)
    {
        mC[i] = z[i] - mu[i] * mC[i + 1];
        inv = ((Real)1) / dt[i];
        mB[i] = inv * (mA[i + 1] - mA[i]) - oneThird*dt[i] * (mC[i + 1] +
            ((Real)2) * mC[i]);
        mD[i] = oneThird * inv * (mC[i + 1] - mC[i]);
    }
}
//----------------------------------------------------------------------------
template <int N, typename Real>
void NaturalSplineCurve<N, Real>::GetKeyInfo(Real t, int& key, Real& dt) const
{
    int numSegments = GetNumPoints() - 1;
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
