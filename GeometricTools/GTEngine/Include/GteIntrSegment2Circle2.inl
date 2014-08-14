// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Real>
typename TIQuery<Real, Segment2<Real>, Circle2<Real>>::Result
TIQuery<Real, Segment2<Real>, Circle2<Real>>::operator()(
    Segment2<Real> const& segment, Circle2<Real> const& circle)
{
    Result result;
    FIQuery<Real, Segment2<Real>, Circle2<Real>> scQuery;
    result.intersect = scQuery(segment, circle).intersect;
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
typename FIQuery<Real, Segment2<Real>, Circle2<Real>>::Result
FIQuery<Real, Segment2<Real>, Circle2<Real>>::operator()(
    Segment2<Real> const& segment, Circle2<Real> const& circle)
{
    Vector2<Real> segOrigin, segDirection;
    Real segExtent;
    segment.GetCenteredForm(segOrigin, segDirection, segExtent);

    Result result;
    DoQuery(segOrigin, segDirection, segExtent, circle, result);
    for (int i = 0; i < result.numIntersections; ++i)
    {
        result.point[i] = segOrigin + result.parameter[i] * segDirection;
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
void FIQuery<Real, Segment2<Real>, Circle2<Real>>::DoQuery(
    Vector2<Real> const& segOrigin, Vector2<Real> const& segDirection,
    Real segExtent, Circle2<Real> const& circle, Result& result)
{
    FIQuery<Real, Line2<Real>, Circle2<Real>>::DoQuery(segOrigin,
        segDirection, circle, result);

    if (result.intersect)
    {
        // The line containing the segment intersects the disk; the t-interval
        // is [t0,t1].  The segment intersects the disk as long as [t0,t1]
        // overlaps the segment t-interval [-segExtent,+segExtent].
        std::array<Real, 2> segInterval = { -segExtent, segExtent };
        FIQuery<Real, std::array<Real, 2>, std::array<Real, 2>> iiQuery;
        result.parameter = iiQuery(result.parameter, segInterval).overlap;
    }
}
//----------------------------------------------------------------------------
