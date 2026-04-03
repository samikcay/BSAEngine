#pragma once

#include "BSAEngine/Math/Matrix4.h"

namespace BSA {

    class PerspectiveCamera {
    public:
        enum class ProjectionType { Perspective = 0, Orthographic = 1 };

        PerspectiveCamera(float fovYRad, float aspect, float zNear, float zFar);

        void SetProjection(float fovYRad, float aspect, float zNear, float zFar);

        float GetFOV() const { return m_Fov; }
        void SetFOV(float fov) { m_Fov = fov; RecalculateProjection(); }
        float GetNear() const { return m_Near; }
        void SetNear(float zNear) { m_Near = zNear; RecalculateProjection(); }
        float GetFar() const { return m_Far; }
        void SetFar(float zFar) { m_Far = zFar; RecalculateProjection(); }

        ProjectionType GetProjectionType() const { return m_ProjectionType; }
        void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }

        // Ortho helpers
        float GetOrthographicSize() const { return m_OrthoSize; }
        void SetOrthographicSize(float size) { m_OrthoSize = size; RecalculateProjection(); }
        float GetOrthographicNearClip() const { return m_Near; }
        void SetOrthographicNearClip(float zNear) { m_Near = zNear; RecalculateProjection(); }
        float GetOrthographicFarClip() const { return m_Far; }
        void SetOrthographicFarClip(float zFar) { m_Far = zFar; RecalculateProjection(); }

        float GetPerspectiveVerticalFOV() const { return m_Fov; }
        void SetPerspectiveVerticalFOV(float fov) { m_Fov = fov; RecalculateProjection(); }
        float GetPerspectiveNearClip() const { return m_Near; }
        void SetPerspectiveNearClip(float zNear) { m_Near = zNear; RecalculateProjection(); }
        float GetPerspectiveFarClip() const { return m_Far; }
        void SetPerspectiveFarClip(float zFar) { m_Far = zFar; RecalculateProjection(); }

        const BSA::Math::Vector3& GetPosition() const { return m_Position; }
        void SetPosition(const BSA::Math::Vector3& position) { m_Position = position; RecalculateViewMatrix(); }

        const BSA::Math::Vector3& GetRotation() const { return m_Rotation; }
        void SetRotation(const BSA::Math::Vector3& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

        const BSA::Math::Matrix4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const BSA::Math::Matrix4& GetViewMatrix() const { return m_ViewMatrix; }
        const BSA::Math::Matrix4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

    private:
        void RecalculateProjection();
        void RecalculateViewMatrix();

    private:
        BSA::Math::Matrix4 m_ProjectionMatrix;
        BSA::Math::Matrix4 m_ViewMatrix;
        BSA::Math::Matrix4 m_ViewProjectionMatrix;

        float m_Fov, m_Aspect, m_Near, m_Far;
        float m_OrthoSize = 10.0f;
        ProjectionType m_ProjectionType = ProjectionType::Perspective;

        BSA::Math::Vector3 m_Position = { 0.0f, 0.0f, 0.0f };
        BSA::Math::Vector3 m_Rotation = { 0.0f, 0.0f, 0.0f }; 
    };

}
