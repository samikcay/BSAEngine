#include "BSAEngine/Renderer/PerspectiveCamera.h"

namespace BSA {

    PerspectiveCamera::PerspectiveCamera(float fovYRad, float aspect, float zNear, float zFar)
        : m_ProjectionMatrix(BSA::Math::Matrix4::Perspective(fovYRad, aspect, zNear, zFar)), m_ViewMatrix(1.0f)
    {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void PerspectiveCamera::SetProjection(float fovYRad, float aspect, float zNear, float zFar) {
        m_ProjectionMatrix = BSA::Math::Matrix4::Perspective(fovYRad, aspect, zNear, zFar);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void PerspectiveCamera::RecalculateViewMatrix() {
        BSA::Math::Matrix4 transform = BSA::Math::Matrix4::Translate(BSA::Math::Matrix4(1.0f), m_Position) *
                                       BSA::Math::Matrix4::Rotate(BSA::Math::Matrix4(1.0f), BSA::Math::Radians(m_Rotation.y), BSA::Math::Vector3(0, 1, 0)) *
                                       BSA::Math::Matrix4::Rotate(BSA::Math::Matrix4(1.0f), BSA::Math::Radians(m_Rotation.x), BSA::Math::Vector3(1, 0, 0)) *
                                       BSA::Math::Matrix4::Rotate(BSA::Math::Matrix4(1.0f), BSA::Math::Radians(m_Rotation.z), BSA::Math::Vector3(0, 0, 1));

        m_ViewMatrix = transform.Inverse();
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

}
