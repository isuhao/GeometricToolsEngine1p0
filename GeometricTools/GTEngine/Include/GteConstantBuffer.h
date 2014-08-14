// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GTEngineDEF.h"
#include "GteBuffer.h"
#include "GteMemberLayout.h"

namespace gte
{

class GTE_IMPEXP ConstantBuffer : public Buffer
{
public:
    // Construction.
    ConstantBuffer(size_t numBytes, bool allowDynamicUpdate);

    // Access to constant buffer members using the layout of a shader
    // program itself is allowed as long as you have attached the constant
    // buffer to a shader first.
    //   std::shared_ptr<VertexShader> vshader = <some shader>;
    //   std::shared_ptr<ConstantBuffer> cbuffer = <buffer for the shader>;
    //   vshader->Set("MyCBuffer", cbuffer);
    // Now you can use SetMember/GetMember calls successfully.  In these
    // calls, you are required to specify the correct type T for the member.
    // No checking is performed for the size of the input; i.e., too large a
    // 'value' will cause a memory overwrite within the buffer.  The code
    // does test to ensure that no overwrite occurs outside the buffer.

    inline std::vector<MemberLayout> const& GetLayout() const;

    // Set or get a non-array member.
    template <typename T>
    bool SetMember(std::string const& name, T const& value);

    template <typename T>
    bool GetMember(std::string const& name, T& value) const;

    // Set or get an array member.
    template <typename T>
    bool SetMember(std::string const& name, unsigned int index,
        T const& value);

    template <typename T>
    bool GetMember(std::string const& name, unsigned int index,
        T& value) const;

protected:
    // For rounding up to nearest 16-byte multiple when allocating
    // buffer memory (HLSL register uses 16-bytes).
    static size_t GetRoundedNumBytes(size_t numBytes);

    // Allow the Shader class to set the data to avoid having to expose
    // setting the lookup table via a public interface.  The 'const' is
    // to allow Shader to call SetLayout from a 'const' ConstantBuffer.
    friend class Shader;
    mutable std::vector<MemberLayout> mLayout;

public:
    // For use by the Shader class for storing reflection information.
    static int const shaderDataLookup = 0;
};

#include "GteConstantBuffer.inl"

}
