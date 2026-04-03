cmake_minimum_required(VERSION 3.20)
project(BSAEngine VERSION 0.1.0 LANGUAGES CXX)

# -------------------------------------------------------------------
# C++ Standart Ayarları
# -------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------------------------------------------------------
# Build Tipi (varsayılan: Debug)
# -------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
message(STATUS "[BSAEngine] Build Type: ${CMAKE_BUILD_TYPE}")

# -------------------------------------------------------------------
# Çıktı Dizinleri
# -------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE})

# -------------------------------------------------------------------
# Platform Tanımları
# -------------------------------------------------------------------
if(WIN32)
    add_definitions(-DBSA_PLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    add_definitions(-DBSA_PLATFORM_LINUX)
elseif(APPLE)
    add_definitions(-DBSA_PLATFORM_MACOS)
endif()

# -------------------------------------------------------------------
# Build Tipi Tanımları
# -------------------------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DBSA_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DBSA_RELEASE)
endif()

# -------------------------------------------------------------------
# Üçüncü Parti Kütüphaneler (FetchContent)
# -------------------------------------------------------------------
include(FetchContent)

# ImGui (Docking Branch)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
)
FetchContent_MakeAvailable(imgui)

# spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.1
)
FetchContent_MakeAvailable(spdlog)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

# GLM (OpenGL Mathematics)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1 # Veya master branch'teki stabil bir sürüm
)
FetchContent_MakeAvailable(glm)

# EnTT (Entity Component System)
FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG        v3.13.0
)
FetchContent_MakeAvailable(entt)

# Box2D (2D Fizik Motoru)
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG        v2.4.2
)
FetchContent_MakeAvailable(box2d)

# Jolt Physics (3D Fizik Motoru)
set(TARGET_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(TARGET_HELLO_WORLD OFF CACHE BOOL "" FORCE)
set(TARGET_PERFORMANCE_TEST OFF CACHE BOOL "" FORCE)
set(TARGET_SAMPLES OFF CACHE BOOL "" FORCE)
set(TARGET_VIEWER OFF CACHE BOOL "" FORCE)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    JoltPhysics
    GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
    GIT_TAG        v5.2.0
    SOURCE_SUBDIR  Build
)
FetchContent_MakeAvailable(JoltPhysics)

# yaml-cpp (Serialization)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG        0.8.0
)
FetchContent_MakeAvailable(yaml-cpp)

# GLAD (OpenGL context yükleyici)
add_library(Glad STATIC vendor/Glad/glad.c)
target_include_directories(Glad PUBLIC vendor/Glad/include)

# -------------------------------------------------------------------
# Kaynak Dosyaları
# -------------------------------------------------------------------
file(GLOB IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/*.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

file(GLOB_RECURSE BSA_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/vendor/stb_image/stb_image.cpp
)

file(GLOB_RECURSE BSA_HEADERS
    ${CMAKE_SOURCE_DIR}/include/*.h
    ${CMAKE_SOURCE_DIR}/include/*.hpp
)

# -------------------------------------------------------------------
# Hedef: BSAEngine (çalıştırılabilir)
# -------------------------------------------------------------------
add_executable(BSAEngine ${BSA_SOURCES} ${BSA_HEADERS} ${IMGUI_SOURCES})

target_include_directories(BSAEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor/stb_image
    ${imgui_SOURCE_DIR}          # ImGui Core Headerları
    ${imgui_SOURCE_DIR}/backends # ImGui Backend Headerları
    ${CMAKE_SOURCE_DIR}/vendor/mono/include/mono-2.0
)

# -------------------------------------------------------------------
# Kütüphane Bağlantıları
# -------------------------------------------------------------------
target_link_libraries(BSAEngine PRIVATE
    spdlog::spdlog
    glfw
    glm::glm
    EnTT::EnTT
    Glad
    box2d
    Jolt
    yaml-cpp
)

if(WIN32)
    target_link_libraries(BSAEngine PRIVATE ${CMAKE_SOURCE_DIR}/vendor/mono/lib/mono-2.0-sgen.lib)
elseif(UNIX)
    find_library(MONO_LIB mono-2.0 PATHS /usr/lib /usr/local/lib)
    if(MONO_LIB)
        target_link_libraries(BSAEngine PRIVATE ${MONO_LIB})
    else()
        message(WARNING "Mono library not found. Scripting support will not be available.")
    endif()
endif()

# -------------------------------------------------------------------
# Post-Build: Copy Assets and Mono DLL
# -------------------------------------------------------------------
add_custom_command(TARGET BSAEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:BSAEngine>/assets"
)

add_custom_command(TARGET BSAEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/vendor/mono/bin/mono-2.0-sgen.dll"
        $<TARGET_FILE_DIR:BSAEngine>
)

# -------------------------------------------------------------------
# Derleyici Uyarıları
# -------------------------------------------------------------------
if(MSVC)
    target_compile_options(BSAEngine PRIVATE /W4)
else()
    target_compile_options(BSAEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()

message(STATUS "[BSAEngine] Version: ${PROJECT_VERSION}")
message(STATUS "[BSAEngine] Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "[BSAEngine] Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
