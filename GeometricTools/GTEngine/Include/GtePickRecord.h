// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GTEngineDEF.h"
#include "GteSpatial.h"

namespace gte
{

class GTE_IMPEXP PickRecord
{
public:
    // Construction and destruction.  The default constructor does not
    // initialize any members.  It exists to support the construction
    // of the const static PickRecord in the Picker class.
    ~PickRecord ();
    PickRecord ();

    // For sorting purposes.
    inline bool operator== (PickRecord const& record) const;
    inline bool operator!= (PickRecord const& record) const;
    inline bool operator<  (PickRecord const& record) const;
    inline bool operator<= (PickRecord const& record) const;
    inline bool operator>  (PickRecord const& record) const;
    inline bool operator>= (PickRecord const& record) const;

    // The intersected object.
    Spatial* intersected;

    // The linear component is parameterized by P + t*D.  The t member is
    // the value of the parameter at the intersection point.
    float t;

    // The index of the triangle that is intersected by the ray.
    int triangle;

    // The barycentric coordinates of the point of intersection.  All of the
    // coordinates are in [0,1] and b0 + b1 + b2 = 1.
    float bary[3];
};

#include "GtePickRecord.inl"

}
