// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Real>
Minimize1<Real>::Minimize1(std::function<Real(Real)> const& F, int maxLevel,
    int maxBracket)
    :
    mFunction(F),
    mMaxLevel(maxLevel),
    mMaxBracket(maxBracket)
{
}
//----------------------------------------------------------------------------
template <typename Real>
void Minimize1<Real>::GetMinimum(Real t0, Real t1, Real tInitial,
    Real& tMin, Real& fMin)
{
    LogAssert(t0 <= tInitial && tInitial <= t1, "Invalid initial t value.");

    mTMin = std::numeric_limits<Real>::max();
    mFMin = std::numeric_limits<Real>::max();

    Real f0 = mFunction(t0);
    if (f0 < mFMin)
    {
        mTMin = t0;
        mFMin = f0;
    }

    Real fInitial = mFunction(tInitial);
    if (fInitial < mFMin)
    {
        mTMin = tInitial;
        mFMin = fInitial;
    }

    Real f1 = mFunction(t1);
    if (f1 < mFMin)
    {
        mTMin = t1;
        mFMin = f1;
    }

    GetMinimum(t0, f0, tInitial, fInitial, t1, f1, mMaxLevel);

    tMin = mTMin;
    fMin = mFMin;
}
//----------------------------------------------------------------------------
template <typename Real>
void Minimize1<Real>::GetMinimum(Real t0, Real f0, Real tm, Real fm, Real t1,
    Real f1, int level)
{
    if (level-- == 0)
    {
        return;
    }

    if ((t1 - tm)*(f0 - fm) > (tm - t0)*(fm - f1))
    {
        // The quadratic fit has positive second derivative at the midpoint.
        if (f1 > f0)
        {
            if (fm >= f0)
            {
                // Increasing, repeat on [t0,tm].
                GetMinimum(t0, f0, tm, fm, level);
            }
            else
            {
                // Not monotonic, have a bracket.
                GetBracketedMinimum(t0, f0, tm, fm, t1, f1, level);
            }
        }
        else if (f1 < f0)
        {
            if (fm >= f1)
            {
                // Decreasing, repeat on [tm,t1].
                GetMinimum(tm, fm, t1, f1, level);
            }
            else
            {
                // Not monotonic, have a bracket.
                GetBracketedMinimum(t0, f0, tm, fm, t1, f1, level);
            }
        }
        else
        {
            // Constant, repeat on [t0,tm] and [tm,t1].
            GetMinimum(t0, f0, tm, fm, level);
            GetMinimum(tm, fm, t1, f1, level);
        }
    }
    else
    {
        // The quadratic fit has a nonpositive second derivative at the
        // midpoint.
        if (f1 > f0)
        {
            // Repeat on [t0,tm].
            GetMinimum(t0, f0, tm, fm, level);
        }
        else if ( f1 < f0 )
        {
            // Repeat on [tm,t1].
            GetMinimum(tm, fm, t1, f1, level);
        }
        else
        {
            // Repeat on [t0,tm] and [tm,t1].
            GetMinimum(t0, f0, tm, fm, level);
            GetMinimum(tm, fm, t1, f1, level);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Minimize1<Real>::GetMinimum(Real t0, Real f0, Real t1, Real f1,
    int level)
{
    if (level-- == 0)
    {
        return;
    }

    Real tm = ((Real)0.5)*(t0 + t1);
    Real fm = mFunction(tm);
    if (fm < mFMin)
    {
        mTMin = tm;
        mFMin = fm;
    }

    if (f0 - ((Real)2)*fm + f1 > (Real)0)
    {
        // The quadratic fit has positive second derivative at the midpoint.
        if (f1 > f0)
        {
            if (fm >= f0)
            {
                // Increasing, repeat on [t0,tm].
                GetMinimum(t0, f0, tm, fm, level);
            }
            else
            {
                // Not monotonic, have a bracket.
                GetBracketedMinimum(t0, f0, tm, fm, t1, f1, level);
            }
        }
        else if (f1 < f0)
        {
            if (fm >= f1)
            {
                // Decreasing, repeat on [tm,t1].
                GetMinimum(tm, fm, t1, f1, level);
            }
            else
            {
                // Not monotonic, have a bracket.
                GetBracketedMinimum(t0, f0, tm, fm, t1, f1, level);
            }
        }
        else
        {
            // Constant, repeat on [t0,tm] and [tm,t1].
            GetMinimum(t0, f0, tm, fm, level);
            GetMinimum(tm, fm, t1, f1, level);
        }
    }
    else
    {
        // The quadratic fit has nonpositive second derivative at the
        // midpoint.
        if (f1 > f0)
        {
            // Repeat on [t0,tm].
            GetMinimum(t0, f0, tm, fm, level);
        }
        else if (f1 < f0)
        {
            // Repeat on [tm,t1].
            GetMinimum(tm, fm, t1, f1, level);
        }
        else
        {
            // Repeat on [t0,tm] and [tm,t1].
            GetMinimum(t0, f0, tm, fm, level);
            GetMinimum(tm, fm, t1, f1, level);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Minimize1<Real>::GetBracketedMinimum(Real t0, Real f0, Real tm,
    Real fm, Real t1, Real f1, int level)
{
    for (int i = 0; i < mMaxBracket; ++i)
    {
        // Update minimum value.
        if (fm < mFMin)
        {
            mTMin = tm;
            mFMin = fm;
        }

        // Test for convergence.  TODO: Expose the epsilon and tolerance
        // parameters to the caller.
        Real const epsilon = (Real)1e-08;
        Real const tolerance = (Real)1e-04;
        if (std::abs(t1-t0) <= ((Real)2)*tolerance*std::abs(tm) + epsilon)
        {
            break;
        }

        // Compute vertex of interpolating parabola.
        Real dt0 = t0 - tm;
        Real dt1 = t1 - tm;
        Real df0 = f0 - fm;
        Real df1 = f1 - fm;
        Real tmp0 = dt0*df1;
        Real tmp1 = dt1*df0;
        Real denom = tmp1 - tmp0;
        if (std::abs(denom) < epsilon)
        {
           return;
        }

        Real tv = tm + ((Real)0.5)*(dt1*tmp1 - dt0*tmp0)/denom;
        LogAssert(t0 <= tv && tv <= t1, "Vertex not in interval.");
        Real fv = mFunction(tv);
        if (fv < mFMin)
        {
            mTMin = tv;
            mFMin = fv;
        }

        if (tv < tm)
        {
            if (fv < fm)
            {
                t1 = tm;
                f1 = fm;
                tm = tv;
                fm = fv;
            }
            else
            {
                t0 = tv;
                f0 = fv;
            }
        }
        else if (tv > tm)
        {
            if (fv < fm)
            {
                t0 = tm;
                f0 = fm;
                tm = tv;
                fm = fv;
            }
            else
            {
                t1 = tv;
                f1 = fv;
            }
        }
        else
        {
            // The vertex of parabola is already at middle sample point.
            GetMinimum(t0, f0, tm, fm, level);
            GetMinimum(tm, fm, t1, f1, level);
        }
    }
}
//----------------------------------------------------------------------------
