// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#if !defined(_MSC_VER) || _MSC_VER < 1800
#error Microsoft Visual Studio 2013 or later is required.
#endif

// Add GTE_EXPORT to project preprocessor options for dynamic library
// configurations to export their symbols.
#if defined(GTE_EXPORT)
    // For the dynamic library configurations.
    #define GTE_IMPEXP __declspec(dllexport)
#else
    // For a client of the dynamic library or for the static library
    // configurations.
    #define GTE_IMPEXP
#endif

// Select the storage convention for matrices.  Exactly one convention must be
// enabled.
#define GTE_USE_ROW_MAJOR
//#define GTE_USE_COL_MAJOR
#if defined(GTE_USE_ROW_MAJOR) && defined(GTE_USE_COL_MAJOR)
#error Exactly one storage order must be specified.
#endif
#if !defined(GTE_USE_ROW_MAJOR) && !defined(GTE_USE_COL_MAJOR)
#error Exactly one storage order must be specified.
#endif

// Select the matrix-vector multiplication convention.  Exactly one convention
// must be enabled.
#define GTE_USE_MAT_VEC
//#define GTE_USE_VEC_MAT
#if defined(GTE_USE_MAT_VEC) && defined(GTE_USE_VEC_MAT)
#error Exactly one multiplication convention must be specified.
#endif
#if !defined(GTE_USE_MAT_VEC) && !defined(GTE_USE_VEC_MAT)
#error Exactly one multiplication convention must be specified.
#endif
