// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GTEngineDEF.h"
#include "GteDX11TextureSingle.h"
#include "GteTexture3.h"

namespace gte
{

class GTE_IMPEXP DX11Texture3 : public DX11TextureSingle
{
public:
    // Construction and destruction.
    virtual ~DX11Texture3();
    DX11Texture3(ID3D11Device* device, Texture3 const* texture);
    static DX11GraphicsObject* Create(ID3D11Device* device,
        GraphicsObject const* object);

    // Member access.
    Texture3* GetTexture() const;
    ID3D11Texture3D* GetDXTexture() const;

private:
    // Support for construction.
    void CreateStaging(ID3D11Device* device, D3D11_TEXTURE3D_DESC const& tx);
    void CreateSRView(ID3D11Device* device, D3D11_TEXTURE3D_DESC const& tx);
    void CreateUAView(ID3D11Device* device, D3D11_TEXTURE3D_DESC const& tx);
};

}
