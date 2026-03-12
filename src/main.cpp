#include "BSAEngine/Log/Log.h"
#include <iostream>

int main() {
    // Loglama sistemini başlat
    BSA::Log::Init();

    BSA_ENGINE_INFO("BSAEngine v0.1.0 baslatildi.");
    BSA_ENGINE_TRACE("Trace seviyesi mesaj.");
    BSA_ENGINE_WARN("Uyari seviyesi mesaj.");

    BSA_INFO("Client tarafinda bilgi mesaji.");

#ifdef BSA_PLATFORM_WINDOWS
    BSA_ENGINE_INFO("Platform: Windows");
#elif defined(BSA_PLATFORM_LINUX)
    BSA_ENGINE_INFO("Platform: Linux");
#elif defined(BSA_PLATFORM_MACOS)
    BSA_ENGINE_INFO("Platform: macOS");
#endif

#ifdef BSA_DEBUG
    BSA_ENGINE_WARN("Debug modu aktif.");
#endif

    return 0;
}
