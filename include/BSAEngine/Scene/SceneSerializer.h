#pragma once

#include "BSAEngine/Scene/Scene.h"
#include <string>

namespace YAML {
    class Emitter;
    class Node;
}

namespace BSA {

    class SceneSerializer {
    public:
        SceneSerializer(const std::shared_ptr<Scene>& scene);

        void Serialize(const std::string& filepath);
        void SerializeRuntime(const std::string& filepath);

        bool Deserialize(const std::string& filepath);
        bool DeserializeRuntime(const std::string& filepath);

        // Prefab Support
        static void SerializeEntity(YAML::Emitter& out, Entity entity);
        Entity DeserializeEntity(YAML::Node& entityNode);

    private:
        std::shared_ptr<Scene> m_Scene;
    };

} // namespace BSA
