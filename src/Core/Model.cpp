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

    void Model::LoadModel(std::string path)
    {
        cgltf_options options = {};
        cgltf_data* data = nullptr;
        cgltf_result result = cgltf_parse_file(&options, path.c_str(), &data);

        if (result != cgltf_result_success)
        {
            Log::Error("Failed to parse gltf file");
        }

        result = cgltf_load_buffers(&options, data, path.c_str());

        if (result != cgltf_result_success)
        {
            cgltf_free(data);
            Log::Error("Failed to load buffers");
        }

        cgltf_scene* scene = data->scene;

        m_dirPath = path.substr(0, path.find_last_of("/"));

        for (size_t i = 0; i < scene->nodes_count; i++)
        {
            ProcessNode(scene->nodes[i], data, vertices, indices);
        }

        if (!scene)
        {
            Log::Error("No scene found in gltf file");
        }

        Log::Info("Successfully loaded gltf file");

        cgltf_free(data);
    }

    void Model::ProcessMesh(cgltf_mesh* mesh, const cgltf_data* data, std::vector<SimpleVertex>& vertices, std::vector<u32>& indices)
    {
        for (size_t i = 0; i < mesh->primitives_count; i++)
        {
            ProcessPrimitive(&mesh->primitives[i], data, vertices, indices);
        }
    }

    void Model::ProcessNode(cgltf_node* node, const cgltf_data* data, std::vector<SimpleVertex>& vertices, std::vector<u32>& indices)
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

    void Model::ProcessPrimitive(cgltf_primitive* primitive, const cgltf_data* data, std::vector<SimpleVertex>& vertices, std::vector<u32>& indices)
    {
        if (primitive->type != cgltf_primitive_type_triangles)
        {
            Log::Error("Primitive type is not triangles");
            return;
        }

        if (primitive->indices == nullptr)
        {
            Log::Error("Primitive has no indices");
            return;
        }

        if (primitive->material == nullptr)
        {
            Log::Error("Primitive has no material");
            return;
        }

        Primitive prim;

        // Get attributes
        cgltf_attribute* pos_attribute = nullptr;
        cgltf_attribute* uv_attribute = nullptr;
        cgltf_attribute* norm_attribute = nullptr;

        for (int i = 0; i < primitive->attributes_count; i++)
        {
            if (!strcmp(primitive->attributes[i].name, "POSITION"))
            {
                pos_attribute = &primitive->attributes[i];
            }
            if (!strcmp(primitive->attributes[i].name, "TEXCOORD_0"))
            {
                uv_attribute = &primitive->attributes[i];
            }
            if (!strcmp(primitive->attributes[i].name, "NORMAL"))
            {
                norm_attribute = &primitive->attributes[i];
            }
        }
        if (!pos_attribute || !uv_attribute || !norm_attribute)
        {
            Log::Warn("[CGLTF] Missing attributes in primitive");
            return;
        }

        // Load vertices
        int vertexCount = pos_attribute->data->count;
        int indexCount = primitive->indices->count;

        std::vector<SimpleVertex> vertices = {};
        std::vector<uint32_t> indices = {};

        for (int i = 0; i < vertexCount; i++)
        {
            SimpleVertex vertex;

            if (!cgltf_accessor_read_float(pos_attribute->data, i, &vertex.Pos.x, 3))
            {
                Log::Warn("[CGLTF] Unable to read Position attributes!");
            }
            if (!cgltf_accessor_read_float(uv_attribute->data, i, &vertex.Tex.x, 2))
            {
                Log::Warn("[CGLTF] Unable to read Texture attributes!");
            }
            if (!cgltf_accessor_read_float(norm_attribute->data, i, &vertex.Normal.x, 3))
            {
                Log::Warn("[CGLTF] IUnable to read Normal attributes!");
            }

            vertices.push_back(vertex);
        }

        for (int i = 0; i < indexCount; i++)
        {
            indices.push_back(cgltf_accessor_read_index(primitive->indices, i));
        }

        prim.VertexCount = vertices.size();
        prim.IndexCount = indices.size();

    };
};
