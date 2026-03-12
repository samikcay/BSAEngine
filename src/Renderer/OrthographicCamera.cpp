#include "BSAEngine/Renderer/OrthographicCamera.h"

namespace BSA {

    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
        : m_ProjectionMatrix(BSA::Math::Matrix4::Orthographic(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
    {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::SetProjection(float left, float right, float bottom, float top) {
        m_ProjectionMatrix = BSA::Math::Matrix4::Orthographic(left, right, bottom, top, -1.0f, 1.0f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::RecalculateViewMatrix() {
        BSA::Math::Matrix4 transform = BSA::Math::Matrix4::Translate(BSA::Math::Matrix4(1.0f), m_Position) *
                                       BSA::Math::Matrix4::Rotate(BSA::Math::Matrix4(1.0f), BSA::Math::Radians(m_Rotation), BSA::Math::Vector3(0, 0, 1));

        m_ViewMatrix = transform.Inverse();
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

} // namespace BSA
