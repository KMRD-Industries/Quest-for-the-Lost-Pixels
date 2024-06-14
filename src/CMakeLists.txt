set(ASSET_DIR ${CMAKE_SOURCE_DIR}/assets)

set(SOURCES
        main.cpp
        Game.cpp
        Game.h
)

set(CORE
        Core/ComponentArray.h
        Core/Physics.h
        Core/Physics.cpp
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
        Core/InputHandler.h
        Core/InputHandler.cpp
)

set(COMPONENTS
        Components/RenderComponent.h
        Components/TransformComponent.h
        Components/PlayerComponent.h
        Components/ColliderComponent.h
        Components/CharacterComponent.h
        Components/TransformComponent.h
        Components/TileComponent.h
        Components/MapComponent.h
        Components/AnimationComponent.h
        Components/AnimationFrame.h
        Components/DoorComponent.h
        Components/TravellingDungeonComponent.h
)

set(MAP
        Map/DungeonGenerator.cpp
        Map/DungeonGenerator.h
        Map/FloorGenerator.cpp
        Map/FloorGenerator.h
        Map/Dungeon.cpp
        Map/Dungeon.h
        Map/Room.cpp
        Map/Room.h
)

set(SYSTEMS
        Systems/RenderSystem.cpp
        Systems/RenderSystem.cpp
        Systems/CharacterSystem.cpp
        Systems/CharacterSystem.h
        Systems/PlayerMovementSystem.cpp
        Systems/PlayerMovementSystem.h
        Systems/MultiplayerSystem.cpp
        Systems/MultiplayerSystem.h
        Systems/CollisionSystem.cpp
        Systems/CollisionSystem.h
        Systems/MapSystem.cpp
        Systems/MapSystem.h
        Systems/TextureSystem.cpp
        Systems/AnimationSystem.cpp
        Systems/DoorSystem.cpp
        Systems/DoorSystem.h
        Systems/TravellingSystem.cpp
        Systems/TravellingSystem.h
)

set(ATLAS_COMPONENTS
        AtlasComponents/Texture.h
        AtlasComponents/Map.h
        AtlasComponents/Tileset.h
)

set(UTILS
        Utils/Paths.h
        Utils/Helpers.h
        Utils/GameTypes.h
        Utils/Config.h
        Utils/MapParser.h
        Utils/TextureParser.h
)

source_group("Core" FILES ${CORE})
source_group("Components" FILES ${COMPONENTS})
source_group("Systems" FILES ${SYSTEMS})
source_group("AtlasComponents" FILES ${ATLAS_COMPONENTS})
source_group("Utils" FILES ${UTILS})
source_group("Map" FILES ${MAP})

add_definitions(-D_ASSET_PATH=\"${ASSET_DIR}\")
include_directories(./Core ./Components ./Systems ./Map ./Utils ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(./AtlasComponents ./Core ./Components ./Systems ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(qlp ${SOURCES} ${CORE} ${SYSTEMS} ${MAP} ${COMPONENTS} ${UTILS} ${ATLAS_COMPONENTS})
target_link_libraries(qlp PRIVATE nlohmann_json sfml-graphics ImGui-SFML::ImGui-SFML glm::glm box2d Boost::asio qlp-proto)
target_compile_features(qlp PRIVATE cxx_std_23)

if (WIN32)
    add_custom_command(
            TARGET qlp
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:qlp>
            VERBATIM
    )
endif ()
