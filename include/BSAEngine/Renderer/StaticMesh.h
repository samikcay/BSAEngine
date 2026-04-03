#pragma once

#include "BSAEngine/Asset/Asset.h"
#include "BSAEngine/Renderer/VertexArray.h"
#include "BSAEngine/Renderer/Buffer.h"
#include <vector>
#include <glm/glm.hpp>

namespace BSA {

    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
    };

    class StaticMesh : public Asset {
    public:
        StaticMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        virtual ~StaticMesh() = default;

        void Bind() const { m_VertexArray->Bind(); }
        void Unbind() const { m_VertexArray->Unbind(); }

        const std::shared_ptr<VertexArray>& GetVertexArray() const { return m_VertexArray; }
        uint32_t GetIndexCount() const { return m_IndexCount; }

        virtual AssetType GetType() const override { return AssetType::StaticMesh; }

        static std::shared_ptr<StaticMesh> Create(const std::string& filepath);
        static std::shared_ptr<StaticMesh> CreateCube();

    private:
        std::shared_ptr<VertexArray> m_VertexArray;
        uint32_t m_IndexCount;
    };

} // namespace BSA
