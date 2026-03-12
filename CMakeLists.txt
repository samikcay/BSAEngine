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

# GLAD (OpenGL context yükleyici)
add_library(Glad STATIC vendor/Glad/glad.c)
target_include_directories(Glad PUBLIC vendor/Glad/include)

# -------------------------------------------------------------------
# Kaynak Dosyaları
# -------------------------------------------------------------------
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
add_executable(BSAEngine ${BSA_SOURCES} ${BSA_HEADERS})

target_include_directories(BSAEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor/stb_image
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
