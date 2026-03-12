#pragma once

#include "BSAEngine/Math/Math.h"
#include "BSAEngine/Math/Vector2.h"
#include <iostream>

namespace BSA {
    namespace Math {

        // ---------------------------------------------------------------
        // Vector3 Sınıfı
        // GLM::vec3 sarmalayıcısı
        // ---------------------------------------------------------------
        struct Vector3 {
            float x, y, z;

            Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
            Vector3(float scalar) : x(scalar), y(scalar), z(scalar) {}
            Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
            Vector3(const Vector2& v, float _z) : x(v.x), y(v.y), z(_z) {}

            // GLM çeviriciler
            Vector3(const glm::vec3& vec) : x(vec.x), y(vec.y), z(vec.z) {}
            operator glm::vec3() const { return glm::vec3(x, y, z); }

            // Matematik Operatörleri
            Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
            Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
            Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
            Vector3 operator/(float scalar) const { return Vector3(x / scalar, y / scalar, z / scalar); }

            Vector3& operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
            Vector3& operator-=(const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
            Vector3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
            Vector3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

            bool operator==(const Vector3& other) const { return x == other.x && y == other.y && z == other.z; }
            bool operator!=(const Vector3& other) const { return !(*this == other); }

            // Yardımcı Fonksiyonlar
            float Length() const { return glm::length((glm::vec3)*this); }
            Vector3 Normalize() const { return glm::normalize((glm::vec3)*this); }
            static float Dot(const Vector3& a, const Vector3& b) { return glm::dot((glm::vec3)a, (glm::vec3)b); }
            static Vector3 Cross(const Vector3& a, const Vector3& b) { return glm::cross((glm::vec3)a, (glm::vec3)b); }

            friend std::ostream& operator<<(std::ostream& stream, const Vector3& vector) {
                stream << "Vector3(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
                return stream;
            }
        };

    } // namespace Math
} // namespace BSA
