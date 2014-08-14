// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GteBasisFunction.h"
#include "GteVector.h"

namespace gte
{

template <int N, typename Real>
class NURBSCurve
{
public:
    // Construction and destruction.  This object makes copies of the input
    // arrays.
    ~NURBSCurve();
    NURBSCurve(BasisFunctionInput<Real> const& input,
        Vector<N, Real> const* controls, Real const* weights);

    // To validate construction, create an object as shown:
    //     NURBSCurve<N, Real> curve(parameters);
    //     if (!curve) { <constructor failed, handle accordingly>; }
    inline operator bool() const;

    // Member access.
    inline int GetNumControls() const;
    inline Vector<N, Real> const* GetControls() const;
    inline Real const* GetWeights() const;
    inline BasisFunction<Real> const& GetBasisFunction() const;
    inline Real GetMinDomain() const;
    inline Real GetMaxDomain() const;

    // Evaluation of the curve.  The function supports derivative calculation
    // through order 3; that is, maxOrder <= 3 is required.  If you want
    // only the position, pass in maxOrder of 0.  If you want the position and
    // first derivative, pass in maxOrder of 1, and so on.  The output
    // 'values' are ordered as: position, first derivative, second derivative,
    // third derivative.
    void Evaluate(Real t, unsigned int maxOrder, Vector<N, Real> values[4])
        const;

private:
    // Support for Evaluate(...).
    void Compute(unsigned int order, int imin, int imax, Vector<N, Real>& X,
        Real& w) const;

    BasisFunction<Real> mBasisFunction;
    std::vector<Vector<N, Real>> mControls;
    std::vector<Real> mWeights;
    bool mConstructed;
};

#include "GteNURBSCurve.inl"

}
