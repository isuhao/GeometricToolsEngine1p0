// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

namespace gte
{

// For 2D arrays:  data[bound1][bound0]
template <typename T>
T** Allocate2(size_t const bound0, size_t const bound1);

// For 3D arrays:  data[bound2][bound1][bound0]
template <typename T>
T*** Allocate3(size_t const bound0, size_t const bound1, size_t const bound2);

// For 4D arrays:  data[bound3][bound2][bound1][bound0]
template <typename T>
T**** Allocate4(size_t const bound0, size_t const bound1, size_t const bound2,
    size_t const bound3);

// For 2D arrays:  data[bound1][bound0]
template <typename T>
void Deallocate2(T**& data);

// For 3D arrays:  data[bound2][bound1][bound0]
template <typename T>
void Deallocate3(T***& data);

// For 4D arrays:  data[bound3][bound2][bound1][bound0]
template <typename T>
void Deallocate4(T****& data);

#include "GteMemory.inl"

}
