#pragma once

#include "BSAEngine/Renderer/RenderCommand.h"
#include "BSAEngine/Renderer/PerspectiveCamera.h"
#include "BSAEngine/Renderer/Shader.h"
#include "BSAEngine/Renderer/UniformBuffer.h"
#include <memory>
#include <vector>

namespace BSA {

    class StaticMesh;

    class Renderer {
    public:
        static void Init();
        static void Shutdown();
        static void OnWindowResize(uint32_t width, uint32_t height);

        static void BeginScene(PerspectiveCamera& camera);
        static void BeginScene(const glm::mat4& viewProjection, const glm::vec3& cameraPosition);
        static void BeginScene(const glm::mat4& viewProjection); // Legacy/Shortcut
        static void EndScene();

        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f), int entityID = -1);

        // 3D Instancing
        static void DrawInstancedMesh(const std::shared_ptr<StaticMesh>& mesh, const std::shared_ptr<Shader>& shader, const std::vector<glm::mat4>& transforms);

        // Lighting Public Structures
        struct DirectionalLight {
            glm::vec3 Direction;
            glm::vec3 Color;
            float Intensity;
        };

        struct PointLight {
            glm::vec3 Position;
            glm::vec3 Color;
            float Intensity;
            float Constant;
            float Linear;
            float Quadratic;
        };

        struct SpotLight {
            glm::vec3 Position;
            glm::vec3 Direction;
            glm::vec3 Color;
            float Intensity;
            float CutOff;
            float OuterCutOff;
            float Constant;
            float Linear;
            float Quadratic;
        };

        static void SubmitDirectionalLight(const DirectionalLight& light);
        static void SubmitPointLight(const PointLight& light);
        static void SubmitSpotLight(const SpotLight& light);

        static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

    private:
        struct DrawCommand {
            std::shared_ptr<Shader> ShaderPtr;
            std::shared_ptr<VertexArray> VertexArrayPtr;
            glm::mat4 Transform;
            int EntityID;
        };

        // UBO Layout Structures (std140)
        struct CameraData {
            glm::mat4 ViewProjection;
            glm::vec3 ViewPosition;
            float padding;
        };

        struct PointLightData {
            glm::vec3 Position;
            float padding1;
            glm::vec3 Color;
            float Intensity;
            float Constant;
            float Linear;
            float Quadratic;
            float padding2;
        };

        struct SpotLightData {
            glm::vec3 Position;
            float padding1;
            glm::vec3 Direction;
            float padding2;
            glm::vec3 Color;
            float Intensity;
            float CutOff;
            float OuterCutOff;
            float Constant;
            float Linear;
            float Quadratic;
            float padding3[3];
        };

        struct LightingData {
            struct {
                glm::vec3 Direction;
                float padding1;
                glm::vec3 Color;
                float Intensity;
            } DirLight;

            PointLightData PointLights[8];
            int PointLightCount;
            float padding[3];

            SpotLightData SpotLights[8];
            int SpotLightCount;
            float padding2[3];
        };

        struct SceneData {
            glm::mat4 ViewProjectionMatrix;
            glm::vec3 ViewPosition;
            
            DirectionalLight DirLight;
            std::vector<PointLight> PointLights;
            std::vector<SpotLight> SpotLights;

            std::vector<DrawCommand> DrawQueue;

            std::shared_ptr<UniformBuffer> CameraUniformBuffer;
            std::shared_ptr<UniformBuffer> LightingUniformBuffer;
        };

        static std::unique_ptr<SceneData> s_SceneData;
    };

} // namespace BSA
