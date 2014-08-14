// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include <new>
#include <malloc.h>

namespace gte
{

template <size_t N>
class AlignedMemory
{
public:
    void* operator new(size_t s);
    void operator delete(void* p) throw();

    void* operator new(size_t s, const std::nothrow_t& nt) throw();
    void operator delete(void* p, const std::nothrow_t& nt) throw();

    void* operator new(size_t s, void* location) throw();
    void operator delete(void* p, void* location) throw();

    void* operator new[](size_t s);
    void operator delete[](void* p) throw();

    void* operator new[](size_t s, const std::nothrow_t& nt) throw();
    void operator delete[](void* p, const std::nothrow_t& nt) throw();

    void* operator new[](size_t s, void* location) throw();
    void operator delete[](void* p, void* location) throw();
};

#include "GteAlignedMemory.inl"

}
