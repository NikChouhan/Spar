#include "Model.h"
#include "Log.h"
#include "Texture.h"
#include "renderer.h"
#include "Camera.h"

Spar::Model::Model()
{
    m_model = nullptr;
}
Spar::Model::~Model()
{
    if (m_model)
    {
        cgltf_free(m_model);
    }
}

void Spar::Model::LoadModel(std::shared_ptr<Spar::Renderer> renderer, std::string path)
{
    this->renderer = renderer;
    cgltf_options options = {};
    cgltf_data *data = nullptr;
    cgltf_result result = cgltf_parse_file(&options, path.c_str(), &data);

    if (result != cgltf_result_success)
        Log::Error("[CGLTF] Failed to parse gltf file");
    else
        Log::Info("[CGLTF] Successfully parsed gltf file");

    result = cgltf_load_buffers(&options, data, path.c_str());

    if (result != cgltf_result_success)
    {
        cgltf_free(data);
        Log::Error("[CGLTF] Failed to load buffers");
    }
    else
    {
        Log::Info("[CGLTF] Successfully loaded buffers");
    }

    cgltf_scene *scene = data->scene;

    if (!scene)
    {
        Log::Error("[CGLTF] No scene found in gltf file");
    }
    else
    {
        Log::Info("[CGLTF] Scene found in gltf file");
        m_dirPath = path.substr(0, path.find_last_of("/"));

        for (size_t i = 0; i < (scene->nodes_count); i++)
        {
            ProcessNode(scene->nodes[i], data, vertices, indices);
        }

        SetBuffers();

        Log::Info("[CGLTF] Successfully loaded gltf file");
    }


    cgltf_free(data);
}

void Spar::Model::ProcessMesh(cgltf_mesh *mesh, const cgltf_data *data, std::vector<SimpleVertex> &vertices, std::vector<u32> &indices)
{
    for (size_t i = 0; i < mesh->primitives_count; i++)
    {
        ProcessPrimitive(&mesh->primitives[i], data, vertices, indices);
    }
}

void Spar::Model::ProcessNode(cgltf_node *node, const cgltf_data *data, std::vector<SimpleVertex> &vertices, std::vector<u32> &indices)
{
    if (node->mesh)
    {
        ProcessMesh(node->mesh, data, vertices, indices);
    }

    for (size_t i = 0; i < node->children_count; i++)
    {
        ProcessNode(node->children[i], data, vertices, indices);
    }
}

