#pragma once

#include <string>
#include "BSAEngine/Math/Matrix4.h"

namespace BSA {

    // ---------------------------------------------------------------
    // Shader (Gölgelendirici) Soyut Arayüzü
    // ---------------------------------------------------------------
    class Shader {
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void UploadUniformMat4(const std::string& name, const BSA::Math::Matrix4& matrix) = 0;

        // İleride tek bir .glsl dosyasından okuyan versiyon eklenecek:
        // static Shader* Create(const std::string& filepath);

        // Şimdilik string alarak oluşturan factory metodu:
        static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc);
    };

} // namespace BSA
