cmake_minimum_required(VERSION 3.16)
project(qlp LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_STANDARD 23)

if (WIN32)
    option(BUILD_SHARED_LIBS "Build shared libraries" ON)
else ()
    option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
endif()

set(BOX2D_BUILD_UNIT_TESTS OFF)
set(BOX2D_BUILD_TESTBED OFF)

include(FetchContent)

FetchContent_Declare(box2d
	GIT_REPOSITORY https://github.com/erincatto/box2d.git
	GIT_TAG 411acc3)
FetchContent_MakeAvailable(box2d)

FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(ImGui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG v1.90.4)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)

set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_Declare(ImGui-SFML
        GIT_REPOSITORY https://github.com/SFML/imgui-sfml
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(ImGui-SFML)

FetchContent_Declare(glm
        GIT_REPOSITORY	https://github.com/g-truc/glm.git
        GIT_TAG 	1.0.1)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.11.3)
FetchContent_MakeAvailable(nlohmann_json)

set(BOOST_INCLUDE_LIBRARIES asio)
set(BOOST_ENABLE_CMAKE ON)
FetchContent_Declare(Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.85.0/boost-1.85.0-cmake.tar.xz
    URL_MD5 badea970931766604d4d5f8f4090b176
)
FetchContent_MakeAvailable(Boost)

FetchContent_Declare(
        zlib-cmake
        URL https://github.com/jimmy-park/zlib-cmake/archive/main.tar.gz
)
FetchContent_MakeAvailable(zlib-cmake)

FetchContent_Declare(qlp-proto
    GIT_REPOSITORY https://github.com/kmrd-industries/qlp-proto-bindings
    GIT_TAG 31-AI-pierwsza-iteracja)
FetchContent_MakeAvailable(qlp-proto)

add_subdirectory(src)
install(TARGETS qlp)
