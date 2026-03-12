#pragma once

#include <utility>

namespace BSA {

    // ---------------------------------------------------------------
    // Girdi (Input) Polling Arayüzü
    // Bu sınıf, oyunun herhangi bir yerinden (Event beklemeden) 
    // anlık tuş veya fare durumunu sorgulamak için kullanılır.
    // ---------------------------------------------------------------
    class Input {
    public:
        // Herhangi bir tuşa basılıp basılmadığını kontrol eder.
        inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

        // Farenin sol(0), sağ(1), orta(2) tuşlarına basılıp basılmadığını kontrol eder.
        inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }

        // Farenin anlık ekran koordinatlarını (X, Y) döndürür.
        inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
        inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
        inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }

    protected:
        virtual bool IsKeyPressedImpl(int keycode) = 0;
        virtual bool IsMouseButtonPressedImpl(int button) = 0;
        virtual std::pair<float, float> GetMousePositionImpl() = 0;
        virtual float GetMouseXImpl() = 0;
        virtual float GetMouseYImpl() = 0;

    private:
        static Input* s_Instance;
    };

} // namespace BSA
