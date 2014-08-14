// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GteGVector.h"
#include <functional>

namespace gte
{

// The TVector template parameter allows you to create solvers with
// Vector<N,Real> when the dimension N is known at compile time or
// GVector<Real> when the dimension N is known at run time.  Both classes
// have 'int GetSize() const' that allow OdeSolver-derived classes to query
// for the dimension.
template <typename Real, typename TVector>
class OdeSolver
{
public:
    // The differential equation is dx/dt = F(t,x), where the right-hand
    // side is a function with the following signature.
    typedef std::function<TVector(Real,TVector const&)> Function;

    // Abstract base class.
public:
    virtual ~OdeSolver();
protected:
    OdeSolver(Real tDelta, Function const& F);

public:
    // Member access.
    inline void SetTDelta(Real tDelta);
    inline Real GetTDelta() const;

    // Estimate x(t + tDelta) from x(t) using dx/dt = F(t,x).  The
    // derived classes implement this so that it is possible for xIn and
    // xOut to be the same object.
    virtual void Update(Real tIn, TVector const& xIn, Real& tOut,
        TVector& xOut) = 0;

protected:
    Real mTDelta;
    Function mFunction;
};

#include "GteOdeSolver.inl"

}
