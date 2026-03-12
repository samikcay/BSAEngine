#include "BSAEngine/Renderer/VertexArray.h"
#include "../Platform/OpenGL/OpenGLVertexArray.h"

namespace BSA {

    VertexArray* VertexArray::Create() {
        return new OpenGLVertexArray();
    }

} // namespace BSA
