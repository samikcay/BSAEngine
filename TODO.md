# BSAEngine - Bug & Missing Parts Tracker

All items have been addressed.

## CRITICAL (all fixed)

- [x] 1. **Null pointer dereference in ScriptEngine when assembly load fails** — Added null check for `ReadBytes` return value before passing to Mono
- [x] 2. **Spot lights collected but never uploaded to GPU** — Added `SpotLightData` to UBO struct, upload in `EndScene()`, capped at 8
- [x] 3. **PointLight struct layout mismatch (CPU vs GPU)** — Zero-initialized `LightingData` to prevent garbage in padding
- [x] 4. **OpenGLShader filepath constructor is empty stub** — Implemented file-based shader loading with `#type vertex`/`#type fragment` parsing
- [x] 5. **OpenGLVertexBuffer uses GL_STATIC_DRAW for dynamic buffers** — Uses `GL_DYNAMIC_DRAW` when vertices is nullptr
- [x] 6. **OpenGLTexture2D uninitialized members when image load fails** — Zero-initialized all members, guard in destructor
- [x] 7. **Unsupported channel count in texture loading** — Added GL_R8/GL_RG8 support for 1/2 channel images

## MODERATE (all fixed)

- [x] 8. **Renderer::Shutdown() doesn't clean up UBOs** — Added `s_SceneData.reset()`
- [x] 9. **SceneHierarchyPanel only shows entities with RelationshipComponent** — `CreateEntityWithUUID` now adds `RelationshipComponent` by default
- [x] 10. **Serialization incomplete — many components not saved/loaded** — Added serialization for SpotLight, Rigidbody2D, BoxCollider2D, CircleCollider2D, Script, Relationship
- [x] 11. **SceneSerializer::Serialize uses wrong EnTT view** — Changed to `view<IDComponent>()`, fixed `return` to `continue`
- [x] 12. **Framebuffer::ClearAttachment uses wrong GL format** — Uses correct base format (GL_RGBA/GL_RED_INTEGER) and type per attachment
- [x] 13. **DrawComponent assumes font index 1 exists** — Added bounds check before PushFont
- [x] 14. **Application::OnEvent passes event to ImGui even when handled** — Added `e.Handled` guard
- [x] 15. **Raw new/delete in main.cpp** — Changed to `std::make_unique`
- [x] 16. **Scene::OnViewportResize division by zero** — Early return when width or height is 0
- [x] 17. **GLSL version mismatch** — Normalized Renderer2D shaders to `#version 450 core`

## CMAKE (all fixed)

- [x] 18. **GLOB_RECURSE nesting bug** — Moved `${IMGUI_SOURCES}` to `add_executable` instead of inside GLOB_RECURSE
- [x] 19. **Hardcoded Mono .lib path** — Platform-conditional linking: .lib on Windows, find_library on Unix

## MISSING IMPLEMENTATIONS (all addressed)

- [x] 20. `OpenGLShader(filepath)` — Implemented `#type vertex`/`#type fragment` file parsing
- [x] 21. `StaticMesh::Create(filepath)` — Implemented OBJ loader with v/vn/vt/f support and fan triangulation
- [x] 22. `MeshRendererComponent` serialization — Saves MeshHandle (UUID) and MaterialName; loads mesh via AssetManager
- [x] 23. `SerializeRuntime` / `DeserializeRuntime` — Delegates to YAML for now, with warning logged
- [x] 24. Play/Pause/Stop buttons — Full scene state machine (Edit/Play/Pause), physics only runs in Play mode, scene restore on Stop
- [x] 25. PhysicsContactListener collision callbacks — `BeginContact` now calls `OnCollisionBegin` on NativeScriptComponent instances
- [x] 26. EndContact — `EndContact` now calls `OnCollisionEnd` on NativeScriptComponent instances
