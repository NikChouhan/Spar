#pragma once

#include "WinUtil.h"

struct ConstantBuffer
{
    DirectX::XMMATRIX mWorld;
    DirectX::XMMATRIX mView;
    DirectX::XMMATRIX mProjection;
    DirectX::XMFLOAT4 vMeshColor;
};

struct DrawableAssets
{
    wrl::ComPtr<ID3D11Buffer> vertexBuffer;
    wrl::ComPtr<ID3D11Buffer> indexBuffer;
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