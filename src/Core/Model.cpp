#include "Model.h"

namespace Spar
{
    HRESULT Model::LoadModel(const char* path)
    {
        if (!path)
        {
            return E_INVALIDARG;
        }

        cgltf_options options = {};
        cgltf_data* data = nullptr;
        cgltf_result result = cgltf_parse_file(&options, path, &data);

        if (result != cgltf_result_success)
        {
            return E_FAIL;
        }

        result = cgltf_load_buffers(&options, data, path);

        if (result != cgltf_result_success)
        {
            cgltf_free(data);
            return E_FAIL;
        }

        m_model = data;

        return S_OK;
    }

    HRESULT Model::LoadTexture(std::shared_ptr<Graphics::Renderer> renderer, wrl::ComPtr<ID3D11ShaderResourceView> m_textureView, const WCHAR* path)
    {
        if (!path)
        {
            return E_INVALIDARG;
        }

        HRESULT hr = DirectX::CreateDDSTextureFromFile(renderer->m_device.Get(), path, nullptr, m_textureView.GetAddressOf());


        if (FAILED(hr))
        {
            return hr;
        }

        return S_OK;
    }
}