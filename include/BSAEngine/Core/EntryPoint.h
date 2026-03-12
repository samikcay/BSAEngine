#pragma once

#include "BSAEngine/Log/Log.h"
#include "BSAEngine/Core/Application.h"

// ---------------------------------------------------------------
// Motorun Giriş Noktası (Entry Point)
// Kullanıcı kendi oyununda (client tarafı) sadece `main` yazmak 
// yerine `CreateApplication()` döndürecek. Biz main'i burada yöneteceğiz.
// ---------------------------------------------------------------

#ifdef BSA_PLATFORM_WINDOWS

extern BSA::Application* BSA::CreateApplication();

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    // 1. Loglama sistemini başlat
    BSA::Log::Init();
    BSA_ENGINE_INFO("BSAEngine v0.1.0 baslatildi (Entry Point uzerinden).");

    // 2. Client tarafından motor nesnesini (Application) talep et
    auto app = BSA::CreateApplication();

    // 3. Ana oyun döngüsünü çalıştır
    app->Run();

    // 4. Kapanış
    delete app;

    return 0;
}

#else
    #error BSAEngine su an sadece Windows destekliyor! (Entry Point icin)
#endif
