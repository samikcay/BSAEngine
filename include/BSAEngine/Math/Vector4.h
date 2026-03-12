#pragma once

#include "BSAEngine/Math/Math.h"
#include "BSAEngine/Math/Vector3.h"
#include <iostream>

namespace BSA {
    namespace Math {

        // ---------------------------------------------------------------
        // Vector4 Sınıfı
        // GLM::vec4 sarmalayıcısı
        // ---------------------------------------------------------------
        struct Vector4 {
            float x, y, z, w;

            Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
            Vector4(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
            Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
            Vector4(const Vector3& v, float _w) : x(v.x), y(v.y), z(v.z), w(_w) {}

            // GLM çeviriciler
            Vector4(const glm::vec4& vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}
            operator glm::vec4() const { return glm::vec4(x, y, z, w); }

            // Matematik Operatörleri
            Vector4 operator+(const Vector4& other) const { return Vector4(x + other.x, y + other.y, z + other.z, w + other.w); }
            Vector4 operator-(const Vector4& other) const { return Vector4(x - other.x, y - other.y, z - other.z, w - other.w); }
            Vector4 operator*(float scalar) const { return Vector4(x * scalar, y * scalar, z * scalar, w * scalar); }
            Vector4 operator/(float scalar) const { return Vector4(x / scalar, y / scalar, z / scalar, w / scalar); }

            Vector4& operator+=(const Vector4& other) { x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
            Vector4& operator-=(const Vector4& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
            Vector4& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
            Vector4& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }

            bool operator==(const Vector4& other) const { return x == other.x && y == other.y && z == other.z && w == other.w; }
            bool operator!=(const Vector4& other) const { return !(*this == other); }

            // Yardımcı Fonksiyonlar
            float Length() const { return glm::length((glm::vec4)*this); }
            Vector4 Normalize() const { return glm::normalize((glm::vec4)*this); }
            static float Dot(const Vector4& a, const Vector4& b) { return glm::dot((glm::vec4)a, (glm::vec4)b); }

            friend std::ostream& operator<<(std::ostream& stream, const Vector4& vector) {
                stream << "Vector4(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";
                return stream;
            }
        };

    } // namespace Math
} // namespace BSA
