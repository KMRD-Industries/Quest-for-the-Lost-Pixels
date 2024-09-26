set(ASSET_DIR ${CMAKE_SOURCE_DIR}/assets)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

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
        Components/MultiplayerComponent.h
        Components/ColliderComponent.h
        Components/CharacterComponent.h
        Components/TransformComponent.h
        Components/TileComponent.h
        Components/MapComponent.h
        Components/ItemComponent.h
        Components/ItemAnimationComponent.h
        Components/LootComponent.h
        Components/ChestComponent.h
        Components/AnimationComponent.h
        Components/AnimationFrame.h
        Components/DoorComponent.h
        Components/TravellingDungeonComponent.h
        Components/SpawnerComponent.h
        Components/EnemyComponent.h
        Components/WeaponComponent.h
        Components/InventoryComponent.h
        Components/EquippedWeaponComponent.h
        Components/TextTagComponent.h
        Components/PassageComponent.h
        Components/FloorComponent.h
        Components/CreateBodyWithCollisionEvent.h
        Components/FightActionEvent.h
        Components/BulletComponent.h
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
        Systems/ItemSpawnerSystem.cpp
        Systems/ItemSpawnerSystem.h
        Systems/ChestSystem.cpp
        Systems/ChestSystem.h
        Systems/HealthBarSystem.cpp
        Systems/HealthBarSystem.h
        Systems/TextureSystem.cpp
        Systems/AnimationSystem.cpp
        Systems/DoorSystem.cpp
        Systems/DoorSystem.h
        Systems/ChestSpawnerSystem.cpp
        Systems/ChestSpawnerSystem.h
        Systems/RoomListenerSystem.cpp
        Systems/RoomListenerSystem.h
        Systems/TravellingSystem.cpp
        Systems/TravellingSystem.h
        Systems/EnemySystem.cpp
        Systems/SpawnerSystem.cpp
        Systems/AnimationSystem.h
        Systems/WeaponsSystem.cpp
        Systems/WeaponsSystem.h
        Systems/InventorySystem.cpp
        Systems/InventorySystem.h
        Systems/EquipWeaponSystem.cpp
        Systems/EquipWeaponSystem.h
        Systems/TextTagSystem.cpp
        Systems/TextTagSystem.h
        Systems/PassageSystem.cpp
        Systems/PassageSystem.h
        Systems/BulletSystem.h
        Systems/BulletSystem.cpp
)

set(EVENT_SYSTEMS
        EventSystems/ObjectCreatorSystem.cpp
        EventSystems/ObjectCreatorSystem.h
        EventSystems/FightSystem.cpp
        EventSystems/FightSystem.h
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
        Utils/MapParser.h
        Utils/Config.h
        Utils/TextureParser.h
        Utils/TextTag.h
        Utils/GameUtility.h
)

source_group("Core" FILES ${CORE})
source_group("Components" FILES ${COMPONENTS})
source_group("Systems" FILES ${SYSTEMS})
source_group("AtlasComponents" FILES ${ATLAS_COMPONENTS})
source_group("Utils" FILES ${UTILS})
source_group("Map" FILES ${MAP})
source_group("EventSystems" FILES ${EVENT_SYSTEMS})

add_definitions(-D_ASSET_PATH=\"${ASSET_DIR}\")
include_directories(./Core ./Components ./Systems ./Map ./Utils ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(./AtlasComponents ./Core ./Components ./Systems ./EventSystems ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(qlp ${SOURCES} ${CORE} ${EVENT_SYSTEMS} ${SYSTEMS} ${MAP} ${COMPONENTS} ${UTILS} ${ATLAS_COMPONENTS})

target_link_libraries(qlp PRIVATE nlohmann_json sfml-graphics ImGui-SFML::ImGui-SFML glm::glm box2d Boost::asio qlp-proto)

if (WIN32)
    target_link_libraries(qlp PRIVATE ws2_32)
endif ()

target_compile_features(qlp PRIVATE cxx_std_23)
if (WIN32)
    set(BOX2D_SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/box2d-build/bin")
    set(ADDITIONAL_DLL_DIR "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}")
    set(SFML_DLL_DIR "${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>")

    add_custom_target(copy_dlls ALL
            COMMENT "Copy necessary DLLs"
            VERBATIM
    )

    function(copy_if_exists src dest target)
        if (EXISTS ${src})
            add_custom_command(
                    TARGET ${target}
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${src} ${dest}
                    VERBATIM
            )
        endif ()
    endfunction()

    if (EXISTS ${BOX2D_SOURCE_DIR})
        add_custom_command(
                TARGET copy_dlls
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${BOX2D_SOURCE_DIR} $<TARGET_FILE_DIR:qlp>
                VERBATIM
        )
    endif ()

    # Copy OpenAL DLL from SFML
    copy_if_exists(${SFML_DLL_DIR}/openal32.dll $<TARGET_FILE_DIR:qlp> copy_dlls)

    # Copy abseil_dll.dll if it exists
    copy_if_exists(${ADDITIONAL_DLL_DIR}/abseil_dll.dll $<TARGET_FILE_DIR:qlp> copy_dlls)

    # Copy libprotobufd.dll if it exists
    copy_if_exists(${ADDITIONAL_DLL_DIR}/libprotobufd.dll $<TARGET_FILE_DIR:qlp> copy_dlls)

    # Copy libprotobuf.dll if it exists
    copy_if_exists(${ADDITIONAL_DLL_DIR}/libprotobuf.dll $<TARGET_FILE_DIR:qlp> copy_dlls)

endif ()