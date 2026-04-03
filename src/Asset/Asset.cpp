#include "BSAEngine/Asset/Asset.h"
#include <map>

namespace BSA {

    static std::map<std::string, AssetType> s_AssetTypeMap = {
        { "None", AssetType::None },
        { "Texture2D", AssetType::Texture2D },
        { "Shader", AssetType::Shader },
        { "StaticMesh", AssetType::StaticMesh },
        { "Scene", AssetType::Scene }
    };

    std::string Asset::AssetTypeToString(AssetType type) {
        for (auto& [name, assetType] : s_AssetTypeMap) {
            if (assetType == type)
                return name;
        }
        return "None";
    }

    AssetType Asset::AssetTypeFromString(const std::string& type) {
        if (s_AssetTypeMap.find(type) != s_AssetTypeMap.end())
            return s_AssetTypeMap[type];
        return AssetType::None;
    }

} // namespace BSA
