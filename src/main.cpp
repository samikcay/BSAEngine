#include "BSAEngine/Core/Application.h"
#include "BSAEngine/Core/EditorLayer.h"
#include "BSAEngine/Log/Log.h"
#include <memory>

int main() {
    BSA::Log::Init();
    BSA_ENGINE_INFO("BSAEngine Baslatiliyor...");

    auto app = std::make_unique<BSA::Application>();
    app->PushLayer(new BSA::EditorLayer());
    app->Run();

    return 0;
}
