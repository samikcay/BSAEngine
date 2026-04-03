#include "BSAEngine/Renderer/Renderer.h"
#include "BSAEngine/Renderer/Renderer2D.h"
#include "BSAEngine/Renderer/StaticMesh.h"

namespace BSA {

    std::unique_ptr<Renderer::SceneData> Renderer::s_SceneData = std::make_unique<Renderer::SceneData>();

    void Renderer::Init() {
        RenderCommand::Init();
        Renderer2D::Init();

        s_SceneData->CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0);
        s_SceneData->LightingUniformBuffer = UniformBuffer::Create(sizeof(LightingData), 1);
    }

    void Renderer::Shutdown() {
        Renderer2D::Shutdown();
        s_SceneData.reset();
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
        RenderCommand::SetViewport(0, 0, width, height);
    }

    void Renderer::BeginScene(PerspectiveCamera& camera) {
        s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
        s_SceneData->ViewPosition = camera.GetPosition();
        
        s_SceneData->PointLights.clear();
        s_SceneData->SpotLights.clear();
        s_SceneData->DrawQueue.clear();
    }

    void Renderer::BeginScene(const glm::mat4& viewProjection, const glm::vec3& cameraPosition) {
        s_SceneData->ViewProjectionMatrix = viewProjection;
        s_SceneData->ViewPosition = cameraPosition;

        s_SceneData->PointLights.clear();
        s_SceneData->SpotLights.clear();
        s_SceneData->DrawQueue.clear();
    }

    void Renderer::BeginScene(const glm::mat4& viewProjection) {
        BeginScene(viewProjection, glm::vec3(0.0f));
    }

    void Renderer::EndScene() {
        // 1. Update Camera UBO
        CameraData cameraData;
        cameraData.ViewProjection = s_SceneData->ViewProjectionMatrix;
        cameraData.ViewPosition = s_SceneData->ViewPosition;
        s_SceneData->CameraUniformBuffer->SetData(&cameraData, sizeof(CameraData));

        // 2. Update Lighting UBO
        LightingData lightingData{};  // Zero-initialize to avoid garbage in padding
        lightingData.DirLight.Direction = s_SceneData->DirLight.Direction;
        lightingData.DirLight.Color = s_SceneData->DirLight.Color;
        lightingData.DirLight.Intensity = s_SceneData->DirLight.Intensity;

        lightingData.PointLightCount = (int)s_SceneData->PointLights.size();
        for (size_t i = 0; i < s_SceneData->PointLights.size(); i++) {
            lightingData.PointLights[i].Position = s_SceneData->PointLights[i].Position;
            lightingData.PointLights[i].Color = s_SceneData->PointLights[i].Color;
            lightingData.PointLights[i].Intensity = s_SceneData->PointLights[i].Intensity;
            lightingData.PointLights[i].Constant = s_SceneData->PointLights[i].Constant;
            lightingData.PointLights[i].Linear = s_SceneData->PointLights[i].Linear;
            lightingData.PointLights[i].Quadratic = s_SceneData->PointLights[i].Quadratic;
        }

        lightingData.SpotLightCount = (int)s_SceneData->SpotLights.size();
        for (size_t i = 0; i < s_SceneData->SpotLights.size() && i < 8; i++) {
            lightingData.SpotLights[i].Position = s_SceneData->SpotLights[i].Position;
            lightingData.SpotLights[i].Direction = s_SceneData->SpotLights[i].Direction;
            lightingData.SpotLights[i].Color = s_SceneData->SpotLights[i].Color;
            lightingData.SpotLights[i].Intensity = s_SceneData->SpotLights[i].Intensity;
            lightingData.SpotLights[i].CutOff = s_SceneData->SpotLights[i].CutOff;
            lightingData.SpotLights[i].OuterCutOff = s_SceneData->SpotLights[i].OuterCutOff;
            lightingData.SpotLights[i].Constant = s_SceneData->SpotLights[i].Constant;
            lightingData.SpotLights[i].Linear = s_SceneData->SpotLights[i].Linear;
            lightingData.SpotLights[i].Quadratic = s_SceneData->SpotLights[i].Quadratic;
        }
        s_SceneData->LightingUniformBuffer->SetData(&lightingData, sizeof(LightingData));

        // 3. Execute Draw Queue
        for (auto& command : s_SceneData->DrawQueue) {
            command.ShaderPtr->Bind();
            command.ShaderPtr->UploadUniformMat4("u_Transform", command.Transform);
            command.ShaderPtr->UploadUniformInt("u_EntityID", command.EntityID);

            command.VertexArrayPtr->Bind();
            RenderCommand::DrawIndexed(command.VertexArrayPtr);
        }
    }

    void Renderer::SubmitDirectionalLight(const DirectionalLight& light) {
        s_SceneData->DirLight = light;
    }

    void Renderer::SubmitPointLight(const PointLight& light) {
        if (s_SceneData->PointLights.size() < 8)
            s_SceneData->PointLights.push_back(light);
    }

    void Renderer::SubmitSpotLight(const SpotLight& light) {
        if (s_SceneData->SpotLights.size() < 8)
            s_SceneData->SpotLights.push_back(light);
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, int entityID) {
        s_SceneData->DrawQueue.push_back({ shader, vertexArray, transform, entityID });
    }

    void Renderer::DrawInstancedMesh(const std::shared_ptr<StaticMesh>& mesh, const std::shared_ptr<Shader>& shader, const std::vector<glm::mat4>& transforms) {
        // For instancing, we could also queue it, but for now we keep it immediate or separate
        shader->Bind();
        mesh->Bind();
        for (const auto& transform : transforms) {
            shader->UploadUniformMat4("u_Transform", transform);
            RenderCommand::DrawIndexed(mesh->GetVertexArray());
        }
    }

} // namespace BSA
