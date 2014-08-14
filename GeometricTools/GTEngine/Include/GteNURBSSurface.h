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
class NURBSSurface
{
public:
    // Construction and destruction.  This object makes copies of the input
    // arrays.  The 'controls' and 'weights' must be stored in row-major
    // order, attribute[i0 + numControls0*i1].  As a 2D array, this
    // corresponds to attribute2D[i1][i0].
    ~NURBSSurface();
    NURBSSurface(BasisFunctionInput<Real> const input[2],
        Vector<N, Real> const* controls, Real const* weights);

    // To validate construction, create an object as shown:
    //     NURBSSurface<N, Real> surface(parameters);
    //     if (!surface) { <constructor failed, handle accordingly>; }
    inline operator bool() const;

    // Member access.  The index 'dim' must be in {0,1}.
    inline int GetNumControls(int dim) const;
    inline Vector<N, Real> const* GetControls() const;
    inline Real const* GetWeights() const;
    inline BasisFunction<Real> const& GetBasisFunction(int dim) const;
    inline Real GetMinDomain(int dim) const;
    inline Real GetMaxDomain(int dim) const;

    // Evaluation of the surface.  The function supports derivative
    // calculation through order 2; that is, maxOrder <= 2 is required.  If
    // you want only the position, pass in maxOrder of 0.  If you want the
    // position and first-order derivatives, pass in maxOrder of 1, and so on.
    // The output 'values' are ordered as: position X; first-order derivatives
    // dX/du, dX/dv; second-order derivatives d2X/du2, d2X/dudv, d2X/dv2.
    void Evaluate(Real u, Real v, unsigned int maxOrder,
        Vector<N, Real> values[6]) const;

private:
    // Support for Evaluate(...).
    void Compute(unsigned int uOrder, unsigned int vOrder, int iumin,
        int iumax, int ivmin, int ivmax, Vector<N, Real>& X, Real& w) const;

    std::array<BasisFunction<Real>, 2> mBasisFunction;
    std::array<int, 2> mNumControls;
    std::vector<Vector<N, Real>> mControls;
    std::vector<Real> mWeights;
    bool mConstructed;
};

#include "GteNURBSSurface.inl"

}
