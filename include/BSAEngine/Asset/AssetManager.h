#pragma once

#include "BSAEngine/Asset/Asset.h"
#include "BSAEngine/Asset/AssetMetadata.h"
#include <map>
#include <memory>
#include <string>

namespace BSA {

    class AssetManager {
    public:
        static void Init();
        static void Shutdown();

        template<typename T>
        static std::shared_ptr<T> GetAsset(AssetHandle handle) {
            std::shared_ptr<Asset> asset = GetAssetInternal(handle);
            return std::static_pointer_cast<T>(asset);
        }

        static AssetHandle ImportAsset(const std::filesystem::path& filepath);
        static bool IsAssetHandleValid(AssetHandle handle);
        static bool IsAssetLoaded(AssetHandle handle);
        
        static const AssetMetadata& GetAssetMetadata(AssetHandle handle);

    private:
        static std::shared_ptr<Asset> GetAssetInternal(AssetHandle handle);
        static std::shared_ptr<Asset> LoadAsset(AssetHandle handle);

    private:
        static std::map<AssetHandle, AssetMetadata> s_AssetRegistry;
        static std::map<AssetHandle, std::shared_ptr<Asset>> s_LoadedAssets;
    };

} // namespace BSA