void Spar::Model::ProcessPrimitive(cgltf_primitive *primitive, const cgltf_data *data, std::vector<SimpleVertex> &vertices, std::vector<u32> &indices)
{
    u32 vertexOffset = vertices.size();
    u32 indexOffset = indices.size();

    if (primitive->type != cgltf_primitive_type_triangles)
    {
        Log::Warn("[CGLTF] Primitive type is not triangles");
        return;
    }

    if (primitive->indices == nullptr)
    {
        Log::Error("[CGLTF] Primitive has no indices");
        return;
    }

    if (primitive->material == nullptr)
    {
        Log::Error("[CGLTF] Primitive has no material");
        return;
    }

    Primitive prim;

    // Get attributes
    cgltf_attribute *pos_attribute = nullptr;
    cgltf_attribute *tex_attribute = nullptr;
    cgltf_attribute *norm_attribute = nullptr;

    for (int i = 0; i < primitive->attributes_count; i++)
    {
        if (strcmp(primitive->attributes[i].name, "POSITION") == 0)
        {
            pos_attribute = &primitive->attributes[i];
        }
        if (strcmp(primitive->attributes[i].name, "TEXCOORD_0") == 0)
        {
            tex_attribute = &primitive->attributes[i];
        }
        if (strcmp(primitive->attributes[i].name, "NORMAL") == 0)
        {
            norm_attribute = &primitive->attributes[i];
        }
    }

    if (!pos_attribute || !tex_attribute || !norm_attribute)
    {
        Log::Warn("[CGLTF] Missing attributes in primitive");
        return;
    }

    int vertexCount = pos_attribute->data->count;
    int indexCount = primitive->indices->count;

    for (int i = 0; i < vertexCount; i++)
    {
        SimpleVertex vertex = { };

        if (cgltf_accessor_read_float(pos_attribute->data, i, &vertex.Pos.x, 3) == 0)
        {
            Log::Warn("[CGLTF] Unable to read Position attributes!");
        }
        if (cgltf_accessor_read_float(tex_attribute->data, i, &vertex.Tex.x, 2) == 0)
        {
            Log::Warn("[CGLTF] Unable to read Texture attributes!");
        }
        if (cgltf_accessor_read_float(norm_attribute->data, i, &vertex.Normal.x, 3) == 0)
        {
            Log::Warn("[CGLTF] IUnable to read Normal attributes!");
        }

        vertices.push_back(vertex);
    }

    for (int i = 0; i < indexCount; i++)
    {
        indices.push_back(cgltf_accessor_read_index(primitive->indices, i));
    }

    // material

    cgltf_material *material = primitive->material;
    Material mat = {};
    prim.materialIndex = material - data->materials;

    if (material->has_pbr_metallic_roughness)
    {
        cgltf_pbr_metallic_roughness *pbr = &material->pbr_metallic_roughness;
        // Load base color (albedo) texture
        LoadMaterialTexture(mat, &pbr->base_color_texture, TextureType::ALBEDO);
        // Load metallic-roughness texture
        LoadMaterialTexture(mat, &pbr->metallic_roughness_texture, TextureType::METALLIC_ROUGHNESS);
    }

    if (material->normal_texture.texture)
    {
        // Load normal texture
        LoadMaterialTexture(mat, &material->normal_texture, TextureType::NORMAL);
    }

    if (material->emissive_texture.texture)
    {
        // Load emissive texture
        LoadMaterialTexture(mat, &material->emissive_texture, TextureType::EMISSIVE);
    }

    if (material->occlusion_texture.texture)
    {
        // Load occlusion texture
        LoadMaterialTexture(mat, &material->occlusion_texture, TextureType::AO);
    }

    prim.startIndex = indexOffset;
    prim.startVertex = vertexOffset;
    prim.vertexCount = vertices.size() - vertexOffset;
    prim.indexCount = indices.size() - indexOffset;

    prim.materialIndex = materials.size();
    materials.push_back(mat);
    primitives.push_back(prim);
}

void Spar::Model::LoadMaterialTexture(Material &mat, cgltf_texture_view *textureView, TextureType type)
{
    if (textureView && textureView->texture && textureView->texture->image)
    {
        cgltf_image *image = textureView->texture->image;
        std::string path = m_dirPath + "/" + std::string(image->uri);

        Texture tex;
        tex.LoadTexture(renderer, path.c_str());

        switch (type)
        {
        case TextureType::ALBEDO:
            mat.AlbedoView = tex.m_textureView;
            mat.HasAlbedo = true;
            break;
        case TextureType::NORMAL:
            mat.NormalView = tex.m_textureView;
            mat.HasNormal = true;
            break;
        case TextureType::METALLIC_ROUGHNESS:
            mat.MetallicRoughnessView = tex.m_textureView;
            mat.HasMetallicRoughness = true;
            break;
        case TextureType::EMISSIVE:
            mat.EmissiveView = tex.m_textureView;
            mat.HasEmissive = true;
            break;
        case TextureType::AO:
            mat.AOView = tex.m_textureView;
            mat.HasAO = true;
            break;
        default:
            Log::Warn("Unknown texture type.");
            break;
        }
    }
    else
    {
        // Handle missing texture or image
        Log::Warn("Texture or image not found for this material.");
    }
}

void Spar::Model::SetBuffers()
{
    // Create vertex buffer
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = sizeof(SimpleVertex) * vertices.size();
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices.data();

    HRESULT hr = renderer->m_device->CreateBuffer(&vbDesc, &vbData, m_vertexBuffer.GetAddressOf());
    if (FAILED(hr))
        Log::Error("Failed to create vertex buffer");

    // Create index buffer
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.ByteWidth = sizeof(u32) * indices.size();
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices.data();

    hr = renderer->m_device->CreateBuffer(&ibDesc, &ibData, m_indexBuffer.GetAddressOf());
    if (FAILED(hr))
        Log::Error("Failed to create index buffer");

    // Set constant buffer
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(ConstantBuffer);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = renderer->m_device->CreateBuffer(&cbDesc, nullptr, m_constantBuffer.GetAddressOf());
    if (FAILED(hr))
        Log::Error("Failed to create world/projection/view matrix constant buffer");

    // Set material constant buffer
    D3D11_BUFFER_DESC matDesc = {};
    matDesc.ByteWidth = sizeof(MaterialConstants);
    matDesc.Usage = D3D11_USAGE_DYNAMIC;
    matDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matDesc.MiscFlags = 0;

    hr = renderer->m_device->CreateBuffer(&matDesc, nullptr, m_materialBuffer.GetAddressOf());
    if (FAILED(hr))
        Log::Error("Failed to create material constant buffer");


    // Store counts
    m_vertexCount = vertices.size();
    m_indexCount = indices.size();

    // Clear CPU data
    vertices.clear();
    indices.clear();
}

