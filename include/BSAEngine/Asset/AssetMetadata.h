#pragma once

#include "BSAEngine/Asset/Asset.h"
#include <filesystem>

namespace BSA {

    struct AssetMetadata {
        AssetHandle Handle;
        AssetType Type;
        std::filesystem::path FilePath;

        operator bool() const { return Type != AssetType::None; }
    };

} // namespace BSA
