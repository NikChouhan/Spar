#pragma once

#include "WinUtil.h"

struct ConstantBuffer
{
    DirectX::XMMATRIX mWorld;
    DirectX::XMMATRIX mView;
    DirectX::XMMATRIX mProjection;
};

struct SimpleVertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT2 Tex;
};

struct CBNeverChanges
{
    DirectX::XMMATRIX mView;
};

struct CBChangesOnResize
{
    DirectX::XMMATRIX mProjection;
};

struct CBChangesEveryFrame
{
    DirectX::XMMATRIX mWorld;
    DirectX::XMFLOAT4 vMeshColor;
};