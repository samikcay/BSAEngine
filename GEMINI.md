# BSAEngine - Project Overview

BSAEngine is a custom, cross-platform C++ game engine designed with a layered architecture and integrated C# scripting capabilities.

## Architecture & Core Technologies

- **Language:** C++17
- **Graphics API:** OpenGL (via Glad loader and GLFW for windowing)
- **Math Library:** GLM (integrated with custom Math wrappers in `BSAEngine/Math`)
- **UI Framework:** ImGui (Docking branch) for the editor and tools
- **ECS:** EnTT (Entity Component System)
- **Physics:** Integrated Box2D (2D) and Jolt Physics (3D)
- **Scripting:** C# support via Mono (C# side defined in `BSAEngine-ScriptCore`)
- **Logging:** spdlog (wrapped in `BSA::Log`)
- **Asset Loading:** stb_image for textures

## Project Structure

- `include/BSAEngine/`: Core engine headers
  - `Core/`: Application, Layer, Input, and Windowing systems
  - `Scene/`: Entity-Component-System (ECS) implementation and Scene management
  - `Scripting/`: C#/C++ interop and Mono integration
  - `Renderer/`: Rendering abstractions (Shaders, Textures, VertexArrays, etc.)
  - `Math/`: Math primitives and utility functions
  - `Panels/`: ImGui-based editor panels
- `src/`: Implementation files corresponding to the headers
- `BSAEngine-ScriptCore/`: C# source code for the engine's scripting API
- `vendor/`: Third-party dependencies (Glad, Mono, stb_image)
- `assets/`: Runtime assets such as shaders and textures

## Building and Running

### Prerequisites
- CMake 3.20 or higher
- A C++17 compatible compiler (MSVC, GCC, or Clang)
- Mono JIT runtime (required for scripting support)

### Building
```powershell
# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build the project (e.g., using MSVC)
cmake --build . --config Debug
```

### Running
The executable is generated in `bin/Debug` (or `bin/Release`).
Ensure that `BSAEngine-ScriptCore.dll` (compiled from the C# source) is present in the same directory or a location accessible by the `ScriptEngine`.

## Development Conventions

### Logging
Always use the provided logging macros for engine and application logs:
- `BSA_ENGINE_TRACE(...)`
- `BSA_ENGINE_INFO(...)`
- `BSA_ENGINE_WARN(...)`
- `BSA_ENGINE_ERROR(...)`
- `BSA_ENGINE_FATAL(...)`

### ECS & Components
New components should be defined in `include/BSAEngine/Scene/Components.h`.
- Use `entt::entity` for underlying IDs.
- Use `BSA::Entity` as a wrapper for safer access to components.

### Scripting
C# scripts must inherit from `BSAEngine.Entity`.
- Lifecycle methods: `OnCreate()`, `OnUpdate(float ts)`, `OnDestroy()`.
- C++/C# communication is handled via `InternalCalls`.

### Platform Specifics
Platform-specific code should be isolated in `src/Platform/` and guarded with platform definitions:
- `BSA_PLATFORM_WINDOWS`
- `BSA_PLATFORM_LINUX`
- `BSA_PLATFORM_MACOS`
