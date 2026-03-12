#pragma once

#include "BSAEngine/Math/Math.h"
#include <iostream>

namespace BSA {
    namespace Math {

        // ---------------------------------------------------------------
        // Vector2 Sınıfı
        // GLM::vec2 sarmalayıcısı
        // ---------------------------------------------------------------
        struct Vector2 {
            float x, y;

            Vector2() : x(0.0f), y(0.0f) {}
            Vector2(float scalar) : x(scalar), y(scalar) {}
            Vector2(float _x, float _y) : x(_x), y(_y) {}

            // GLM çeviriciler
            Vector2(const glm::vec2& vec) : x(vec.x), y(vec.y) {}
            operator glm::vec2() const { return glm::vec2(x, y); }

            // Matematik Operatörleri
            Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
            Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
            Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
            Vector2 operator/(float scalar) const { return Vector2(x / scalar, y / scalar); }

            Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
            Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
            Vector2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
            Vector2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

            bool operator==(const Vector2& other) const { return x == other.x && y == other.y; }
            bool operator!=(const Vector2& other) const { return !(*this == other); }

            // Yardımcı Fonksiyonlar
            float Length() const { return glm::length((glm::vec2)*this); }
            Vector2 Normalize() const { return glm::normalize((glm::vec2)*this); }
            static float Dot(const Vector2& a, const Vector2& b) { return glm::dot((glm::vec2)a, (glm::vec2)b); }

            friend std::ostream& operator<<(std::ostream& stream, const Vector2& vector) {
                stream << "Vector2(" << vector.x << ", " << vector.y << ")";
                return stream;
            }
        };

    } // namespace Math
} // namespace BSA
