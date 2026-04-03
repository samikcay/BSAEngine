# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Configure (from project root)
cmake -B build

# Build (Debug)
cmake --build build --config Debug

# Build (Release)
cmake --build build --config Release
```

The executable outputs to `bin/Debug/` or `bin/Release/`. A post-build step copies assets and `mono-2.0-sgen.dll` to the output directory automatically.

**Prerequisites:** CMake 3.20+, C++17 compiler (MSVC/GCC/Clang), Mono runtime. Dependencies (ImGui, spdlog, GLFW, GLM, EnTT, Box2D, Jolt, yaml-cpp) are fetched automatically via CMake FetchContent.

## Architecture

BSAEngine is a C++17 game engine with an integrated ImGui editor. The namespace is `BSA::`.

### Core Loop

`main.cpp` → `Application` (singleton via `Application::Get()`) → pushes `EditorLayer` → runs the main loop which updates layers, dispatches events (reverse order), and renders ImGui.

### Layer System

`Layer` is the base class. `LayerStack` manages ordering. `EditorLayer` is the primary editor layer owning the scene, viewport framebuffer, editor camera, and UI panels. `ImGuiLayer` handles ImGui frame lifecycle.

### ECS (EnTT-based)

- `Scene` owns the `entt::registry` and manages entity lifecycle
- `Entity` wraps `entt::entity` + `Scene*` for type-safe component access (`AddComponent<T>`, `GetComponent<T>`, `HasComponent<T>`)
- Components are plain structs in `include/BSAEngine/Scene/Components.h`: `TransformComponent`, `MeshRendererComponent`, `CameraComponent`, `Rigidbody2DComponent`, `BoxCollider2DComponent`, `CircleCollider2DComponent`, `DirectionalLightComponent`, `PointLightComponent`, `SpotLightComponent`, `ScriptComponent`, `NativeScriptComponent`, `RelationshipComponent`, `IDComponent`, `TagComponent`
- Scene hierarchy uses `RelationshipComponent` for parent/children; world transforms computed recursively via `Scene::GetWorldTransform()`

### Rendering Pipeline

OpenGL-based with platform abstraction in `src/Platform/OpenGL/`. Key flow:

1. `Renderer::BeginScene()` → sets view-projection UBO (slot 0) and camera position
2. `Renderer::Submit()` → queues draw commands (shader + VAO + transform)
3. `Renderer::SubmitLight()` → queues lights into UBO (slot 1, max 8 point lights)
4. `Renderer::EndScene()` → executes all queued draws

`Renderer2D` provides batched quad rendering. `Framebuffer` is used for the editor viewport.

**Graphics abstractions:** `Shader`, `Texture`, `Buffer` (VBO/IBO), `VertexArray`, `UniformBuffer`, `Framebuffer` — all have OpenGL implementations in `src/Platform/OpenGL/`.

**Material** wraps a Shader + uniform value maps (floats, vectors, matrices, textures).

### Asset System

`AssetManager` (singleton) manages assets by UUID handles. Types: `Texture2D`, `Shader`, `StaticMesh`, `Scene`. Assets are imported via `ImportAsset(filepath)` and retrieved via `GetAsset<T>(handle)`.

### C# Scripting (Mono)

`ScriptEngine` loads Mono assemblies (`BSAEngine-ScriptCore.dll`). C# scripts inherit `BSAEngine.Entity` and implement `OnCreate()`, `OnUpdate(float ts)`, `OnDestroy()`. C++↔C# bridge uses Mono InternalCalls. The `ScriptComponent` stores a class name string; `ScriptInstance` holds the MonoObject and cached method pointers.

The C# scripting API project is in `BSAEngine-ScriptCore/`.

### Physics

Box2D for 2D physics. Scene creates/destroys the `b2World`. Bodies are instantiated from `Rigidbody2DComponent` + collider components in `Scene::StartPhysics()`. Fixed timestep update in `Scene::OnFixedUpdate()`.

Jolt Physics is linked but integration is less mature than Box2D.

### Event System

Dispatcher pattern: events are created by the window/input layer, dispatched through layers in reverse stack order. `EventDispatcher::Dispatch<T>(callback)` for type-safe handling.

### Serialization

YAML-based (yaml-cpp). `SceneSerializer` saves/loads full scenes. `PrefabSerializer` handles entity prefabs. Asset references use UUIDs for persistence.

## Key Conventions

- Platform-specific code goes in `src/Platform/` guarded by `BSA_PLATFORM_WINDOWS`, `BSA_PLATFORM_LINUX`, `BSA_PLATFORM_MACOS`
- Logging: use `BSA_ENGINE_TRACE/INFO/WARN/ERROR/FATAL(...)` macros
- New components: define in `include/BSAEngine/Scene/Components.h`
- Vendored code (Glad, Mono, stb_image) lives in `vendor/`
- Build type defines: `BSA_DEBUG`, `BSA_RELEASE`
