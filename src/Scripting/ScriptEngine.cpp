#include "BSAEngine/Scripting/ScriptEngine.h"
#include "BSAEngine/Log/Log.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/attrdefs.h> // for MONO_TYPE_CLASS

#include "BSAEngine/Scene/Entity.h"
#include "BSAEngine/Scene/Components.h"
#include <GLFW/glfw3.h> // For Key mapping in Input

#include <fstream>
#include <filesystem>
#include <unordered_map>

namespace BSA {

    struct ScriptInstance {
        MonoObject* Instance = nullptr;
        uint32_t GCHandle = 0; // Garbage collection korumasi
        MonoMethod* OnCreateMethod = nullptr;
        MonoMethod* OnUpdateMethod = nullptr;
    };

    struct ScriptEngineData {
        MonoDomain* RootDomain = nullptr;
        MonoDomain* AppDomain = nullptr;
        MonoAssembly* CoreAssembly = nullptr;
        MonoImage* CoreAssemblyImage = nullptr;

        Scene* SceneContext = nullptr;
        std::unordered_map<uint64_t, ScriptInstance> EntityInstances;
    };

    static ScriptEngineData* s_Data = nullptr;

    // Helper file loading
    static char* ReadBytes(const std::string& filepath, uint32_t* outSize) {
        std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

        if (!stream) {
            // İkinci bir şans: Eğer bin klasöründeysek bir üst dizinlere bak
            std::string fallbackPath = "../../" + filepath;
            stream.open(fallbackPath, std::ios::binary | std::ios::ate);
            
            if (!stream) {
                fallbackPath = "../../../" + filepath;
                stream.open(fallbackPath, std::ios::binary | std::ios::ate);
            }

            if (!stream) {
                // BSA_ENGINE_ERROR("Dosya bulunamadı: {0}", filepath);
                return nullptr;
            }
        }

        std::streampos end = stream.tellg();
        stream.seekg(0, std::ios::beg);
        uint32_t size = (uint32_t)(end - stream.tellg());

        if (size == 0) {
            BSA_ENGINE_ERROR("Dosya bos: {0}", filepath);
            return nullptr;
        }

        char* buffer = new char[size];
        stream.read((char*)buffer, size);
        stream.close();

        *outSize = size;
        return buffer;
    }

    static MonoAssembly* LoadMonoAssembly(const std::string& assemblyPath) {
        uint32_t fileSize = 0;
        char* fileData = ReadBytes(assemblyPath, &fileSize);

        // NOTE: This assumes mono is already initialized
        MonoImageOpenStatus status;
        MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

        if (status != MONO_IMAGE_OK) {
            const char* errorMessage = mono_image_strerror(status);
            BSA_ENGINE_ERROR("Mono image hatalı: {0}", errorMessage);
            return nullptr;
        }

        MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
        mono_image_close(image);

        // Don't forget to free the file data
        delete[] fileData;

        return assembly;
    }

    // ===================================
    // Internal Callbacks (C# -> C++)
    // ===================================

    static void NativeLog(MonoString* string, int parameter) {
        char* cStr = mono_string_to_utf8(string);
        std::string str(cStr);
        mono_free(cStr);
        BSA_ENGINE_INFO("C#: {0}, Param: {1}", str, parameter);
    }

