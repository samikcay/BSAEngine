#pragma once

// ---------------------------------------------------------------
// BSAEngine Core Math Header
// GLM kütüphanesi motor içerisinde kullanılmak üzere buradan dahil edilir.
// Projenin geri kalanında bu dosya include edilecek.
// ---------------------------------------------------------------

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

// Vektörler, Matrisler, Kuaterniyonlar vs. genellikle 
// GLM namespace'inde (glm::vec3, glm::mat4) kullanılacak.
// Gerekirse ileride kendi sarmalayıcı (wrapper) sınıflarımız yazılabilir,
// ancak GLM sektörel standart olduğu için genelde typedef'ler yeterli olur.

namespace BSA {
    namespace Math {

        // İlerisi için özel matematik yardımcı fonksiyonları:
        static inline float Radians(float degrees) {
            return glm::radians(degrees);
        }

        static inline float Degrees(float radians) {
            return glm::degrees(radians);
        }

    }
}
