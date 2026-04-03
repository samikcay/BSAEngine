#pragma once

#include "BSAEngine/Asset/AssetHandle.h"
#include <string>
#include <cstdint>

namespace BSA {

    enum class AssetType : uint16_t {
        None = 0,
        Texture2D,
        Shader,
        StaticMesh,
        Scene
    };

    class Asset {
    public:
        AssetHandle Handle;

        virtual ~Asset() = default;

        virtual AssetType GetType() const = 0;

        static std::string AssetTypeToString(AssetType type);
        static AssetType AssetTypeFromString(const std::string& type);
    };

} // namespace BSA
