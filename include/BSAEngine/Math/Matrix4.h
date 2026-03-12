#pragma once

#include "BSAEngine/Math/Math.h"
#include "BSAEngine/Math/Vector3.h"
#include "BSAEngine/Math/Vector4.h"

namespace BSA {
    namespace Math {

        // ---------------------------------------------------------------
        // Matrix4 Sınıfı (4x4 Matris)
        // GLM::mat4 sarmalayıcısı
        // ---------------------------------------------------------------
        struct Matrix4 {
            glm::mat4 m_Data;

            // Yapıcılar (Constructors)
            Matrix4() : m_Data(1.0f) {} // Birim Matris (Identity Matrix) varsayılan
            Matrix4(float diagonal) : m_Data(diagonal) {}
            Matrix4(const glm::mat4& mat) : m_Data(mat) {}

            // Sütun erişimi (GLM sütun tabanlı - column-major - çalışır)
            Vector4 operator[](int index) const { return Vector4(m_Data[index]); }
            Vector4& operator[](int index) { return reinterpret_cast<Vector4&>(m_Data[index]); }

            // GLM çeviriciler
            operator glm::mat4&() { return m_Data; }
            operator const glm::mat4&() const { return m_Data; }

            // Matematik Operatörleri
            Matrix4 operator*(const Matrix4& other) const { return Matrix4(m_Data * other.m_Data); }
            Matrix4& operator*=(const Matrix4& other) { m_Data *= other.m_Data; return *this; }

            Vector4 operator*(const Vector4& vec) const { return Vector4(m_Data * (glm::vec4)vec); }

            bool operator==(const Matrix4& other) const { return m_Data == other.m_Data; }
            bool operator!=(const Matrix4& other) const { return !(*this == other); }

            // -----------------------------------------------------------
            // Dönüşüm (Transformation) Fonksiyonları
            // -----------------------------------------------------------

            static Matrix4 Translate(const Matrix4& transform, const Vector3& translation) {
                return Matrix4(glm::translate((glm::mat4)transform, (glm::vec3)translation));
            }

            // Radian cinsinden açı alır
            static Matrix4 Rotate(const Matrix4& transform, float angleRad, const Vector3& axis) {
                return Matrix4(glm::rotate((glm::mat4)transform, angleRad, (glm::vec3)axis));
            }

            static Matrix4 Scale(const Matrix4& transform, const Vector3& scale) {
                return Matrix4(glm::scale((glm::mat4)transform, (glm::vec3)scale));
            }

            // -----------------------------------------------------------
            // Kamera Matris Fonksiyonları
            // -----------------------------------------------------------

            static Matrix4 Orthographic(float left, float right, float bottom, float top, float zNear, float zFar) {
                return Matrix4(glm::ortho(left, right, bottom, top, zNear, zFar));
            }

            static Matrix4 Perspective(float fovYRad, float aspect, float zNear, float zFar) {
                return Matrix4(glm::perspective(fovYRad, aspect, zNear, zFar));
            }

            static Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
                return Matrix4(glm::lookAt((glm::vec3)eye, (glm::vec3)center, (glm::vec3)up));
            }

            // Ters matris alma işlemi (Inverse)
            Matrix4 Inverse() const {
                return Matrix4(glm::inverse(m_Data));
            }

            // Pointer döndür (OpenGL/Vulkan'a direkt veri göndermek için)
            const float* ValuePtr() const {
                return glm::value_ptr(m_Data);
            }
        };

    } // namespace Math
} // namespace BSA
