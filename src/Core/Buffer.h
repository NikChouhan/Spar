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

struct RenderCommand
{
    wrl::ComPtr<ID3D11Buffer> vertexBuffer;
    wrl::ComPtr<ID3D11Buffer> indexBuffer;
    wrl::ComPtr<ID3D11ShaderResourceView> textureView;
    wrl::ComPtr<ID3D11SamplerState> samplerState;
    UINT indexCount;
};