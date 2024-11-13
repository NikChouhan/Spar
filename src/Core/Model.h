#pragma once

#include "cgltf.h"
#include "WinUtil.h"
#include "renderer.h"
#include "DDSTextureLoader.h"


namespace Spar
{
    class Model
    {
    public:
        Model()
        {
            m_model = nullptr;
        }
        ~Model()
        {
            if (m_model)
            {
                cgltf_free(m_model);
            }
        }

        HRESULT LoadModel(const char* path);
        HRESULT LoadTexture(std::shared_ptr<Graphics::Renderer> renderer, wrl::ComPtr<ID3D11ShaderResourceView> m_textureView, const WCHAR* path);

    private:
        cgltf_data* m_model;
        std::shared_ptr<Spar::Graphics::Renderer> renderer;
        wrl::ComPtr<ID3D11ShaderResourceView> m_textureView = nullptr;

    };
}