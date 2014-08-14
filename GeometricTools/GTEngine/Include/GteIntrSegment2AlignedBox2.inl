// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Real>
typename TIQuery<Real, Segment2<Real>, AlignedBox2<Real>>::Result
TIQuery<Real, Segment2<Real>, AlignedBox2<Real>>::operator()(
    Segment2<Real> const& segment, AlignedBox2<Real> const& box)
{
    // Get the centered form of the aligned box.  The axes are implicitly
    // Axis[d] = Vector2<Real>::Unit(d).
    Vector2<Real> boxCenter, boxExtent;
    box.GetCenteredForm(boxCenter, boxExtent);

    // Transform the segment to a centered form in the aligned-box coordinate
    // system.
    Vector2<Real> transformedP0 = segment.p[0] - boxCenter;
    Vector2<Real> transformedP1 = segment.p[1] - boxCenter;
    Segment2<Real> transformedSegment(transformedP0, transformedP1);
    Vector2<Real> segOrigin, segDirection;
    Real segExtent;
    transformedSegment.GetCenteredForm(segOrigin, segDirection, segExtent);

    Result result;
    DoQuery(segOrigin, segDirection, segExtent, boxExtent, result);
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
void TIQuery<Real, Segment2<Real>, AlignedBox2<Real>>::DoQuery(
    Vector2<Real> const& segOrigin, Vector2<Real> const& segDirection,
    Real segExtent, Vector2<Real> const& boxExtent, Result& result)
{
    for (int i = 0; i < 2; ++i)
    {
        if (std::abs(segOrigin[i]) > boxExtent[i] +
            segExtent*std::abs(segDirection[i]))
        {
            result.intersect = false;
            return;
        }
    }

    TIQuery<Real, Line2<Real>, AlignedBox2<Real>>::DoQuery(segOrigin,
        segDirection, boxExtent, result);
}
//----------------------------------------------------------------------------
template <typename Real>
typename FIQuery<Real, Segment2<Real>, AlignedBox2<Real>>::Result
FIQuery<Real, Segment2<Real>, AlignedBox2<Real>>::operator()(
    Segment2<Real> const& segment, AlignedBox2<Real> const& box)
{
    // Get the centered form of the aligned box.  The axes are implicitly
    // Axis[d] = Vector2<Real>::Unit(d).
    Vector2<Real> boxCenter, boxExtent;
    box.GetCenteredForm(boxCenter, boxExtent);

    // Transform the segment to a centered form in the aligned-box coordinate
    // system.
    Vector2<Real> transformedP0 = segment.p[0] - boxCenter;
    Vector2<Real> transformedP1 = segment.p[1] - boxCenter;
    Segment2<Real> transformedSegment(transformedP0, transformedP1);
    Vector2<Real> segOrigin, segDirection;
    Real segExtent;
    transformedSegment.GetCenteredForm(segOrigin, segDirection, segExtent);

    Result result;
    DoQuery(segOrigin, segDirection, segExtent, boxExtent, result);
    for (int i = 0; i < result.numIntersections; ++i)
    {
        result.point[i] = segOrigin + result.parameter[i] * segDirection;
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
void FIQuery<Real, Segment2<Real>, AlignedBox2<Real>>::DoQuery(
    Vector2<Real> const& segOrigin, Vector2<Real> const& segDirection,
    Real segExtent, Vector2<Real> const& boxExtent, Result& result)
{
    FIQuery<Real, Line2<Real>, AlignedBox2<Real>>::DoQuery(segOrigin,
        segDirection, boxExtent, result);

    if (result.intersect)
    {
        // The line containing the segment intersects the box; the t-interval
        // is [t0,t1].  The segment intersects the box as long as [t0,t1]
        // overlaps the segment t-interval [-segExtent,+segExtent].
        std::array<Real, 2> segInterval = { -segExtent, segExtent };
        FIQuery<Real, std::array<Real, 2>, std::array<Real, 2>> iiQuery;
        result.parameter = iiQuery(result.parameter, segInterval).overlap;
    }
}
//----------------------------------------------------------------------------
