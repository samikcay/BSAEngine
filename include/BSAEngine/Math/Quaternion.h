#pragma once

#include "BSAEngine/Math/Math.h"
#include "BSAEngine/Math/Vector3.h"
#include "BSAEngine/Math/Matrix4.h"

namespace BSA {
    namespace Math {

        // ---------------------------------------------------------------
        // Quaternion Sınıfı
        // GLM::quat sarmalayıcısı
        // ---------------------------------------------------------------
        struct Quaternion {
            glm::quat m_Data;

            // Yapıcılar (Constructors)
            Quaternion() : m_Data(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) {} // Identity
            Quaternion(float w, float x, float y, float z) : m_Data(w, x, y, z) {}
            Quaternion(const glm::quat& q) : m_Data(q) {}
            
            // Euler acılarından (Radian) Quaternion oluşturma
            Quaternion(const Vector3& eulerAngles) {
                m_Data = glm::quat((glm::vec3)eulerAngles);
            }

            // GLM çeviriciler
            operator glm::quat&() { return m_Data; }
            operator const glm::quat&() const { return m_Data; }

            // Operatörler
            Quaternion operator*(const Quaternion& other) const {
                return Quaternion(m_Data * other.m_Data);
            }

            Quaternion& operator*=(const Quaternion& other) {
                m_Data *= other.m_Data;
                return *this;
            }

            // Vektörü Quaternion ile döndürmek için operatör
            Vector3 operator*(const Vector3& vector) const {
                return Vector3(m_Data * (glm::vec3)vector);
            }

            // -----------------------------------------------------------
            // Yardımcı Fonksiyonlar ve Dönüşümler
            // -----------------------------------------------------------

            Matrix4 ToMatrix4() const {
                return Matrix4(glm::toMat4(m_Data));
            }

            Vector3 ToEulerAngles() const {
                return Vector3(glm::eulerAngles(m_Data));
            }

            Quaternion Inverse() const {
                return Quaternion(glm::inverse(m_Data));
            }

            Quaternion Normalize() const {
                return Quaternion(glm::normalize(m_Data));
            }

            // Küresel Doğrusal İnterpolasyon (Slerp)
            static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t) {
                return Quaternion(glm::slerp((glm::quat)a, (glm::quat)b, t));
            }
        };

    } // namespace Math
} // namespace BSA
