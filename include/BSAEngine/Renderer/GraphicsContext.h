#pragma once

namespace BSA {

    // ---------------------------------------------------------------
    // Grafik Context (Renderer Context) Soyut Arayüzü
    // Hangi grafik API'sinin (OpenGL, DirectX, Vulkan) kullanıldığına
    // bakmaksızın motorun iletişim kuracağı ortak arayüz.
    // ---------------------------------------------------------------
    class GraphicsContext {
    public:
        virtual ~GraphicsContext() = default;

        // Context'i yükler ve başlatır (Örn: GLAD yüklemesi)
        virtual void Init() = 0;

        // Arka plan buffer'ını ön plana kopyalar (Frame bitiminde çağrılır)
        virtual void SwapBuffers() = 0;
    };

} // namespace BSA
