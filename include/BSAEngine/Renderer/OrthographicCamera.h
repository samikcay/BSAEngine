#pragma once

#include "BSAEngine/Math/Matrix4.h"

namespace BSA {

    class OrthographicCamera {
    public:
        // left, right, bottom, top -> Kutu (Box) seklindeki gorus alaninin sinirlari
        OrthographicCamera(float left, float right, float bottom, float top);

        void SetProjection(float left, float right, float bottom, float top);

        const BSA::Math::Vector3& GetPosition() const { return m_Position; }
        void SetPosition(const BSA::Math::Vector3& position) { m_Position = position; RecalculateViewMatrix(); }

        float GetRotation() const { return m_Rotation; }
        // Rotation Z ekseni uzerinde 2D dondurmedir
        void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

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
        float m_Rotation = 0.0f; // Sadece Z ekseni
    };

} // namespace BSA
