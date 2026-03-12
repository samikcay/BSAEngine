#pragma once

#include "BSAEngine/Renderer/Buffer.h"
#include <memory>
#include <vector>

namespace BSA {

    // ---------------------------------------------------------------
    // Vertex Array Soyut Arayüzü
    // VBO ve IBO'ları bir araya getirip attribute bağlantılarını tutar.
    // ---------------------------------------------------------------
    class VertexArray {
    public:
        virtual ~VertexArray() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        // VertexArray, objelerin ömürlerini kendi içinde Unique/Shared ptr 
        // veya nesne kopyası ile tutabilir. Basitlik açısından ref tutuyoruz.
        virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
        virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

        virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const = 0;
        virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const = 0;

        // VAO Factory Method
        static VertexArray* Create();
    };

} // namespace BSA
