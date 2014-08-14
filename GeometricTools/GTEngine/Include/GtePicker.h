// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GTEngineDEF.h"
#include "GtePickRecord.h"

namespace gte
{

class GTE_IMPEXP Picker
{
public:
    // Construction and destruction.
    ~Picker ();
    Picker ();

    // The linear component is parameterized by P + t*D, where P is a point on
    // the component (the origin) and D is a unit-length direction vector.
    // Both P and D must be in world coordinates.  The interval [tmin,tmax] is
    //   line:     tmin = -Mathf::MAX_REAL, tmax = Mathf::MAX_REAL
    //   ray:      tmin = 0, tmax = Mathf::MAX_REAL
    //   segment:  tmin = 0, tmax > 0;
    // where INFINITY is std::numeric_limits<float>::max().  A call to this
    // function will automatically clear the Records array.  If you need any
    // information from this array obtained by a previous call to Execute, you
    // must save it first.
    void Execute (Spatial* scene, Vector4<float> const& origin,
        Vector4<float> const& direction, float tmin, float tmax);

    // The following three functions return the record satisfying the
    // constraints.  They should be called only when mRecords.size() > 0.

    // Locate the record whose t-value is smallest in absolute value.
    PickRecord const& GetClosestToZero () const;

    // Locate the record with nonnegative t-value closest to zero.
    PickRecord const& GetClosestNonnegative () const;

    // Locate the record with nonpositive t-value closest to zero.
    PickRecord const& GetClosestNonpositive () const;

    // Access to all the records for the pick operation.
    std::vector<PickRecord> records;

private:
    // The picking occurs recursively by traversing the input scene.
    void ExecuteRecursive (Spatial* object);

    Vector4<float> mOrigin;
    Vector4<float> mDirection;
    float mTMin, mTMax;

    // The value returned if the Get* functions are called when mRecords has
    // no elements.
    static PickRecord const msInvalid;
};

}
