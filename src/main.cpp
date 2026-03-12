#include "BSAEngine/Core/Application.h"
#include "BSAEngine/Core/EntryPoint.h"
#include "BSAEngine/Core/Input.h"
#include "BSAEngine/Core/KeyCodes.h"
#include "BSAEngine/Core/MouseButtonCodes.h"
#include "BSAEngine/Log/Log.h"
#include "BSAEngine/Events/KeyEvent.h"

// ---------------------------------------------------------------
// İstemci (Client - Oyun) Uygulaması
// ---------------------------------------------------------------

class Sandbox : public BSA::Application {
public:
    Sandbox() {
        BSA_INFO("Sandbox Uygulamasi (Oyun) Olusturuldu!");
    }

    ~Sandbox() {
        BSA_INFO("Sandbox Uygulamasi (Oyun) Kapatiliyor...");
    }

    // Uygulamanın OnUpdate döngüsüne (şimdilik yok) simülasyon olarak 
    // veya Event sistemini yakalayan bir metod override edilebilir.
    // Şimdilik test amaçlı Input sistemini kullanmak için Event'leri veya Run döngüsünü kullanabiliriz.
};

// EntryPoint.h bu fonksiyonu arayacak ve oyunu başlatacak
BSA::Application* BSA::CreateApplication() {
    return new Sandbox();
}
