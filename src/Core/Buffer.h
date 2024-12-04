#pragma once

#include "WinUtil.h"


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