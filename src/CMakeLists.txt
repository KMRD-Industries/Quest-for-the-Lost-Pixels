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
)

set(UI_COMPONENTS
		UiComponents/ButtonComponent.h
		UiComponents/UiComponent.h
)

set(ATLAS_COMPONENTS
        AtlasComponents/Texture.h
        AtlasComponents/Map.h
        AtlasComponents/Tileset.h
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
)

set(UI_SYSTEMS
        UiSystems/HealthBarSystem.cpp
        UiSystems/HealthBarSystem.h        
		UiSystems/ButtonSystem.cpp
        UiSystems/ButtonSystem.h		
		UiSystems/BackgroundSystem.cpp
        UiSystems/BackgroundSystem.h
)

set(UTILS
        Utils/Paths.h
        Utils/Helpers.h
        Utils/GameTypes.h
        Utils/MapParser.h
        Utils/Config.h
        Utils/TextureParser.h
        Utils/CircularIterator.h
        Utils/TextTag.h
)

set(MENU_STATE_MACHINE
        MenuStateMachine/MainMenuState.cpp
        MenuStateMachine/MainMenuState.h
        MenuStateMachine/GameState.cpp
        MenuStateMachine/GameState.h
        MenuStateMachine/State.cpp       
        MenuStateMachine/State.h        
        MenuStateMachine/StateMachine.cpp        
        MenuStateMachine/StateMachine.h        
)

set(UI
		Ui/ButtonDTO.h
		Ui/Button.h
		Ui/Button.cpp
		Ui/ResourceManager.h
		Ui/Graphics.h
		Ui/Graphics.cpp
		Ui/InputTextDTO.h
		Ui/InputText.h
		Ui/InputText.cpp
)


source_group("Core" FILES ${CORE})
source_group("Components" FILES ${COMPONENTS})
source_group("Systems" FILES ${SYSTEMS})
source_group("AtlasComponents" FILES ${ATLAS_COMPONENTS})
source_group("Utils" FILES ${UTILS})
source_group("Map" FILES ${MAP})
source_group("MenuStateMachine" FILES ${MENU_STATE_MACHINE})
source_group("UiComponents" FILES ${UI_COMPONENTS})
source_group("UiSystems" FILES ${UI_SYSTEMS})
source_group("Ui" FILES ${UI})

add_definitions(-D_ASSET_PATH=\"${ASSET_DIR}\")
include_directories(./Core ./Components ./Systems ./Map ./Utils ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(./AtlasComponents ./Core ./Components ./Systems ./MenuStateMachine ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(./UiComponents ./UiSystems ./Ui ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(qlp ${SOURCES} ${CORE} ${SYSTEMS} ${MAP} ${COMPONENTS} ${UTILS} ${ATLAS_COMPONENTS} ${MENU_STATE_MACHINE} ${UI_COMPONENTS} ${UI_SYSTEMS} ${UI})

target_link_libraries(qlp PRIVATE nlohmann_json sfml-graphics ImGui-SFML::ImGui-SFML glm::glm box2d Boost::asio qlp-proto)

if(WIN32)
    target_link_libraries(qlp PRIVATE ws2_32)
endif()

target_compile_features(qlp PRIVATE cxx_std_23)

if (WIN32)
    set(BOX2D_SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/box2d-build/bin")
    set(ADDITIONAL_DLL_DIR "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}")

    if (EXISTS  ${BOX2D_SOURCE_DIR} )
        add_custom_command(
                TARGET qlp
                COMMENT "Copy box2d BLLS"
                PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory  ${BOX2D_SOURCE_DIR} $<TARGET_FILE_DIR:qlp>
                VERBATIM
        )
    endif ()

    if(EXISTS  ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>)
        add_custom_command(
                TARGET qlp
                COMMENT "Copy box2d BLLS"
                PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:qlp>
                VERBATIM
        )
    endif ()

    if (EXISTS  ${ADDITIONAL_DLL_DIR} )
        add_custom_command(
            TARGET qlp
            COMMENT "Copy abseil_dll DLLs"
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${ADDITIONAL_DLL_DIR}/abseil_dll.dll $<TARGET_FILE_DIR:qlp>
            VERBATIM
    )
    endif ()

    # Dodanie komendy do kopiowania libprotobufd.dll, jeśli istnieje
    if (EXISTS ${ADDITIONAL_DLL_DIR}/libprotobufd.dll)
        add_custom_command(
                TARGET qlp
                PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${ADDITIONAL_DLL_DIR}/libprotobufd.dll $<TARGET_FILE_DIR:qlp>
                VERBATIM
        )
    endif ()

    if (EXISTS ${ADDITIONAL_DLL_DIR}/libprotobuf.dll)
        add_custom_command(
                TARGET qlp
                PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${ADDITIONAL_DLL_DIR}/libprotobuf.dll $<TARGET_FILE_DIR:qlp>
                VERBATIM
        )
    endif ()
endif ()