#pragma once

#include "WinUtil.h"

struct ConstantBuffer
{
    DirectX::XMMATRIX mWorld;
    DirectX::XMMATRIX mView;
    DirectX::XMMATRIX mProjection;
};


struct VertexBuffer
{
    wrl::ComPtr<ID3D11Buffer> Buffer;
    u32 Stride;
    u32 Offset;
};

struct IndexBuffer
{
    wrl::ComPtr<ID3D11Buffer> Buffer;
    u32 Stride;
    u32 Offset;
};