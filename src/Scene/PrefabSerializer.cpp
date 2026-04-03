#include "BSAEngine/Scene/PrefabSerializer.h"
#include "BSAEngine/Scene/SceneSerializer.h"
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace BSA {

    void PrefabSerializer::Serialize(const std::string& filepath, Entity entity) {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Prefab" << YAML::Value << "Untitled";
        out << YAML::Key << "Entity" << YAML::Value;
        
        SceneSerializer::SerializeEntity(out, entity);
        
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    Entity PrefabSerializer::Deserialize(const std::string& filepath, Scene* scene) {
        std::ifstream stream(filepath);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
        if (!data["Prefab"]) return {};

        YAML::Node entityNode = data["Entity"];
        if (entityNode) {
            SceneSerializer serializer(std::shared_ptr<Scene>(scene, [](Scene*){})); // Dummy deleter
            return serializer.DeserializeEntity(entityNode);
        }

        return {};
    }

} // namespace BSA
