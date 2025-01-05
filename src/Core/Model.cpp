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

void Spar::Model::LoadModel(std::shared_ptr<Spar::Renderer> renderer, std::shared_ptr<Camera> camera, std::string path)
{
    this->camera = camera;
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
            Transformation transform;
            ProcessNode(scene->nodes[i], data, m_vertices, m_indices, transform);
        }
        // no of nodes
        Log::InfoDebug("[CGLTF] No of nodes in the scene: ", scene->nodes_count);

        SetBuffers();

        Log::Info("[CGLTF] Successfully loaded gltf file");
    }

    ValidateResources();

    cgltf_free(data);
}

void Spar::Model::ProcessNode(cgltf_node *node, const cgltf_data *data, std::vector<SimpleVertex> &vertices, std::vector<u32> &indices, Transformation& parentTransform)
{
    Transformation localTransform;
    localTransform.Matrix = parentTransform.Matrix;
    if (node->has_scale) 
    {
        localTransform.Matrix *= DirectX::XMMatrixScaling(node->scale[0], node->scale[1], node->scale[2]);
    }
    if (node->has_rotation) 
    {
        DirectX::XMVECTOR quat = DirectX::XMVectorSet(node->rotation[3], node->rotation[0], node->rotation[1], node->rotation[2]);
        localTransform.Matrix *= DirectX::XMMatrixRotationQuaternion(quat);
    }
    if (node->has_translation) 
    {
        localTransform.Matrix *= DirectX::XMMatrixTranslation(node->translation[0], node->translation[1], node->translation[2]);
    }

    //localTransform.Matrix = scaleMatrix * rotationMatrix * translationMatrix;
    //Log::InfoDebug("[CGLTF] parentTransform Matrix: {}", localTransform.Matrix);

    // Process mesh if exists
    if (node->mesh)
    {
        for (size_t i = 0; i < (node->mesh->primitives_count); i++)
        {
            //Log::InfoDebug("[CGLTF] parentTransform Matrix: {}", localTransform.Matrix);
            /*Log::InfoDebug("[CGLTF] parentTransform Position: {}", localTransform.Position);
            Log::InfoDebug("[CGLTF] parentTransform Rotation: {}", localTransform.Rotation);
            Log::InfoDebug("[CGLTF] parentTransform Scale: {}", localTransform.Scale);*/

            ProcessPrimitive(&node->mesh->primitives[i], data, vertices, indices, localTransform);
        }
    }

    // Recursively process child nodes
    for (size_t i = 0; i < node->children_count; i++)
    {
        ProcessNode(node->children[i], data, vertices, indices, localTransform);
    }
}

void Spar::Model::ProcessPrimitive(cgltf_primitive *primitive, const cgltf_data *data, std::vector<SimpleVertex> &vertices, std::vector<u32> &indices, Transformation& parentTransform)
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

    prim.transform = parentTransform;

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
        SimpleVertex vertex = {};

        // Read original vertex data
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
            Log::Warn("[CGLTF] Unable to read Normal attributes!");
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

    HRESULT hr = E_FAIL;

    // map texture types to their respective textures
    std::unordered_map<TextureType, cgltf_texture_view *> textureMap;   

    //the following code for materials is very much unoptimised. It should only look for materials once, make a texture, sampler and save it in a map, not per primitive
    //#TODO

    if (material->has_pbr_metallic_roughness)
    {
        cgltf_pbr_metallic_roughness *pbr = &material->pbr_metallic_roughness;
        // Map base color texture (albedo)
        textureMap[TextureType::ALBEDO] = &pbr->base_color_texture;
        // Map metallic-roughness texture
        textureMap[TextureType::METALLIC_ROUGHNESS] = &pbr->metallic_roughness_texture;
    }

    if (material->normal_texture.texture)
    {
        // Map normal texture
        textureMap[TextureType::NORMAL] = &material->normal_texture;
    }
    if (material->emissive_texture.texture)
    {
        // Map emissive texture
        textureMap[TextureType::EMISSIVE] = &material->emissive_texture;
    }

    if (material->occlusion_texture.texture)
    {
        // Map occlusion texture
        textureMap[TextureType::AO] = &material->occlusion_texture;
    }

    // Load all textures from the map if they haven't been loaded before
    for (const auto &pair : textureMap)
    {
        std::string textureIdentifier = std::to_string(static_cast<int>(pair.first)); // Unique identifier for texture type

        if (loadedTextures.find(textureIdentifier) == loadedTextures.end()) // If not already loaded
        {
            hr = LoadMaterialTexture(mat, pair.second, pair.first);
            if (FAILED(hr))
            {
                Log::Error("[Texture] Failed to load texture of type " + textureIdentifier);
            }
            else
            {
                Log::Info("[Texture] Loaded texture of type " + textureIdentifier);
                loadedTextures.insert(textureIdentifier); // Mark this texture type as loaded
            }
        }
        else
        {
            Log::Info("[Texture] Skipping already loaded texture of type " + textureIdentifier);
        }
    }

    prim.transform = parentTransform;
    prim.startIndex = indexOffset;
    prim.startVertex = vertexOffset;
    prim.vertexCount = vertexCount;
    prim.indexCount = indexCount;

    // prim.materialIndex = m_materials.size();
    m_materials.push_back(mat);
    m_primitives.push_back(prim);
}

