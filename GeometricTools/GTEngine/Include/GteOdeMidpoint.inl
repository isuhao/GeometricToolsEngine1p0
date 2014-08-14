// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Real, typename TVector>
OdeMidpoint<Real,TVector>::~OdeMidpoint()
{
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
OdeMidpoint<Real,TVector>::OdeMidpoint(Real tDelta, Function const& F)
    :
    OdeSolver(tDelta, F)
{
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
void OdeMidpoint<Real, TVector>::Update(Real tIn, TVector const& xIn,
    Real& tOut, TVector& xOut)
{
    // Compute the first step.
    Real halfTDelta = ((Real)0.5) * mTDelta;
    TVector fVector = mFunction(tIn, xIn);
    TVector xTemp = xIn + halfTDelta * fVector;

    // Compute the second step.
    Real halfT = tIn + halfTDelta;
    fVector = mFunction(halfT, xTemp);
    tOut = tIn + mTDelta;
    xOut = xIn + mTDelta * fVector;
}
//----------------------------------------------------------------------------
