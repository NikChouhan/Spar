#pragma once

#include "cgltf.h"
#include "WinUtil.h"
#include "DDSTextureLoader.h"
#include "Buffer.h"


namespace Spar
{
    class Model
    {
    public:
        Model();
        ~Model();
        HRESULT LoadModel(const char* path);
    //private:
        cgltf_data* m_model;
        wrl::ComPtr<ID3D11ShaderResourceView> m_textureView = nullptr;

        DrawableAssets assets{};
        
    };
}