HRESULT Spar::Model::LoadMaterialTexture(Material &mat, cgltf_texture_view *textureView, TextureType type)
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
            mat.AlbedoPath = path;
            return S_OK;
        case TextureType::NORMAL:
            mat.NormalView = tex.m_textureView;
            mat.HasNormal = true;
            mat.NormalPath = path;
            return S_OK;
        case TextureType::METALLIC_ROUGHNESS:
            mat.MetallicRoughnessView = tex.m_textureView;
            mat.HasMetallicRoughness = true;
            mat.MetallicRoughnessPath = path;
            return S_OK;
        case TextureType::EMISSIVE:
            mat.EmissiveView = tex.m_textureView;
            mat.HasEmissive = true;
            mat.EmissivePath = path;
            return S_OK;
        case TextureType::AO:
            mat.AOView = tex.m_textureView;
            mat.HasAO = true;
            mat.AOPath = path;
            return S_OK;
        default:
            Log::Warn("[Texture] Unknown texture type.");
            return E_FAIL;
        }
    }
    else
    {
        // Handle missing texture or image
        Log::Warn("[Texture] Texture or image not found for this material.");
        return E_FAIL;
    }
}

void Spar::Model::SetBuffers()
{
    // Create vertex buffer
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = sizeof(SimpleVertex) * m_vertices.size();
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = m_vertices.data();

    HRESULT hr = renderer->m_device->CreateBuffer(&vbDesc, &vbData, m_vertexBuffer.GetAddressOf());
    if (FAILED(hr))
        Log::Error("[D3D] Failed to create vertex buffer");

    // Create index buffer
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.ByteWidth = sizeof(u32) * m_indices.size();
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = m_indices.data();

    hr = renderer->m_device->CreateBuffer(&ibDesc, &ibData, m_indexBuffer.GetAddressOf());
    if (FAILED(hr))
        Log::Error("[D3D] Failed to create index buffer");

    // create sampler state;
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Linear filtering
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;    // Wrap texture coordinates in U
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;    // Wrap texture coordinates in V
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;    // Wrap texture coordinates in W
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;                        // Default anisotropy level
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS; // Always pass comparison
    samplerDesc.BorderColor[0] = 0.0f;                    // Border color (not used with WRAP)
    samplerDesc.BorderColor[1] = 0.0f;
    samplerDesc.BorderColor[2] = 0.0f;
    samplerDesc.BorderColor[3] = 0.0f;
    samplerDesc.MinLOD = 0; // Minimum mip level
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = renderer->m_device->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());

    // Set constant buffer
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(ConstantBuffer);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = renderer->m_device->CreateBuffer(&cbDesc, nullptr, m_constantBuffer.GetAddressOf());
    if (FAILED(hr))
        Log::Error("[D3D] Failed to create world/projection/view matrix constant buffer");

    // Set material constant buffer
    D3D11_BUFFER_DESC matDesc = {};
    matDesc.ByteWidth = sizeof(MaterialConstants);
    matDesc.Usage = D3D11_USAGE_DYNAMIC;
    matDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matDesc.MiscFlags = 0;

    hr = renderer->m_device->CreateBuffer(&matDesc, nullptr, m_materialBuffer.GetAddressOf());
    if (FAILED(hr))
        Log::Error("[D3D] Failed to create material constant buffer");

    // Store counts
    m_vertexCount = m_vertices.size();
    m_indexCount = m_indices.size();

    // Clear CPU data
    /*vertices.clear();
    indices.clear();*/
}

