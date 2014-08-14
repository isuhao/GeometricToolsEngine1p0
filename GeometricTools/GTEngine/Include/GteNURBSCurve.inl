// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <int N, typename Real>
NURBSCurve<N, Real>::~NURBSCurve()
{
}
//----------------------------------------------------------------------------
template <int N, typename Real>
NURBSCurve<N, Real>::NURBSCurve(BasisFunctionInput<Real> const& input,
    Vector<N, Real> const* controls, Real const* weights)
    :
    mBasisFunction(input),
    mConstructed(false)
{
    if (!controls || !weights)
    {
        LogError("Invalid controls or weights pointer.");
        return;
    }

    if (!mBasisFunction)
    {
        // Errors were already generated during construction of the
        // basis function.
        return;
    }

    // The replication of control points for periodic splines is avoided
    // by wrapping the i-loop index in Evaluate.
    mControls.resize(input.numControls);
    mWeights.resize(input.numControls);
    std::copy(controls, controls + input.numControls, mControls.begin());
    std::copy(weights, weights + input.numControls, mWeights.begin());
    mConstructed = true;
}
//----------------------------------------------------------------------------
template <int N, typename Real>
NURBSCurve<N, Real>::operator bool() const
{
    return mConstructed;
}
//----------------------------------------------------------------------------
template <int N, typename Real>
int NURBSCurve<N, Real>::GetNumControls() const
{
    return static_cast<int>(mControls.size());
}
//----------------------------------------------------------------------------
template <int N, typename Real>
Vector<N, Real> const* NURBSCurve<N, Real>::GetControls() const
{
    return &mControls[0];
}
//----------------------------------------------------------------------------
template <int N, typename Real>
Real const* NURBSCurve<N, Real>::GetWeights() const
{
    return &mWeights[0];
}
//----------------------------------------------------------------------------
template <int N, typename Real>
BasisFunction<Real> const& NURBSCurve<N, Real>::GetBasisFunction() const
{
    return mBasisFunction;
}
//----------------------------------------------------------------------------
template <int N, typename Real>
Real NURBSCurve<N, Real>::GetMinDomain() const
{
    return mBasisFunction.GetMinDomain();
}
//----------------------------------------------------------------------------
template <int N, typename Real>
Real NURBSCurve<N, Real>::GetMaxDomain() const
{
    return mBasisFunction.GetMaxDomain();
}
//----------------------------------------------------------------------------
template <int N, typename Real>
void NURBSCurve<N, Real>::Evaluate(Real t, unsigned int maxOrder,
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

    int imin, imax;
    mBasisFunction.Evaluate(t, maxOrder, imin, imax);

    // Compute position.
    Vector<N, Real> X;
    Real w;
    Compute(0, imin, imax, X, w);
    Real invW = ((Real)1) / w;
    values[0] = invW * X;

    if (maxOrder >= 1)
    {
        // Compute first derivative.
        Vector<N, Real> XDer1;
        Real wDer1;
        Compute(1, imin, imax, XDer1, wDer1);
        values[1] = invW * (XDer1 - wDer1 * values[0]);

        if (maxOrder >= 2)
        {
            // Compute second derivative.
            Vector<N, Real> XDer2;
            Real wDer2;
            Compute(2, imin, imax, XDer2, wDer2);
            values[2] = invW * (XDer2 - ((Real)2) * wDer1 * values[1] -
                wDer2 * values[0]);

            if (maxOrder == 3)
            {
                // Compute third derivative.
                Vector<N, Real> XDer3;
                Real wDer3;
                Compute(3, imin, imax, XDer3, wDer3);
                values[3] = invW * (XDer3 - ((Real)3) * wDer1 * values[2] -
                    ((Real)3) * wDer2 * values[1] - wDer3 * values[0]);
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
void NURBSCurve<N, Real>::Compute(unsigned int order, int imin, int imax,
    Vector<N, Real>& X, Real& w) const
{
    // The j-index introduces a tiny amount of overhead in order to handle
    // both aperiodic and periodic splines.  For aperiodic splines, j = i
    // always.

    int numControls = GetNumControls();
    X.MakeZero();
    w = (Real)0;
    for (int i = imin; i <= imax; ++i)
    {
        int j = (i >= numControls ? i - numControls : i);
        Real tmp = mBasisFunction.GetValue(order, i) * mWeights[j];
        X += tmp * mControls[j];
        w += tmp;
    }
}
//----------------------------------------------------------------------------
