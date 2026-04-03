#include "BSAEngine/Renderer/StaticMesh.h"
#include "BSAEngine/Log/Log.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unordered_map>

namespace BSA {

    // Simple OBJ file loader supporting v, vn, vt, f directives
    static bool LoadOBJ(const std::string& filepath, std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            BSA_ENGINE_ERROR("OBJ dosyasi acilamadi: {0}", filepath);
            return false;
        }

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;

        // Map to deduplicate vertices: "posIdx/texIdx/normIdx" -> index in outVertices
        std::unordered_map<std::string, uint32_t> vertexMap;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") {
                glm::vec3 pos;
                iss >> pos.x >> pos.y >> pos.z;
                positions.push_back(pos);
            } else if (prefix == "vn") {
                glm::vec3 norm;
                iss >> norm.x >> norm.y >> norm.z;
                normals.push_back(norm);
            } else if (prefix == "vt") {
                glm::vec2 tex;
                iss >> tex.x >> tex.y;
                texCoords.push_back(tex);
            } else if (prefix == "f") {
                // Parse face - supports: v, v/vt, v/vt/vn, v//vn
                std::vector<uint32_t> faceIndices;
                std::string vertexStr;
                while (iss >> vertexStr) {
                    // Check if this unique vertex combo already exists
                    auto it = vertexMap.find(vertexStr);
                    if (it != vertexMap.end()) {
                        faceIndices.push_back(it->second);
                    } else {
                        // Parse "posIdx/texIdx/normIdx"
                        int posIdx = 0, texIdx = 0, normIdx = 0;
                        // Replace '/' with spaces for easier parsing
                        std::string parsed = vertexStr;
                        for (char& c : parsed) { if (c == '/') c = ' '; }
                        std::istringstream viss(parsed);

                        size_t slashCount = 0;
                        bool doubleSlash = vertexStr.find("//") != std::string::npos;
                        for (char c : vertexStr) { if (c == '/') slashCount++; }

                        viss >> posIdx;
                        if (slashCount == 1) {
                            viss >> texIdx;
                        } else if (slashCount == 2) {
                            if (doubleSlash) {
                                viss >> normIdx;
                            } else {
                                viss >> texIdx >> normIdx;
                            }
                        }

                        Vertex vertex{};
                        if (posIdx != 0)
                            vertex.Position = positions[posIdx > 0 ? posIdx - 1 : positions.size() + posIdx];
                        if (texIdx != 0 && !texCoords.empty())
                            vertex.TexCoord = texCoords[texIdx > 0 ? texIdx - 1 : texCoords.size() + texIdx];
                        if (normIdx != 0 && !normals.empty())
                            vertex.Normal = normals[normIdx > 0 ? normIdx - 1 : normals.size() + normIdx];

                        uint32_t index = (uint32_t)outVertices.size();
                        outVertices.push_back(vertex);
                        vertexMap[vertexStr] = index;
                        faceIndices.push_back(index);
                    }
                }

                // Triangulate face (fan triangulation for convex polygons)
                for (size_t i = 1; i + 1 < faceIndices.size(); i++) {
                    outIndices.push_back(faceIndices[0]);
                    outIndices.push_back(faceIndices[i]);
                    outIndices.push_back(faceIndices[i + 1]);
                }
            }
        }

        return !outVertices.empty();
    }

    StaticMesh::StaticMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
        : m_IndexCount((uint32_t)indices.size())
    {
        m_VertexArray = std::shared_ptr<VertexArray>(VertexArray::Create());

        std::shared_ptr<VertexBuffer> vb = std::shared_ptr<VertexBuffer>(VertexBuffer::Create((float*)vertices.data(), (uint32_t)(vertices.size() * sizeof(Vertex))));
        vb->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal"   },
            { ShaderDataType::Float2, "a_TexCoord" }
        });
        m_VertexArray->AddVertexBuffer(vb);

        std::shared_ptr<IndexBuffer> ib = std::shared_ptr<IndexBuffer>(IndexBuffer::Create((uint32_t*)indices.data(), (uint32_t)indices.size()));
        m_VertexArray->SetIndexBuffer(ib);
    }

    std::shared_ptr<StaticMesh> StaticMesh::Create(const std::string& filepath) {
        std::filesystem::path path(filepath);
        std::string ext = path.extension().string();

        if (ext == ".obj" || ext == ".OBJ") {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            if (LoadOBJ(filepath, vertices, indices)) {
                BSA_ENGINE_INFO("OBJ yuklendi: {0} ({1} vertices, {2} indices)", filepath, vertices.size(), indices.size());
                return std::make_shared<StaticMesh>(vertices, indices);
            }
            BSA_ENGINE_ERROR("OBJ yuklenemedi: {0}. Cube placeholder kullaniliyor.", filepath);
        } else {
            BSA_ENGINE_WARN("Desteklenmeyen mesh formati: {0}. Cube placeholder kullaniliyor.", ext);
        }

        return CreateCube();
    }

    std::shared_ptr<StaticMesh> StaticMesh::CreateCube() {
        std::vector<Vertex> vertices = {
            // Positions          // Normals           // TexCoords
            // Front
            {{ -0.5f, -0.5f,  0.5f }, { 0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f }},
            {{  0.5f, -0.5f,  0.5f }, { 0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f }},
            {{  0.5f,  0.5f,  0.5f }, { 0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f }},
            {{ -0.5f,  0.5f,  0.5f }, { 0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f }},
            // Back
            {{ -0.5f, -0.5f, -0.5f }, { 0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f }},
            {{  0.5f, -0.5f, -0.5f }, { 0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f }},
            {{  0.5f,  0.5f, -0.5f }, { 0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f }},
            {{ -0.5f,  0.5f, -0.5f }, { 0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f }},
            // Left
            {{ -0.5f,  0.5f,  0.5f }, {-1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }},
            {{ -0.5f,  0.5f, -0.5f }, {-1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }},
            {{ -0.5f, -0.5f, -0.5f }, {-1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }},
            {{ -0.5f, -0.5f,  0.5f }, {-1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }},
            // Right
            {{  0.5f,  0.5f,  0.5f }, { 1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }},
            {{  0.5f,  0.5f, -0.5f }, { 1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }},
            {{  0.5f, -0.5f, -0.5f }, { 1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }},
            {{  0.5f, -0.5f,  0.5f }, { 1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }},
            // Top
            {{ -0.5f,  0.5f, -0.5f }, { 0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f }},
            {{  0.5f,  0.5f, -0.5f }, { 0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f }},
            {{  0.5f,  0.5f,  0.5f }, { 0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f }},
            {{ -0.5f,  0.5f,  0.5f }, { 0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f }},
            // Bottom
            {{ -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f }},
            {{  0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f }},
            {{  0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f }},
            {{ -0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f }}
        };

        std::vector<uint32_t> indices = {
            0,  1,  2,  2,  3,  0,
            4,  5,  6,  6,  7,  4,
            8,  9,  10, 10, 11, 8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20
        };

        return std::make_shared<StaticMesh>(vertices, indices);
    }

} // namespace BSA
