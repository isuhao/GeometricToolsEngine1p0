// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GteDCPQuery.h"
#include "GteRay3.h"

namespace gte
{

template <typename Real>
class DCPQuery<Real, Ray3<Real>, Ray3<Real>>
{
public:
    struct Result
    {
        Real distance, sqrDistance;
        Real parameter[2];
        Vector3<Real> closestPoint[2];
    };

    Result operator()(Ray3<Real> const& ray0, Ray3<Real> const& ray1);
};

#include "GteDistRay3Ray3.inl"

}
