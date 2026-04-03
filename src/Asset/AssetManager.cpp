#include "BSAEngine/Asset/AssetManager.h"
#include "BSAEngine/Renderer/Texture.h"
#include "BSAEngine/Renderer/Shader.h"
#include "BSAEngine/Renderer/StaticMesh.h"
#include "BSAEngine/Log/Log.h"
#include <filesystem>

namespace BSA {

    std::map<AssetHandle, AssetMetadata> AssetManager::s_AssetRegistry;
    std::map<AssetHandle, std::shared_ptr<Asset>> AssetManager::s_LoadedAssets;

    void AssetManager::Init() {
        BSA_ENGINE_INFO("AssetManager Initialized");
    }

    void AssetManager::Shutdown() {
        s_LoadedAssets.clear();
        s_AssetRegistry.clear();
        BSA_ENGINE_INFO("AssetManager Shutdown");
    }

    AssetHandle AssetManager::ImportAsset(const std::filesystem::path& filepath) {
        if (!std::filesystem::exists(filepath)) {
            BSA_ENGINE_ERROR("Asset path does not exist: {0}", filepath.string());
            return 0;
        }

        // Check if already imported
        for (auto& [handle, metadata] : s_AssetRegistry) {
            if (metadata.FilePath == filepath)
                return handle;
        }

        AssetMetadata metadata;
        metadata.Handle = AssetHandle();
        metadata.FilePath = filepath;
        
        std::string extension = filepath.extension().string();
        if (extension == ".png" || extension == ".jpg" || extension == ".jpeg")
            metadata.Type = AssetType::Texture2D;
        else if (extension == ".glsl" || extension == ".shader")
            metadata.Type = AssetType::Shader;
        else if (extension == ".obj" || extension == ".fbx")
            metadata.Type = AssetType::StaticMesh;
        else if (extension == ".bsascene")
            metadata.Type = AssetType::Scene;
        else
            metadata.Type = AssetType::None;

        s_AssetRegistry[metadata.Handle] = metadata;
        return metadata.Handle;
    }

    bool AssetManager::IsAssetHandleValid(AssetHandle handle) {
        return s_AssetRegistry.find(handle) != s_AssetRegistry.end();
    }

    bool AssetManager::IsAssetLoaded(AssetHandle handle) {
        return s_LoadedAssets.find(handle) != s_LoadedAssets.end();
    }

    const AssetMetadata& AssetManager::GetAssetMetadata(AssetHandle handle) {
        if (s_AssetRegistry.find(handle) != s_AssetRegistry.end())
            return s_AssetRegistry[handle];
        
        static AssetMetadata s_EmptyMetadata;
        return s_EmptyMetadata;
    }

    std::shared_ptr<Asset> AssetManager::GetAssetInternal(AssetHandle handle) {
        if (!IsAssetHandleValid(handle))
            return nullptr;

        if (IsAssetLoaded(handle))
            return s_LoadedAssets[handle];

        return LoadAsset(handle);
    }

    std::shared_ptr<Asset> AssetManager::LoadAsset(AssetHandle handle) {
        const AssetMetadata& metadata = GetAssetMetadata(handle);
        std::shared_ptr<Asset> asset = nullptr;

        switch (metadata.Type) {
            case AssetType::Texture2D:
                asset = std::shared_ptr<Asset>(Texture2D::Create(metadata.FilePath.string()));
                break;
            case AssetType::Shader:
                asset = std::static_pointer_cast<Asset>(Shader::Create(metadata.FilePath.string()));
                break;
            case AssetType::StaticMesh:
                asset = std::static_pointer_cast<Asset>(StaticMesh::Create(metadata.FilePath.string()));
                break;
        }

        if (asset) {
            asset->Handle = handle;
            s_LoadedAssets[handle] = asset;
        }

        return asset;
    }

} // namespace BSA
