#include "BSAEngine/Core/Application.h"
#include "BSAEngine/Core/EntryPoint.h"
#include "BSAEngine/Core/Input.h"
#include "BSAEngine/Core/KeyCodes.h"
#include "BSAEngine/Core/MouseButtonCodes.h"
#include "BSAEngine/Math/Vector2.h"
#include "BSAEngine/Math/Vector3.h"
#include "BSAEngine/Math/Vector4.h"
#include "BSAEngine/Math/Matrix4.h"
#include "BSAEngine/Math/Quaternion.h"
#include "BSAEngine/Log/Log.h"
#include "BSAEngine/Events/KeyEvent.h"

// ---------------------------------------------------------------
// İstemci (Client - Oyun) Uygulaması
// ---------------------------------------------------------------

class Sandbox : public BSA::Application {
public:
    Sandbox() {
        BSA_INFO("Sandbox Uygulamasi (Oyun) Olusturuldu!");

        // Özel Matematik Sınıfları Testi
        BSA::Math::Vector4 engineVector(1.0f, 0.0f, 0.0f, 1.0f);
        
        // Transform matrisi
        BSA::Math::Matrix4 transform = BSA::Math::Matrix4::Translate(BSA::Math::Matrix4(1.0f), BSA::Math::Vector3(5.0f, 0.0f, 0.0f));
        
        // Quaternion (Y ekseninde 90 derece dönüş) - (Radyan ister)
        float angleRad = glm::radians(90.0f);
        BSA::Math::Quaternion rotation(BSA::Math::Vector3(0.0f, angleRad, 0.0f));
        
        // Dönüşümü ve dönmeyi (Rotation) birleştirmek
        BSA::Math::Matrix4 finalTransform = transform * rotation.ToMatrix4();
        
        // Y ekseninde 90 derece dönen ve X ekseninde 5 birim ilerleyen (X=1) vektör
        BSA::Math::Vector4 result = finalTransform * engineVector;
        
        BSA_INFO("Motor Quaternion/Matris Testi: ({0}, {1}, {2}) -> ({3}, {4}, {5})", 
                 engineVector.x, engineVector.y, engineVector.z, 
                 result.x, result.y, result.z);
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