    static bool Input_IsKeyDown(int keycode) {
        if (!s_Data || !s_Data->SceneContext) return false;
        GLFWwindow* window = glfwGetCurrentContext();
        int state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    static void TransformComponent_GetTranslation(uint64_t entityID, BSA::Math::Vector3* outTranslation) {
        if (!s_Data->SceneContext) return;
        Entity entity{ (entt::entity)entityID, s_Data->SceneContext };
        *outTranslation = entity.GetComponent<TransformComponent>().Translation;
    }

    static void TransformComponent_SetTranslation(uint64_t entityID, BSA::Math::Vector3* translation) {
        if (!s_Data->SceneContext) return;
        Entity entity{ (entt::entity)entityID, s_Data->SceneContext };
        entity.GetComponent<TransformComponent>().Translation = *translation;
    }

    // ===================================
    // ScriptEngine Implementation
    // ===================================

    void ScriptEngine::SetSceneContext(Scene* scene) {
        s_Data->SceneContext = scene;
    }

    void ScriptEngine::Init() {
        s_Data = new ScriptEngineData();

        InitMono();

        mono_add_internal_call("BSAEngine.InternalCalls::NativeLog", (void*)NativeLog);
        mono_add_internal_call("BSAEngine.InternalCalls::Input_IsKeyDown", (void*)Input_IsKeyDown);
        mono_add_internal_call("BSAEngine.InternalCalls::TransformComponent_GetTranslation", (void*)TransformComponent_GetTranslation);
        mono_add_internal_call("BSAEngine.InternalCalls::TransformComponent_SetTranslation", (void*)TransformComponent_SetTranslation);
    }

    void ScriptEngine::Shutdown() {
        ShutdownMono();

        delete s_Data;
        s_Data = nullptr;
    }

    void ScriptEngine::InitMono() {
        // Mono'nun mscorlib.dll gibi temel kütüphanelerini bulabilmesi için dizin set edilmeli
        std::string monoLibPath = "vendor/mono/lib";
        
        if (!std::filesystem::exists(monoLibPath)) {
            // Eğer exe bin/Debug/Debug içindeysek root'a gitmeye çalış
            monoLibPath = "../../../vendor/mono/lib";
            if (!std::filesystem::exists(monoLibPath)) {
                // Son ihtimal bir üst dizin
                monoLibPath = "../../vendor/mono/lib";
            }
        }

        mono_set_assemblies_path(monoLibPath.c_str());

        // Root Domain (JIT Initialization)
        MonoDomain* rootDomain = mono_jit_init("BSAEngineJITRuntime");
        if (!rootDomain) {
            BSA_ENGINE_ERROR("Mono JIT başlatılamadı! (Assemblies Path: {0})", monoLibPath);
            return;
        }
        s_Data->RootDomain = rootDomain;

        // App Domain (Custom Environment for scripts so they can be reloaded without closing engine)
        char appDomainName[] = "BSAEngineScriptRuntime";
        s_Data->AppDomain = mono_domain_create_appdomain(appDomainName, nullptr);
        mono_domain_set(s_Data->AppDomain, true);
    }

    void ScriptEngine::ShutdownMono() {
        s_Data->AppDomain = nullptr;
        s_Data->RootDomain = nullptr;
        // mono_jit_cleanup doesn't work well multiple times usually, but for simple exit we can just drop the domain
    }

    void ScriptEngine::LoadAssembly(const std::string& filepath) {
        s_Data->CoreAssembly = LoadMonoAssembly(filepath);
        if (!s_Data->CoreAssembly) {
            // Eğer root'tan verildiyse (bin/Debug/Debug/...) ve EXE o klasördeyse direkt ismini dene
            std::filesystem::path p(filepath);
            std::string filename = p.filename().string();
            if (filename != filepath) {
                BSA_ENGINE_WARN("Assembly {0} bulunamadı, {1} deneniyor...", filepath, filename);
                s_Data->CoreAssembly = LoadMonoAssembly(filename);
            }
        }

        if (s_Data->CoreAssembly) {
            s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
            BSA_ENGINE_INFO("C# Assembly Yuklendi.");
        } else {
            BSA_ENGINE_ERROR("C# Assembly Yuklenemedi: {0}", filepath);
        }
    }

    void ScriptEngine::PrintAssemblyTypes() {
        if (!s_Data->CoreAssemblyImage) return;

        MonoImage* image = s_Data->CoreAssemblyImage;
        const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

        for (int32_t i = 0; i < numTypes; i++) {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

            BSA_ENGINE_TRACE("C# Modul Bulundu -> {0}.{1}", nameSpace, name);
        }
    }

    void ScriptEngine::OnCreateEntity(Entity entity) {
        if (!s_Data || !s_Data->CoreAssemblyImage) return;
        if (!entity.HasComponent<ScriptComponent>()) return;

        auto& sc = entity.GetComponent<ScriptComponent>();
        if (sc.ClassName.empty()) return;

        std::string nameSpace, className;
        size_t dotPos = sc.ClassName.find_last_of('.');
        if (dotPos != std::string::npos) {
            nameSpace = sc.ClassName.substr(0, dotPos);
            className = sc.ClassName.substr(dotPos + 1);
        } else {
            className = sc.ClassName;
        }

        MonoClass* monoClass = mono_class_from_name(s_Data->CoreAssemblyImage, nameSpace.c_str(), className.c_str());
        if (!monoClass) {
            BSA_ENGINE_ERROR("C# Sınıfı bulunamadı: {0}", sc.ClassName);
            return;
        }

        // C# Entity nesnesini oluştur
        MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
        if (!instance) return;

        // Parametresiz yapıcı bloğunu çağır
        mono_runtime_object_init(instance);

        // Referans tutmak için ScriptInstance dizisini güncelle (Garbage Collector'dan sakla)
        uint64_t entityID = (uint64_t)(uint32_t)entity;
        ScriptInstance scriptInstance;
        scriptInstance.Instance = instance;
        scriptInstance.GCHandle = mono_gchandle_new(instance, false); // weak ref degil, strong ref

        // C# tarfındaki özel Entity yapıcı metodunu (ID atayan fonksiyon) çağır: 'internal Entity(ulong id)'
        // Ya da reflection ile C# base sınıfının Private ID Property'sine atama yap
        // Not: Direkt olarak field atamak daha temiz:
        MonoClass* entityBaseClass = mono_class_from_name(s_Data->CoreAssemblyImage, "BSAEngine", "Entity");
        MonoProperty* idProperty = mono_class_get_property_from_name(entityBaseClass, "ID");
        if (idProperty) {
            void* propertyValue = &entityID;
            mono_property_set_value(idProperty, instance, &propertyValue, nullptr);
        }

        // Metodlari onceden onbellege alalim
        scriptInstance.OnCreateMethod = mono_class_get_method_from_name(monoClass, "OnCreate", 0);
        scriptInstance.OnUpdateMethod = mono_class_get_method_from_name(monoClass, "OnUpdate", 1);

        s_Data->EntityInstances[entityID] = scriptInstance;

        // OnCreate'i C# tarafinda tetikle
        if (scriptInstance.OnCreateMethod) {
            mono_runtime_invoke(scriptInstance.OnCreateMethod, scriptInstance.Instance, nullptr, nullptr);
        }
    }

    void ScriptEngine::OnUpdateEntity(Entity entity, float ts) {
        if (!s_Data || !s_Data->CoreAssemblyImage) return;
        if (!entity.HasComponent<ScriptComponent>()) return;

        uint64_t entityID = (uint64_t)(uint32_t)entity;
        if (s_Data->EntityInstances.find(entityID) == s_Data->EntityInstances.end()) return;

        ScriptInstance& scriptInstance = s_Data->EntityInstances[entityID];
        if (scriptInstance.OnUpdateMethod) {
            void* params[1] = { &ts };
            // MonoBehaviour.Update() gibi dusun
            mono_runtime_invoke(scriptInstance.OnUpdateMethod, scriptInstance.Instance, params, nullptr);
        }
    }

}
