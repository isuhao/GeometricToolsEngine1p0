// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GteTIQuery.h"
#include "GteFIQuery.h"
#include "GteLine3.h"
#include "GteEllipsoid3.h"

// The queries consider the ellipsoid to be a solid.

namespace gte
{

template <typename Real>
class TIQuery<Real, Line3<Real>, Ellipsoid3<Real>>
{
public:
    struct Result
    {
        bool intersect;
    };

    Result operator()(Line3<Real> const& line,
        Ellipsoid3<Real> const& ellipsoid);
};

template <typename Real>
class FIQuery<Real, Line3<Real>, Ellipsoid3<Real>>
{
public:
    struct Result
    {
        bool intersect;
        int numIntersections;
        std::array<Real, 2> parameter;
        std::array<Vector3<Real>, 2> point;
    };

    Result operator()(Line3<Real> const& line,
        Ellipsoid3<Real> const& ellipsoid);

protected:
    void DoQuery(Vector3<Real> const& lineOrigin,
        Vector3<Real> const& lineDirection, Ellipsoid3<Real> const& ellipsoid,
        Result& result);
};

#include "GteIntrLine3Ellipsoid3.inl"

}
