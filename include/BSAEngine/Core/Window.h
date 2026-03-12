#pragma once

#include <string>
#include <functional> // Required for std::function

namespace BSA {

    class Event;

    // ---------------------------------------------------------------
    // Pencere Özellikleri (Window Properties)
    // ---------------------------------------------------------------
    struct WindowProps {
        std::string Title;
        unsigned int Width;
        unsigned int Height;

        WindowProps(const std::string& title = "BSA Engine",
                    unsigned int width = 1280,
                    unsigned int height = 720)
            : Title(title), Width(width), Height(height) {}
    };

    // ---------------------------------------------------------------
    // İşletim Sistemi Bağımsız Pencere Arayüzü (Abstract Interface)
    // Motorun geri kalanı yalnızca bu sınıfı bilecek.
    // ---------------------------------------------------------------
    class Window {
    public:
        // EventCallbackFn zaten tanımlıydı, bu kısım değişmiyor.
        // Bu typedef, Event sistemini pencereye bağlamak için kullanılır.
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() = default;

        virtual void OnUpdate() = 0;

        virtual unsigned int GetWidth() const = 0;
        virtual unsigned int GetHeight() const = 0;

        // Pencere ile Event sistemini haberleştirmek için callback set edilir
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        // Platform spesifik native window pointer'ını döndürür (GFLWwindow* vs.)
        virtual void* GetNativeWindow() const = 0;

        // Factory metodu: Aktif platforma göre uygun pencereyi yaratır
        static Window* Create(const WindowProps& props = WindowProps());
    };

} // namespace BSA
