#pragma once

#include "BSAEngine/Math/Matrix4.h"

namespace BSA {

    class PerspectiveCamera {
    public:
        PerspectiveCamera(float fovYRad, float aspect, float zNear, float zFar);

        void SetProjection(float fovYRad, float aspect, float zNear, float zFar);

        const BSA::Math::Vector3& GetPosition() const { return m_Position; }
        void SetPosition(const BSA::Math::Vector3& position) { m_Position = position; RecalculateViewMatrix(); }

        const BSA::Math::Vector3& GetRotation() const { return m_Rotation; }
        void SetRotation(const BSA::Math::Vector3& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

        const BSA::Math::Matrix4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const BSA::Math::Matrix4& GetViewMatrix() const { return m_ViewMatrix; }
        const BSA::Math::Matrix4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

    private:
        void RecalculateViewMatrix();

    private:
        BSA::Math::Matrix4 m_ProjectionMatrix;
        BSA::Math::Matrix4 m_ViewMatrix;
        BSA::Math::Matrix4 m_ViewProjectionMatrix;

        BSA::Math::Vector3 m_Position = { 0.0f, 0.0f, 0.0f };
        BSA::Math::Vector3 m_Rotation = { 0.0f, 0.0f, 0.0f }; // (Pitch X, Yaw Y, Roll Z) angles in degrees
    };

}
