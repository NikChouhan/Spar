#pragma once

#include "WinUtil.h"

struct ConstantBuffer
{
    DirectX::XMMATRIX mWorld;
    DirectX::XMMATRIX mView;
    DirectX::XMMATRIX mProjection;
    DirectX::XMFLOAT4 vMeshColor;
};

struct SimpleVertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT2 Tex;
};

struct DrawableAssets
{

    wrl::ComPtr<ID3D11Buffer> vertexBuffer;
    wrl::ComPtr<ID3D11Buffer> indexBuffer;
    wrl::ComPtr<ID3D11ShaderResourceView> textureView;
    wrl::ComPtr<ID3D11SamplerState> samplerState;
    wrl::ComPtr<ID3D11Buffer> constantBuffer = nullptr;
    UINT indexCount;
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