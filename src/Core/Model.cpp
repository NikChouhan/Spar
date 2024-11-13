#include "Model.h"

namespace Spar
{
    Model::Model()
    {
        m_model = nullptr;
    }
    Model::~Model()
    {
        if (m_model)
        {
            cgltf_free(m_model);
        }
    }

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
}