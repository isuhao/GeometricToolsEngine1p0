// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Real>
typename TIQuery<Real, Plane3<Real>, Cylinder3<Real>>::Result
TIQuery<Real, Plane3<Real>, Cylinder3<Real>>::operator()(
    Plane3<Real> const& plane, Cylinder3<Real> const& cylinder)
{
    LogAssert(cylinder.height != std::numeric_limits<Real>::max(),
        "Cylinder height must be finite for TIQuery.");

    Result result;

    // Compute extremes of signed distance Dot(N,X)-d for points on the
    // cylinder.  These are
    //   min = (Dot(N,C)-d) - r*sqrt(1-Dot(N,W)^2) - (h/2)*|Dot(N,W)|
    //   max = (Dot(N,C)-d) + r*sqrt(1-Dot(N,W)^2) + (h/2)*|Dot(N,W)|
    DCPQuery<Real, Vector3<Real>, Plane3<Real>> vpQuery;
    Real distance = vpQuery(cylinder.axis.origin, plane).distance;
    Real absNdW = std::abs(Dot(plane.normal, cylinder.axis.direction));
    Real root = sqrt(std::max((Real)1 - absNdW*absNdW, (Real)0));
    Real term = cylinder.radius*root + ((Real)0.5)*cylinder.height*absNdW;

    // Intersection occurs if and only if 0 is in the interval [min,max].
    result.intersect = (distance <= term);
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
typename FIQuery<Real, Plane3<Real>, Cylinder3<Real>>::Result
FIQuery<Real, Plane3<Real>, Cylinder3<Real>>::operator()(
    Plane3<Real> const& plane, Cylinder3<Real> const& cylinder)
{
    LogAssert(cylinder.height == std::numeric_limits<Real>::max(),
        "Cylinder height must be infinite for FIQuery.");

    Result result;

    DCPQuery<Real, Vector3<Real>, Plane3<Real>> vpQuery;
    Real sdistance = vpQuery(cylinder.axis.origin, plane).signedDistance;
    Vector3<Real> center = cylinder.axis.origin - sdistance*plane.normal;
    Real cosTheta = Dot(cylinder.axis.direction, plane.normal);
    Real absCosTheta = std::abs(cosTheta);

    if (absCosTheta > (Real)0)
    {
        // The cylinder axis intersects the plane in a unique point.
        result.intersect = true;
        if (absCosTheta < (Real)1)
        {
            result.type = 4;
            result.ellipse.normal = plane.normal;
            result.ellipse.center = cylinder.axis.origin -
                (sdistance / cosTheta)*cylinder.axis.direction;
            result.ellipse.axis[0] = cylinder.axis.direction -
                cosTheta*plane.normal;
            Normalize(result.ellipse.axis[0]);
            result.ellipse.axis[1] = UnitCross(plane.normal,
                result.ellipse.axis[0]);
            result.ellipse.extent[0] = cylinder.radius / absCosTheta;
            result.ellipse.extent[1] = cylinder.radius;
        }
        else
        {
            result.type = 3;
            result.circle.normal = plane.normal;
            result.circle.center = center;
            result.circle.radius = cylinder.radius;
        }
    }
    else
    {
        // The cylinder is parallel to the plane.
        Real distance = std::abs(sdistance);
        if (distance < cylinder.radius)
        {
            result.intersect = true;
            result.type = 2;

            Vector3<Real> offset = Cross(cylinder.axis.direction,
                plane.normal);
            Real extent = sqrt(cylinder.radius*cylinder.radius -
                sdistance*sdistance);

            result.line[0].origin = center - extent*offset;
            result.line[0].direction = cylinder.axis.direction;
            result.line[1].origin = center + extent*offset;
            result.line[1].direction = cylinder.axis.direction;
        }
        else if (distance == cylinder.radius)
        {
            result.intersect = true;
            result.type = 1;
            result.line[0].origin = center;
            result.line[0].direction = cylinder.axis.direction;
        }
        else
        {
            result.intersect = false;
            result.type = 0;
        }
    }

    return result;
}
//----------------------------------------------------------------------------
