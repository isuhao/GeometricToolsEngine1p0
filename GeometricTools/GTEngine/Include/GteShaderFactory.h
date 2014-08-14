// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GTEngineDEF.h"
#include "GteHLSLFactory.h"
#include "GteComputeShader.h"
#include "GteGeometryShader.h"
#include "GtePixelShader.h"
#include "GteVertexShader.h"

namespace gte
{

#define GTE_DEFAULT_HLSL_COMPILE_FLAGS (D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_IEEE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3)

class GTE_IMPEXP ShaderFactory
{
public:
    // Create a shader from an HLSL program in a file.
    static VertexShader* CreateVertex(
        std::string const& filename,
        HLSLDefiner const& definer = HLSLDefiner(),
        std::string const& entry = "VSMain",
        std::string const& target = "vs_5_0",
        unsigned int flags = GTE_DEFAULT_HLSL_COMPILE_FLAGS);

    static PixelShader* CreatePixel(
        std::string const& filename,
        HLSLDefiner const& definer = HLSLDefiner(),
        std::string const& entry = "PSMain",
        std::string const& target = "ps_5_0",
        unsigned int flags = GTE_DEFAULT_HLSL_COMPILE_FLAGS);

    static GeometryShader* CreateGeometry(
        std::string const& filename,
        HLSLDefiner const& definer = HLSLDefiner(),
        std::string const& entry = "GSMain",
        std::string const& target = "gs_5_0",
        unsigned int flags = GTE_DEFAULT_HLSL_COMPILE_FLAGS);

    static ComputeShader* CreateCompute(
        std::string const& filename,
        HLSLDefiner const& definer = HLSLDefiner(),
        std::string const& entry = "CSMain",
        std::string const& target = "cs_5_0",
        unsigned int flags = GTE_DEFAULT_HLSL_COMPILE_FLAGS);

    // Create a shader from an HLSL represented as a string.
    static VertexShader* CreateVertex(
        std::string const& name,
        std::string const& source,
        HLSLDefiner const& definer = HLSLDefiner(),
        std::string const& entry = "VSMain",
        std::string const& target = "vs_5_0",
        unsigned int flags = GTE_DEFAULT_HLSL_COMPILE_FLAGS);

    static PixelShader* CreatePixel(
        std::string const& name,
        std::string const& source,
        HLSLDefiner const& definer = HLSLDefiner(),
        std::string const& entry = "PSMain",
        std::string const& target = "ps_5_0",
        unsigned int flags = GTE_DEFAULT_HLSL_COMPILE_FLAGS);

    static GeometryShader* CreateGeometry(
        std::string const& name,
        std::string const& source,
        HLSLDefiner const& definer = HLSLDefiner(),
        std::string const& entry = "GSMain",
        std::string const& target = "gs_5_0",
        unsigned int flags = GTE_DEFAULT_HLSL_COMPILE_FLAGS);

    static ComputeShader* CreateCompute(
        std::string const& name,
        std::string const& source,
        HLSLDefiner const& definer = HLSLDefiner(),
        std::string const& entry = "CSMain",
        std::string const& target = "cs_5_0",
        unsigned int flags = GTE_DEFAULT_HLSL_COMPILE_FLAGS);

    // Convenient function for generating shader programs as multiline
    // text strings in a CPP file.  A call to this is wrapped by the
    // GTE_MAKE_HLSL_STRING macro.  See GteVertexColorEffect.cpp for an
    // example of how to use this.
    static std::string MakeShaderString(std::string const& text);

private:
    // Create a shader from an HLSL program in a file.
    static Shader* Create(
        std::string const& filename,
        HLSLDefiner const& definer,
        std::string const& entry,
        std::string const& target,
        unsigned int flags);

    // Create a shader from an HLSL represented as a string.
    static Shader* Create(
        std::string const& name,
        std::string const& source,
        HLSLDefiner const& definer,
        std::string const& entry,
        std::string const& target,
        unsigned int flags);
};

// See the comments above for MakeHLSLString.
#define GTE_MAKE_HLSL_STRING(s) ShaderFactory::MakeShaderString(#s)

}