bool Spar::Model::SetTexResources()
{
    renderer->m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    renderer->m_context->PSSetConstantBuffers(0, 1, m_materialBuffer.GetAddressOf());
    renderer->m_context->PSSetShader(renderer->m_pixelShader.Get(), nullptr, 0);

    renderer->m_context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    for (auto &mat : m_materials)
    {
        if (mat.HasAlbedo)
        {
            renderer->m_context->PSSetShaderResources(static_cast<UINT>(TextureType::ALBEDO), 1, mat.AlbedoView.GetAddressOf());
        }
        if (mat.HasNormal)
        {
            renderer->m_context->PSSetShaderResources(static_cast<UINT>(TextureType::NORMAL), 1, mat.NormalView.GetAddressOf());
        }
        if (mat.HasMetallicRoughness)
        {
            renderer->m_context->PSSetShaderResources(static_cast<UINT>(TextureType::METALLIC_ROUGHNESS), 1, mat.MetallicRoughnessView.GetAddressOf());
        }
        if (mat.HasEmissive)
        {
            renderer->m_context->PSSetShaderResources(static_cast<UINT>(TextureType::EMISSIVE), 1, mat.EmissiveView.GetAddressOf());
        }
        if (mat.HasAO)
        {
            renderer->m_context->PSSetShaderResources(static_cast<UINT>(TextureType::AO), 1, mat.AOView.GetAddressOf());
        }
    }

    return true;
}

void Spar::Model::UpdateCB(Primitive prim, DirectX::XMMATRIX worldMatrix, std::shared_ptr<Camera> camera)
{
    this->camera = camera;

    DirectX::XMMATRIX viewMatrix = camera->GetViewMatrix();
    DirectX::XMMATRIX projectionMatrix = camera->GetProjectionMatrix();

    DirectX::XMMATRIX worldViewProjMatrix = worldMatrix * viewMatrix * projectionMatrix;

    // update word/view/projection matrix
    cb.worldProjectionViewMat = worldViewProjMatrix;
    //Log::InfoDebug("[CGLTF] WorldProjViewMat", worldViewProjMatrix);
    // Map the constant buffer for writing
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = renderer->m_context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr))
    {
        memcpy(mappedResource.pData, &cb, sizeof(cb));
        renderer->m_context->Unmap(m_constantBuffer.Get(), 0);
    }
    else
    {
        Log::Error("[D3D] Failed to map constant buffer");
    }
 
    {
        MaterialConstants matColor;
        for (auto& mat : m_materials)
        {
            mat.FlatColor = { 1.0f, 1.0f, 1.0f };
        }

        matColor.ambientColor = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
        matColor.diffuseColor = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
        matColor.specularColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        matColor.specularPower = 32.0f;

        // Map the material constant buffer for writing
        hr = renderer->m_context->Map(m_materialBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (SUCCEEDED(hr))
        {
            memcpy(mappedResource.pData, &matColor, sizeof(matColor));
            renderer->m_context->Unmap(m_materialBuffer.Get(), 0);
        }
        else
        {
            Log::Error("[D3D] Failed to map material constant buffer");
        }
    }
}

void Spar::Model::Render()
{
    // Bind buffers
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    renderer->m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    renderer->m_context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // Draw each primitive
    for (const auto& prim : m_primitives)
    {
        // Set material textures
        const Material& mat = m_materials[prim.materialIndex];
        renderer->m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

        DirectX::XMMATRIX worldMatrix = prim.transform.Matrix;

        UpdateCB(prim, worldMatrix, camera);

        // Draw primitive
        renderer->m_context->DrawIndexed(prim.indexCount, prim.startIndex, prim.startVertex);
    }
}

void Spar::Model::ValidateResources()
{
    Log::Info("[CGLTF] Validating Model Resources:");
    Log::Info("[CGLTF] Vertices: " + std::to_string(m_vertices.size()));
    Log::Info("[CGLTF] Indices: " + std::to_string(m_indices.size()));
    Log::Info("[CGLTF] Materials: " + std::to_string(m_materials.size()));
    Log::Info("[CGLTF] Primitives: " + std::to_string(m_primitives.size()));

    // Check camera position
    DirectX::XMFLOAT3 pos;
    XMStoreFloat3(&pos, camera->GetPosition());
    Log::Info("[D3D] Camera Position: " + std::to_string(pos.x) + ", " +
              std::to_string(pos.y) + ", " + std::to_string(pos.z));
}