bool Spar::Model::SetTexResources()
{
    renderer->m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    renderer->m_context->PSSetConstantBuffers(0, 1, m_materialBuffer.GetAddressOf());
    for (auto &mat : materials)
    {
        if (mat.HasAlbedo)
        {
            renderer->m_context->PSSetShaderResources(static_cast<UINT>(TextureType::ALBEDO), 1, mat.AlbedoView.GetAddressOf());
            renderer->m_context->PSSetSamplers(static_cast<UINT>(TextureType::ALBEDO), 1, mat.samplerState.GetAddressOf());
        }
        if (mat.HasNormal)
        {
            renderer->m_context->PSSetShaderResources(static_cast<UINT>(TextureType::NORMAL), 1, mat.NormalView.GetAddressOf());
            renderer->m_context->PSSetSamplers(static_cast<UINT>(TextureType::NORMAL), 1, mat.samplerState.GetAddressOf());
        }
        if (mat.HasMetallicRoughness)
        {
            renderer->m_context->PSSetShaderResources(static_cast<UINT>(TextureType::METALLIC_ROUGHNESS), 1, mat.MetallicRoughnessView.GetAddressOf());
            renderer->m_context->PSSetSamplers(static_cast<UINT>(TextureType::METALLIC_ROUGHNESS), 1, mat.samplerState.GetAddressOf());
        }
        if (mat.HasEmissive)
        {
            renderer->m_context->PSSetShaderResources(static_cast<UINT>(TextureType::EMISSIVE), 1, mat.EmissiveView.GetAddressOf());
            renderer->m_context->PSSetSamplers(static_cast<UINT>(TextureType::EMISSIVE), 1, mat.samplerState.GetAddressOf());
        }
        if (mat.HasAO)
        {
            renderer->m_context->PSSetShaderResources(static_cast<UINT>(TextureType::AO), 1, mat.AOView.GetAddressOf());
            renderer->m_context->PSSetSamplers(static_cast<UINT>(TextureType::AO), 1, mat.samplerState.GetAddressOf());
        }
    }

    return true;
}

void Spar::Model::UpdateCB(std::shared_ptr<Spar::Renderer> renderer, std::shared_ptr<Spar::Camera> camera, static f64 dt)
{
    this->camera = camera;
    this->renderer = renderer;

    // update word/view/projection matrix
    cb.mWorld = SM::Matrix::CreateRotationY(dt);
    cb.mView = camera->GetViewMatrix().Transpose();
    cb.mProjection = camera->GetProjectionMatrix().Transpose();
    renderer->m_context->UpdateSubresource(this->m_constantBuffer.Get(), 0, nullptr, &cb, 0, 0);

    // update material color
    for (auto &mat : materials)
    {
        mat.FlatColor = {1.0f, 1.0f, 1.0f};
    }

    // update material constant buffer
    matColor.ambientColor = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    matColor.diffuseColor = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    matColor.specularColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    matColor.specularPower = 32.0f;

    renderer->m_context->UpdateSubresource(this->m_materialBuffer.Get(), 0, nullptr, &matColor, 0, 0);

}

void Spar::Model::Render()
{
    // Bind buffers
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    renderer->m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    renderer->m_context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // Draw each primitive
    for (const auto &prim : primitives)
    {
        // Set material textures
        const Material &mat = materials[prim.materialIndex];
        //SetTexResources();

        // Draw primitive
        renderer->m_context->DrawIndexed(
            prim.indexCount,
            prim.startIndex,
            0);
    }
}
