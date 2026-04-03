#include "BSAEngine/Scene/SceneSerializer.h"
#include "BSAEngine/Scene/Entity.h"
#include "BSAEngine/Scene/Components.h"
#include "BSAEngine/Asset/AssetManager.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>

namespace YAML {

    template<>
    struct convert<glm::vec3> {
        static Node encode(const glm::vec3& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs) {
            if (!node.IsSequence() || node.size() != 3) return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4> {
        static Node encode(const glm::vec4& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs) {
            if (!node.IsSequence() || node.size() != 4) return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

}

namespace BSA {

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene)
        : m_Scene(scene)
    {
    }

    void SceneSerializer::SerializeEntity(YAML::Emitter& out, Entity entity) {
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity" << YAML::Value << (uint64_t)entity.GetComponent<IDComponent>().ID;

        if (entity.HasComponent<TagComponent>()) {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap; // TagComponent
            auto& tag = entity.GetComponent<TagComponent>().Tag;
            out << YAML::Key << "Tag" << YAML::Value << tag;
            out << YAML::EndMap; // TagComponent
        }

        if (entity.HasComponent<TransformComponent>()) {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent
            auto& tc = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
            out << YAML::EndMap; // TransformComponent
        }

        if (entity.HasComponent<CameraComponent>()) {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap; // CameraComponent
            auto& cameraComponent = entity.GetComponent<CameraComponent>();
            out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
            out << YAML::Key << "FOV" << YAML::Value << cameraComponent.FOV;
            out << YAML::Key << "Near" << YAML::Value << cameraComponent.Near;
            out << YAML::Key << "Far" << YAML::Value << cameraComponent.Far;
            out << YAML::EndMap; // CameraComponent
        }

        if (entity.HasComponent<MeshRendererComponent>()) {
            out << YAML::Key << "MeshRendererComponent";
            out << YAML::BeginMap; // MeshRendererComponent
            auto& mrc = entity.GetComponent<MeshRendererComponent>();
            if (mrc.Mesh)
                out << YAML::Key << "MeshHandle" << YAML::Value << (uint64_t)mrc.Mesh->Handle;
            if (mrc.Mat)
                out << YAML::Key << "MaterialName" << YAML::Value << mrc.Mat->GetName();
            out << YAML::EndMap; // MeshRendererComponent
        }

        if (entity.HasComponent<DirectionalLightComponent>()) {
            out << YAML::Key << "DirectionalLightComponent";
            out << YAML::BeginMap;
            auto& light = entity.GetComponent<DirectionalLightComponent>();
            out << YAML::Key << "Color" << YAML::Value << light.Color;
            out << YAML::Key << "Intensity" << YAML::Value << light.Intensity;
            out << YAML::EndMap;
        }

        if (entity.HasComponent<PointLightComponent>()) {
            out << YAML::Key << "PointLightComponent";
            out << YAML::BeginMap;
            auto& light = entity.GetComponent<PointLightComponent>();
            out << YAML::Key << "Color" << YAML::Value << light.Color;
            out << YAML::Key << "Intensity" << YAML::Value << light.Intensity;
            out << YAML::Key << "Constant" << YAML::Value << light.Constant;
            out << YAML::Key << "Linear" << YAML::Value << light.Linear;
            out << YAML::Key << "Quadratic" << YAML::Value << light.Quadratic;
            out << YAML::EndMap;
        }

        if (entity.HasComponent<SpotLightComponent>()) {
            out << YAML::Key << "SpotLightComponent";
            out << YAML::BeginMap;
            auto& light = entity.GetComponent<SpotLightComponent>();
            out << YAML::Key << "Color" << YAML::Value << light.Color;
            out << YAML::Key << "Intensity" << YAML::Value << light.Intensity;
            out << YAML::Key << "CutOff" << YAML::Value << light.CutOff;
            out << YAML::Key << "OuterCutOff" << YAML::Value << light.OuterCutOff;
            out << YAML::Key << "Constant" << YAML::Value << light.Constant;
            out << YAML::Key << "Linear" << YAML::Value << light.Linear;
            out << YAML::Key << "Quadratic" << YAML::Value << light.Quadratic;
            out << YAML::EndMap;
        }

        if (entity.HasComponent<Rigidbody2DComponent>()) {
            out << YAML::Key << "Rigidbody2DComponent";
            out << YAML::BeginMap;
            auto& rb = entity.GetComponent<Rigidbody2DComponent>();
            out << YAML::Key << "BodyType" << YAML::Value << (int)rb.Type;
            out << YAML::Key << "Mass" << YAML::Value << rb.Mass;
            out << YAML::Key << "GravityScale" << YAML::Value << rb.GravityScale;
            out << YAML::Key << "FixedRotation" << YAML::Value << rb.FixedRotation;
            out << YAML::Key << "LinearVelocity" << YAML::Value << glm::vec3(rb.LinearVelocity.x, rb.LinearVelocity.y, rb.LinearVelocity.z);
            out << YAML::Key << "AngularVelocity" << YAML::Value << glm::vec3(rb.AngularVelocity.x, rb.AngularVelocity.y, rb.AngularVelocity.z);
            out << YAML::EndMap;
        }

        if (entity.HasComponent<BoxCollider2DComponent>()) {
            out << YAML::Key << "BoxCollider2DComponent";
            out << YAML::BeginMap;
            auto& bc = entity.GetComponent<BoxCollider2DComponent>();
            out << YAML::Key << "HalfExtents" << YAML::Value << glm::vec3(bc.HalfExtents.x, bc.HalfExtents.y, bc.HalfExtents.z);
            out << YAML::Key << "Offset" << YAML::Value << glm::vec3(bc.Offset.x, bc.Offset.y, bc.Offset.z);
            out << YAML::Key << "Density" << YAML::Value << bc.Density;
            out << YAML::Key << "Friction" << YAML::Value << bc.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << bc.Restitution;
            out << YAML::EndMap;
        }

        if (entity.HasComponent<CircleCollider2DComponent>()) {
            out << YAML::Key << "CircleCollider2DComponent";
            out << YAML::BeginMap;
            auto& cc = entity.GetComponent<CircleCollider2DComponent>();
            out << YAML::Key << "Radius" << YAML::Value << cc.Radius;
            out << YAML::Key << "Offset" << YAML::Value << glm::vec3(cc.Offset.x, cc.Offset.y, cc.Offset.z);
            out << YAML::Key << "Density" << YAML::Value << cc.Density;
            out << YAML::Key << "Friction" << YAML::Value << cc.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << cc.Restitution;
            out << YAML::EndMap;
        }

        if (entity.HasComponent<ScriptComponent>()) {
            out << YAML::Key << "ScriptComponent";
            out << YAML::BeginMap;
            auto& sc = entity.GetComponent<ScriptComponent>();
            out << YAML::Key << "ClassName" << YAML::Value << sc.ClassName;
            out << YAML::EndMap;
        }

        if (entity.HasComponent<RelationshipComponent>()) {
            auto& rel = entity.GetComponent<RelationshipComponent>();
            if (rel.Parent != entt::null || !rel.Children.empty()) {
                out << YAML::Key << "RelationshipComponent";
                out << YAML::BeginMap;
                out << YAML::Key << "Parent" << YAML::Value << (uint64_t)rel.Parent;
                out << YAML::Key << "Children" << YAML::Value << YAML::BeginSeq;
                for (auto child : rel.Children)
                    out << (uint64_t)(uint32_t)child;
                out << YAML::EndSeq;
                out << YAML::EndMap;
            }
        }

        out << YAML::EndMap; // Entity
    }

    Entity SceneSerializer::DeserializeEntity(YAML::Node& entity) {
        uint64_t uuid = entity["Entity"].as<uint64_t>();

        std::string name;
        auto tagComponent = entity["TagComponent"];
        if (tagComponent)
            name = tagComponent["Tag"].as<std::string>();

        Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

        auto transformComponent = entity["TransformComponent"];
        if (transformComponent) {
            auto& tc = deserializedEntity.GetComponent<TransformComponent>();
            tc.Translation = transformComponent["Translation"].as<glm::vec3>();
            tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
            tc.Scale = transformComponent["Scale"].as<glm::vec3>();
        }

        auto cameraComponent = entity["CameraComponent"];
        if (cameraComponent) {
            auto& cc = deserializedEntity.AddComponent<CameraComponent>();
            cc.Primary = cameraComponent["Primary"].as<bool>();
            cc.FOV = cameraComponent["FOV"].as<float>();
            cc.Near = cameraComponent["Near"].as<float>();
            cc.Far = cameraComponent["Far"].as<float>();
        }

        auto meshRendererComponent = entity["MeshRendererComponent"];
        if (meshRendererComponent) {
            auto& mrc = deserializedEntity.AddComponent<MeshRendererComponent>();
            if (meshRendererComponent["MeshHandle"]) {
                uint64_t meshHandle = meshRendererComponent["MeshHandle"].as<uint64_t>();
                if (AssetManager::IsAssetHandleValid(meshHandle)) {
                    mrc.Mesh = AssetManager::GetAsset<StaticMesh>(meshHandle);
                }
            }
            // MaterialName is stored for reference; full material deserialization
            // requires a material asset system (not yet implemented)
        }

        auto directionalLightComponent = entity["DirectionalLightComponent"];
        if (directionalLightComponent) {
            auto& dlc = deserializedEntity.AddComponent<DirectionalLightComponent>();
            dlc.Color = directionalLightComponent["Color"].as<glm::vec3>();
            dlc.Intensity = directionalLightComponent["Intensity"].as<float>();
        }

        auto pointLightComponent = entity["PointLightComponent"];
        if (pointLightComponent) {
            auto& plc = deserializedEntity.AddComponent<PointLightComponent>();
            plc.Color = pointLightComponent["Color"].as<glm::vec3>();
            plc.Intensity = pointLightComponent["Intensity"].as<float>();
            plc.Constant = pointLightComponent["Constant"].as<float>();
            plc.Linear = pointLightComponent["Linear"].as<float>();
            plc.Quadratic = pointLightComponent["Quadratic"].as<float>();
        }

        auto spotLightComponent = entity["SpotLightComponent"];
        if (spotLightComponent) {
            auto& slc = deserializedEntity.AddComponent<SpotLightComponent>();
            slc.Color = spotLightComponent["Color"].as<glm::vec3>();
            slc.Intensity = spotLightComponent["Intensity"].as<float>();
            slc.CutOff = spotLightComponent["CutOff"].as<float>();
            slc.OuterCutOff = spotLightComponent["OuterCutOff"].as<float>();
            slc.Constant = spotLightComponent["Constant"].as<float>();
            slc.Linear = spotLightComponent["Linear"].as<float>();
            slc.Quadratic = spotLightComponent["Quadratic"].as<float>();
        }

        auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
        if (rigidbody2DComponent) {
            auto& rb = deserializedEntity.AddComponent<Rigidbody2DComponent>();
            rb.Type = (BodyType)rigidbody2DComponent["BodyType"].as<int>();
            rb.Mass = rigidbody2DComponent["Mass"].as<float>();
            rb.GravityScale = rigidbody2DComponent["GravityScale"].as<float>();
            rb.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
            auto lv = rigidbody2DComponent["LinearVelocity"].as<glm::vec3>();
            rb.LinearVelocity = { lv.x, lv.y, lv.z };
            auto av = rigidbody2DComponent["AngularVelocity"].as<glm::vec3>();
            rb.AngularVelocity = { av.x, av.y, av.z };
        }

        auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
        if (boxCollider2DComponent) {
            auto& bc = deserializedEntity.AddComponent<BoxCollider2DComponent>();
            auto he = boxCollider2DComponent["HalfExtents"].as<glm::vec3>();
            bc.HalfExtents = { he.x, he.y, he.z };
            auto off = boxCollider2DComponent["Offset"].as<glm::vec3>();
            bc.Offset = { off.x, off.y, off.z };
            bc.Density = boxCollider2DComponent["Density"].as<float>();
            bc.Friction = boxCollider2DComponent["Friction"].as<float>();
            bc.Restitution = boxCollider2DComponent["Restitution"].as<float>();
        }

        auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
        if (circleCollider2DComponent) {
            auto& cc = deserializedEntity.AddComponent<CircleCollider2DComponent>();
            cc.Radius = circleCollider2DComponent["Radius"].as<float>();
            auto off = circleCollider2DComponent["Offset"].as<glm::vec3>();
            cc.Offset = { off.x, off.y, off.z };
            cc.Density = circleCollider2DComponent["Density"].as<float>();
            cc.Friction = circleCollider2DComponent["Friction"].as<float>();
            cc.Restitution = circleCollider2DComponent["Restitution"].as<float>();
        }

        auto scriptComponent = entity["ScriptComponent"];
        if (scriptComponent) {
            auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
            sc.ClassName = scriptComponent["ClassName"].as<std::string>();
        }

        return deserializedEntity;
    }

    void SceneSerializer::Serialize(const std::string& filepath) {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        auto view = m_Scene->Reg().view<IDComponent>();
        for (auto entityID : view) {
            Entity entity = { entityID, m_Scene.get() };
            if (!entity) continue;
            SerializeEntity(out, entity);
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }


    void SceneSerializer::SerializeRuntime(const std::string& filepath) {
        // Runtime binary serialization - writes scene in a compact binary format
        // For now, delegate to YAML serialization with .bsascene extension
        // A full binary format can be implemented later for performance
        BSA_ENGINE_WARN("SerializeRuntime: Binary format not yet implemented, falling back to YAML serialization.");
        Serialize(filepath);
    }

    bool SceneSerializer::Deserialize(const std::string& filepath) {
        std::ifstream stream(filepath);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
        if (!data["Scene"]) return false;

        std::string sceneName = data["Scene"].as<std::string>();

        auto entities = data["Entities"];
        if (entities) {
            for (auto entity : entities) {
                DeserializeEntity(entity);
            }
        }

        return true;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string& filepath) {
        // Runtime binary deserialization - reads scene from compact binary format
        // For now, delegate to YAML deserialization
        BSA_ENGINE_WARN("DeserializeRuntime: Binary format not yet implemented, falling back to YAML deserialization.");
        return Deserialize(filepath);
    }

} // namespace BSA
