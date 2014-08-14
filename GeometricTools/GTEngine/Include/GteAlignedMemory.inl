// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <size_t N>
void* AlignedMemory<N>::operator new(size_t s)
{
    return _aligned_malloc(s, N);
}
//----------------------------------------------------------------------------
template <size_t N>
void AlignedMemory<N>::operator delete(void* p) throw()
{
    _aligned_free(p);
}
//----------------------------------------------------------------------------
template <size_t N>
void* AlignedMemory<N>::operator new(size_t s, const std::nothrow_t&) throw()
{
    void* p;
    try
    {
        p = _aligned_malloc(s, N);
    }
    catch (...)
    {
        p = nullptr;
    }
    return p;
}
//----------------------------------------------------------------------------
template <size_t N>
void AlignedMemory<N>::operator delete(void* p, const std::nothrow_t&) throw()
{
    operator delete(p);
}
//----------------------------------------------------------------------------
template <size_t N>
void* AlignedMemory<N>::operator new(size_t, void* location) throw()
{
    // assert:  location is N-byte aligned
    return location;
}
//----------------------------------------------------------------------------
template <size_t N>
void AlignedMemory<N>::operator delete(void*, void*) throw()
{
}
//----------------------------------------------------------------------------
template <size_t N>
void* AlignedMemory<N>::operator new[](size_t s)
{
    return operator new(s);
}
//----------------------------------------------------------------------------
template <size_t N>
void AlignedMemory<N>::operator delete[](void* p) throw()
{
    operator delete(p);
}
//----------------------------------------------------------------------------
template <size_t N>
void* AlignedMemory<N>::operator new[](size_t s, const std::nothrow_t& nt)
throw()
{
    return operator new(s, nt);
}
//----------------------------------------------------------------------------
template <size_t N>
void AlignedMemory<N>::operator delete[](void* p, const std::nothrow_t& nt)
throw()
{
    operator delete(p, nt);
}
//----------------------------------------------------------------------------
template <size_t N>
void* AlignedMemory<N>::operator new[](size_t, void* location) throw()
{
    // assert:  location is N-byte aligned
    return location;
}
//----------------------------------------------------------------------------
template <size_t N>
void AlignedMemory<N>::operator delete[](void*, void*) throw()
{
}
//----------------------------------------------------------------------------
