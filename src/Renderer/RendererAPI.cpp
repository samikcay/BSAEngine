#include "BSAEngine/Renderer/RendererAPI.h"
#include "../Platform/OpenGL/OpenGLRendererAPI.h"

namespace BSA {

    RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

    std::unique_ptr<RendererAPI> RendererAPI::Create() {
        switch (s_API) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_unique<OpenGLRendererAPI>();
        }

        return nullptr;
    }

} // namespace BSA
