#include "BSAEngine/Renderer/RenderCommand.h"

namespace BSA {

    std::unique_ptr<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

} // namespace BSA
