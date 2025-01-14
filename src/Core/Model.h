#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <cgltf.h>

#include "Buffer.h"
#include "WinUtil.h"

namespace Spar
{
    class Renderer;
    class Camera;
}

enum class TextureType
{
    ALBEDO,
    NORMAL,
    METALLIC_ROUGHNESS,
    EMISSIVE,
    AO,
    SPECULAR,
    DISPLACEMENT,
    OPACITY,
    GLOSSINESS,
    HEIGHT,
    CUBEMAP,
    BRDF_LUT,
    SPECULAR_GLOSSINESS
};

struct Transformation
{
    Transformation()
    {
        Matrix = DirectX::XMMatrixIdentity();
        DirectX::XMVECTOR Position = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
        DirectX::XMVECTOR Rotation = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        DirectX::XMVECTOR Scale = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
    }

    DirectX::XMMATRIX Matrix = DirectX::XMMatrixIdentity();
    DirectX::XMVECTOR Position = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
    DirectX::XMVECTOR Rotation = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    DirectX::XMVECTOR Scale = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
};

struct SimpleVertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT2 Tex;
    DirectX::XMFLOAT3 Normal;
};

struct MaterialConstants
{
    DirectX::XMFLOAT4 ambientColor;
    DirectX::XMFLOAT4 diffuseColor;
    DirectX::XMFLOAT4 specularColor;
    float specularPower;
    float padding[3]; // Padding to satisfy 16-byte alignment
};

struct Material
{
    bool HasAlbedo = false;
    bool HasNormal = false;
    bool HasMetallicRoughness = false;
    bool HasEmissive = false;
    bool HasAO = false;

    std::string AlbedoPath;
    std::string NormalPath;
    std::string MetallicRoughnessPath;
    std::string EmissivePath;
    std::string AOPath;

    wrl::ComPtr<ID3D11ShaderResourceView> AlbedoView = nullptr;
    wrl::ComPtr<ID3D11ShaderResourceView> NormalView = nullptr;
    wrl::ComPtr<ID3D11ShaderResourceView> MetallicRoughnessView = nullptr;
    wrl::ComPtr<ID3D11ShaderResourceView> EmissiveView = nullptr;
    wrl::ComPtr<ID3D11ShaderResourceView> AOView = nullptr;

    DirectX::XMFLOAT3 FlatColor;
};

struct Primitive
{
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    std::string name = "";
    uint32_t materialIndex = 0;
    uint32_t startIndex = 0;
    uint32_t startVertex = 0;

    Transformation transform;
};

namespace Spar
{
    class Model
    {
    public:
        Model();
        ~Model();
        void LoadModel(std::shared_ptr<Spar::Renderer> renderer, std::shared_ptr<Camera> camera, std::string path);
        void SetBuffers();
        bool SetTexResources(uint32_t materialIndex);
        void UpdateCB(Primitive prim, std::shared_ptr<Camera> camera);

        void Render();

    private:
        void ProcessNode(cgltf_node *node, const cgltf_data *data, std::vector<SimpleVertex> &vertices, std::vector<u32> &indices, Transformation& parentTransform);
        void ProcessPrimitive(cgltf_primitive *primitive, const cgltf_data *data, std::vector<SimpleVertex> &vertices, std::vector<u32> &indices, Transformation& parentTransform);

        HRESULT LoadMaterialTexture(Material &mat, cgltf_texture_view *view, TextureType type);
        void ValidateResources() const;

        ConstantBuffer cb;
        MaterialConstants matColor;

    public:
        u32 vertexOffset = 0;
        u32 indexOffset = 0;
        cgltf_data *m_model;
        wrl::ComPtr<ID3D11ShaderResourceView> m_textureView = nullptr;
        std::string m_dirPath;
        std::string name;
        std::vector<SimpleVertex> m_vertices;
        std::vector<u32> m_indices;
        std::vector<Primitive> m_primitives;
        std::vector<Material> m_materials;

        std::shared_ptr<Spar::Renderer> renderer;
        std::shared_ptr<Spar::Camera> camera;

    private:
        wrl::ComPtr<ID3D11Buffer> m_vertexBuffer;
        wrl::ComPtr<ID3D11Buffer> m_indexBuffer;
        wrl::ComPtr<ID3D11Buffer> m_constantBuffer;
        UINT m_vertexCount;
        UINT m_indexCount;

        wrl::ComPtr<ID3D11Buffer> m_materialBuffer;
        wrl::ComPtr<ID3D11SamplerState> m_samplerState = nullptr;

        std::unordered_set<std::string> loadedTextures; // To track loaded textures
        std::unordered_map<cgltf_material*, int> materialLookup;

    };
}