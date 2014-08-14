// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Real, typename TVector, typename TMatrix>
OdeImplicitEuler<Real,TVector,TMatrix>::~OdeImplicitEuler()
{
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector, typename TMatrix>
OdeImplicitEuler<Real,TVector,TMatrix>::OdeImplicitEuler(Real tDelta,
    Function const& F, DerivativeFunction const& DF)
    :
    OdeSolver(tDelta, F),
    mDerivativeFunction(DF)
{
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector, typename TMatrix>
void OdeImplicitEuler<Real,TVector,TMatrix>::Update(Real tIn,
    TVector const& xIn, Real& tOut, TVector& xOut)
{
    TVector fVector = mFunction(tIn, xIn);
    TMatrix dfMatrix = mDerivativeFunction(tIn, xIn);
    TMatrix identity;
    MakeIdentity(identity);
    TMatrix dgMatrix = identity - mTDelta * dfMatrix;
    TMatrix dgInverse = Inverse(dgMatrix);
    fVector = dgInverse * fVector;
    tOut = tIn + mTDelta;
    xOut = xIn + mTDelta * fVector;
}
//----------------------------------------------------------------------------
