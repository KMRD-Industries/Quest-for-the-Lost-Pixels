set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(ASSET_DIR ${CMAKE_SOURCE_DIR}/assets)

set(SOURCES
        main.cpp
)

set(CORE
        Core/ComponentArray.h
        Core/ComponentManager.cpp
        Core/ComponentManager.h
        Core/EntityManager.cpp
        Core/EntityManager.h
        Core/IComponentArray.h
        Core/System.h
        Core/SystemManager.cpp
        Core/SystemManager.h
        Core/Types.h
        Core/Coordinator.h
        Core/Coordinator.cpp
)

set(COMPONENTS
        Components/RenderComponent.h
        Components/TransformComponent.h
        Components/PlayerComponent.h
        Components/TransformComponent.h
        Components/TileComponent.h
        Components/PositionComponent.h
        Components/MapComponent.h
        Components/AnimationComponent.h
)

set(SYSTEMS
        Systems/RenderSystem.cpp
        Systems/RenderSystem.h
        Systems/PlayerMovementSystem.cpp
        Systems/PlayerMovementSystem.h
        Systems/MapSystem.cpp
        Systems/MapSystem.h
        Systems/TextureSystem.cpp
        Systems/AnimationSystem.cpp
)

set(ATLAS_COMPONENTS
        AtlasComponents/Texture.cpp
        AtlasComponents/Texture.h
        AtlasComponents/TextureAtlas.cpp
        AtlasComponents/TextureAtlas.h
)

set(UTILS
        Utils/Paths.h
        Utils/Helpers.h
        Systems/TextureSystem.cpp
)

source_group("Core" FILES ${CORE})
source_group("Components" FILES ${COMPONENTS})
source_group("Systems" FILES ${SYSTEMS})
source_group("AtlasComponents" FILES ${ATLAS_COMPONENTS})
source_group("Utils" FILES ${UTILS})


add_definitions(-D_ASSET_PATH=\"${ASSET_DIR}\")
include_directories(./Core ./Components ./Systems ./Utils ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(./AtlasComponents ./Core ./Components ./Systems ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(sf ${SOURCES} ${CORE} ${SYSTEMS} ${COMPONENTS} ${UTILS} ${ATLAS_COMPONENTS})
target_link_libraries(sf PRIVATE nlohmann_json sfml-graphics ImGui-SFML::ImGui-SFML glm::glm)
target_compile_features(sf PRIVATE cxx_std_23)

# Force using zlib
#add_compile_definitions(USE_ZLIB)

if (WIN32)
    # Copy OpenAL DLL
    add_custom_command(
            TARGET sf
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:sf>
            VERBATIM
    )
endif ()