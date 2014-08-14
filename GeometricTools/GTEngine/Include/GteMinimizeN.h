// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GteMinimize1.h"
#include "GteGVector.h"

// The Cartesian-product domain provided to GetMinimum(*) has minimum values
// stored in t0[0..d-1] and maximum values stored in t1[0..d-1], where d is
// 'dimensions'.  The domain is searched along lines through the current
// estimate of the minimum location.  Each such line is searched for a minimum
// using a Minimize1<Real> object.  This is called "Powell's Direction Set
// Method".  The parameters 'maxLevel' and 'maxBracket' are used by
// Minimize1<Real>, so read the documentation for that class (in its header
// file) to understand what these mean.  The input 'maxIterations' is the
// number of iterations for the direction-set method.

namespace gte
{

template <typename Real>
class MinimizeN
{
public:
    // Construction.
    MinimizeN(int dimensions, std::function<Real(Real const*)> const& F,
        int maxLevel, int maxBracket, int maxIterations);

    // Find the minimum on the Cartesian-product domain whose minimum values
    // are stored in t0[0..d-1] and whose maximum values are stored in
    // t1[0..d-1], where d is 'dimensions'.  An initial guess is specified in
    // tInitial[0..d-1].  The location of the minimum is tMin[0..d-1] and
    // the value of the minimum is 'fMin'.
    void GetMinimum(Real const* t0, Real const* t1, Real const* tInitial,
        Real* tMin, Real& fMin);

private:
    // The current estimate of the minimum location is mTCurr[0..d-1].  The
    // direction of the current line to search is mDCurr[0..d-1].  This line
    // must be clipped against the Cartesian-product domain, a process
    // implemented in this function.  If the line is mTCurr+s*mDCurr, the
    // clip result is the s-interval [ell0,ell1].
    void ComputeDomain(Real const* t0, Real const* t1, Real& ell0,
        Real& ell1);

    int mDimensions;
    std::function<Real(Real const*)> mFunction;
    int mMaxIterations;
    std::vector<GVector<Real>> mDirections;
    int mDConjIndex;
    int mDCurrIndex;
    GVector<Real> mTCurr;
    GVector<Real> mTSave;
    Real mFCurr;
    Minimize1<Real> mMinimizer;
};

#include "GteMinimizeN.inl"

}
