#pragma once

#include <string>
#include <vector>

#include "Buffer.h"
#include "cgltf.h"
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
    AO
};

struct SimpleVertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT2 Tex;
    DirectX::XMFLOAT3 Normal;
};

struct MaterialConstants {
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

    wrl::ComPtr<ID3D11SamplerState> samplerState;
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
};

namespace Spar
{
    class Model
    {
    public:
        Model();
        ~Model();
        void LoadModel(std::shared_ptr<Spar::Renderer> renderer, std::string path);
        void SetBuffers();
        bool SetTexResources();
        void UpdateCB(std::shared_ptr<Spar::Renderer> renderer, std::shared_ptr<Spar::Camera> camera, static f64 dt);

        void Render();

    private:
        void ProcessNode(cgltf_node *node, const cgltf_data *data, std::vector<SimpleVertex> &vertices, std::vector<u32> &indices);
        void ProcessMesh(cgltf_mesh *mesh, const cgltf_data *data, std::vector<SimpleVertex> &vertices, std::vector<u32> &indices);
        void ProcessPrimitive(cgltf_primitive *primitive, const cgltf_data *data, std::vector<SimpleVertex> &vertices, std::vector<u32> &indices);

        void LoadMaterialTexture(Material &mat, cgltf_texture_view *view, TextureType type);

        ConstantBuffer cb;
        MaterialConstants matColor;


    public:
        u32 vertexOffset = 0;
        u32 indexOffset = 0;
        cgltf_data *m_model;
        wrl::ComPtr<ID3D11ShaderResourceView> m_textureView = nullptr;
        std::string m_dirPath;
        std::string name;
        std::vector<SimpleVertex> vertices;
        std::vector<u32> indices;
        std::vector<Primitive> primitives;
        std::vector<Material> materials;

        std::shared_ptr<Spar::Renderer> renderer;
        std::shared_ptr<Spar::Camera> camera;

    private:
        wrl::ComPtr<ID3D11Buffer> m_vertexBuffer;
        wrl::ComPtr<ID3D11Buffer> m_indexBuffer;
        wrl::ComPtr<ID3D11Buffer> m_constantBuffer;
        UINT m_vertexCount;
        UINT m_indexCount;

        wrl::ComPtr<ID3D11Buffer> m_materialBuffer;
    };
}