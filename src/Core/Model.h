#pragma once

#include "cgltf.h"
#include "WinUtil.h"
#include "DDSTextureLoader.h"
#include "string"
#include "Buffer.h"
#include <vector>
#include "Log.h"

#define MAX_MESHLET_TRIANGLES 124
#define MAX_MESHLET_VERTICES 64

struct AABB
{
    DirectX::XMFLOAT3 Min;
    DirectX::XMFLOAT3 Max;

    DirectX::XMFLOAT3 Center;
    DirectX::XMFLOAT3 Extent;
};

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

struct MeshletBounds
{
    /* bounding sphere, useful for frustum and occlusion culling */
	DirectX::XMFLOAT3 center;
	float radius;

	/* normal cone, useful for backface culling */
	DirectX::XMFLOAT3 cone_apex;
	DirectX::XMFLOAT3 cone_axis;
	float cone_cutoff; /* = cos(angle/2) */
};

struct Primitive
{
    wrl::ComPtr<ID3D11Buffer> VertexBuffer = nullptr;
    wrl::ComPtr<ID3D11Buffer> IndexBuffer = nullptr;
    wrl::ComPtr<ID3D11Buffer> MeshletBuffer = nullptr;
    wrl::ComPtr<ID3D11Buffer> MeshletVertices = nullptr;
    wrl::ComPtr<ID3D11Buffer> MeshletTriangles = nullptr;
    wrl::ComPtr<ID3D11Buffer> MeshletBounds = nullptr;

    uint32_t VertexCount;
    uint32_t IndexCount;
    uint32_t MeshletCount;

    std::string name;

    uint32_t MaterialIndex;

    AABB BoundingBox = {};
};


namespace Spar
{
    class Model
    {
    public:
        Model();
        ~Model();
        void LoadModel(std::string path);

        cgltf_data* m_model;
        wrl::ComPtr<ID3D11ShaderResourceView> m_textureView = nullptr;

        DrawableAssets assets{};

    private:
        void ProcessNode(cgltf_node* node, const cgltf_data* data, std::vector<SimpleVertex>& vertices, std::vector<u32>& indices);
        void ProcessMesh(cgltf_mesh* mesh, const cgltf_data* data, std::vector<SimpleVertex>& vertices, std::vector<u32>& indices);
        void ProcessPrimitive(cgltf_primitive* primitive, const cgltf_data* data, std::vector<SimpleVertex>& vertices, std::vector<u32>& indices);
        
    public:
        std::string m_dirPath;
        std::string name;
        std::vector<SimpleVertex> vertices;
        std::vector<u32> indices;
    };
}