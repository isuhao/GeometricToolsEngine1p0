// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Real>
typename TIQuery<Real, Line2<Real>, OrientedBox2<Real>>::Result
TIQuery<Real, Line2<Real>, OrientedBox2<Real>>::operator()(
    Line2<Real> const& line, OrientedBox2<Real> const& box)
{
    // Transform the line to the oriented-box coordinate system.
    Vector2<Real> diff = line.origin - box.center;
    Vector2<Real> lineOrigin
        (
        Dot(diff, box.axis[0]),
        Dot(diff, box.axis[1])
        );
    Vector2<Real> lineDirection
        (
        Dot(line.direction, box.axis[0]),
        Dot(line.direction, box.axis[1])
        );

    Result result;
    DoQuery(lineOrigin, lineDirection, box.extent, result);
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
typename FIQuery<Real, Line2<Real>, OrientedBox2<Real>>::Result
FIQuery<Real, Line2<Real>, OrientedBox2<Real>>::operator()(
    Line2<Real> const& line, OrientedBox2<Real> const& box)
{
    // Transform the line to the oriented-box coordinate system.
    Vector2<Real> diff = line.origin - box.center;
    Vector2<Real> lineOrigin
        (
        Dot(diff, box.axis[0]),
        Dot(diff, box.axis[1])
        );
    Vector2<Real> lineDirection
        (
        Dot(line.direction, box.axis[0]),
        Dot(line.direction, box.axis[1])
        );

    Result result;
    DoQuery(lineOrigin, lineDirection, box.extent, result);
    for (int i = 0; i < result.numIntersections; ++i)
    {
        result.point[i] = line.origin + result.parameter[i] * line.direction;
    }
    return result;
}
//----------------------------------------------------------------------------
