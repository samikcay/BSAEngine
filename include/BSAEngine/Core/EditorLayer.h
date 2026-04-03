#pragma once

#include "BSAEngine/Core/Layer.h"
#include "BSAEngine/Renderer/Shader.h"
#include "BSAEngine/Renderer/VertexArray.h"
#include "BSAEngine/Renderer/Framebuffer.h"
#include "BSAEngine/Renderer/EditorCamera.h"
#include "BSAEngine/Renderer/StaticMesh.h"
#include "BSAEngine/Renderer/Material.h"
#include "BSAEngine/Scene/Scene.h"
#include "BSAEngine/Panels/SceneHierarchyPanel.h"
#include "BSAEngine/Panels/ContentBrowserPanel.h"
#include <imgui.h>

namespace BSA {

    enum class SceneState {
        Edit = 0,
        Play,
        Pause
    };

    class EditorLayer : public Layer {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnUpdate(float ts) override;
        virtual void OnImGuiRender() override;
        virtual void OnEvent(Event& e) override;

    private:
        void UI_Toolbar();
        void UI_StatusBar();

        void OnScenePlay();
        void OnScenePause();
        void OnSceneStop();

    private:
        std::shared_ptr<Shader> m_CubeShader;
        std::shared_ptr<StaticMesh> m_CubeMesh;
        std::shared_ptr<Material> m_CubeMaterial;
        std::shared_ptr<Framebuffer> m_Framebuffer;
        std::shared_ptr<Scene> m_ActiveScene;
        std::shared_ptr<Scene> m_EditorScene; // Snapshot of scene before play

        EditorCamera m_EditorCamera;

        SceneHierarchyPanel m_SceneHierarchyPanel;
        ContentBrowserPanel m_ContentBrowserPanel;

        ImVec2 m_ViewportSize = { 0, 0 };

        float m_Accumulator = 0.0f;
        SceneState m_SceneState = SceneState::Edit;
    };

} // namespace BSA
