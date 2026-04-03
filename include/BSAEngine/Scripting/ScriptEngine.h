#pragma once

#include "BSAEngine/Scene/Entity.h"
#include <string>

namespace BSA {

    class ScriptEngine {
    public:
        static void Init();
        static void Shutdown();

        // C# Derlenmis DLL'ini motora yukler
        static void LoadAssembly(const std::string& filepath);
        static void ReloadAssembly();

        // Test amacli: DLL icindeki Butun sinif ve fonksiyonlari (Namespace:Class) yazdirir
        static void PrintAssemblyTypes();

        static void SetSceneContext(Scene* scene);

        // ECS tarafindan tetiklenecek C# yasam dongusu
        static void OnCreateEntity(Entity entity);
        static void OnUpdateEntity(Entity entity, float ts);

    private:
        static void InitMono();
        static void ShutdownMono();
    };

}
