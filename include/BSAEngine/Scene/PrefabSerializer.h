#pragma once

#include "BSAEngine/Scene/Entity.h"
#include <string>

namespace BSA {

    class PrefabSerializer {
    public:
        static void Serialize(const std::string& filepath, Entity entity);
        static Entity Deserialize(const std::string& filepath, Scene* scene);
    };

} // namespace BSA
