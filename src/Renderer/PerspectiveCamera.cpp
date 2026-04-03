#include "BSAEngine/Renderer/PerspectiveCamera.h"
#include "BSAEngine/Math/Math.h"
#include <glm/gtc/matrix_transform.hpp>

namespace BSA {

    PerspectiveCamera::PerspectiveCamera(float fovYRad, float aspect, float zNear, float zFar)
        : m_Fov(fovYRad), m_Aspect(aspect), m_Near(zNear), m_Far(zFar)
    {
        RecalculateProjection();
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::SetProjection(float fovYRad, float aspect, float zNear, float zFar) {
        m_Fov = fovYRad;
        m_Aspect = aspect;
        m_Near = zNear;
        m_Far = zFar;
        RecalculateProjection();
    }

    void PerspectiveCamera::RecalculateProjection() {
        if (m_ProjectionType == ProjectionType::Perspective) {
            m_ProjectionMatrix = BSA::Math::Matrix4::Perspective(m_Fov, m_Aspect, m_Near, m_Far);
        } else {
            float orthoLeft = -m_OrthoSize * m_Aspect * 0.5f;
            float orthoRight = m_OrthoSize * m_Aspect * 0.5f;
            float orthoBottom = -m_OrthoSize * 0.5f;
            float orthoTop = m_OrthoSize * 0.5f;
            
            // Note: Assuming we have an Orthographic helper or using GLM directly
            m_ProjectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_Near, m_Far);
        }
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void PerspectiveCamera::RecalculateViewMatrix() {
        BSA::Math::Matrix4 transform = BSA::Math::Matrix4::Translate(BSA::Math::Matrix4(1.0f), m_Position) *
                                       BSA::Math::Matrix4::Rotate(BSA::Math::Matrix4(1.0f), m_Rotation.x, { 1, 0, 0 }) *
                                       BSA::Math::Matrix4::Rotate(BSA::Math::Matrix4(1.0f), m_Rotation.y, { 0, 1, 0 }) *
                                       BSA::Math::Matrix4::Rotate(BSA::Math::Matrix4(1.0f), m_Rotation.z, { 0, 0, 1 });

        m_ViewMatrix = transform.Inverse();
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

} // namespace BSA
