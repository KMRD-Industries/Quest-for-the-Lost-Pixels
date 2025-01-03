if (DEFINED ENV{QLP_DATA_PATH})
    set(ASSET_DIR $ENV{QLP_DATA_PATH})
else ()
    set(ASSET_DIR ${CMAKE_SOURCE_DIR}/assets)
endif ()

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
        Components/SynchronisedEvent.h
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
        Components/TextTagComponent.h
        Components/PassageComponent.h
        Components/FloorComponent.h
        Components/CreateBodyWithCollisionEvent.h
        Components/FightActionEvent.h
        Components/HelmetComponent.h
        Components/PotionComponent.h
        Components/BodyArmourComponent.h
        Components/EquipmentComponent.h
        Components/SoundComponent.h
        Components/WeaponSwingComponent.h
        Components/BindSwingWeaponEvent.h
        Components/DealDMGToEnemyEvent.h
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
        Systems/TextTagSystem.cpp
        Systems/TextTagSystem.h
        Systems/PassageSystem.cpp
        Systems/PassageSystem.h
        Systems/ItemSystem.cpp
        Systems/ItemSystem.h
		Systems/SoundSystem.cpp
		Systems/SoundSystem.h
        Systems/WeaponBindSystem.cpp
        Systems/WeaponBindSystem.h        
        Systems/DealDMGToEnemySystem.cpp
        Systems/DealDMGToEnemySystem.h
)

set(EVENT_SYSTEMS
        EventSystems/ObjectCreatorSystem.cpp
        EventSystems/ObjectCreatorSystem.h
        EventSystems/FightSystem.cpp
        EventSystems/FightSystem.h
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
        Utils/PublicConfig.h
        Utils/PublicConfigMenager.h
        Utils/TextureParser.h
        Utils/TextTag.h
        Utils/GameUtility.h
)

set(MENU_STATE_MACHINE
        MenuStateMachine/EndGameState.cpp
        MenuStateMachine/EndGameState.h
		MenuStateMachine/LostGameState.cpp
        MenuStateMachine/LostGameState.h
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
        Ui/TextDTO.h
        Ui/Text.h
        Ui/Text.cpp
)

set(Sounds
        Sounds/SoundManager.h
        Sounds/SoundPlayer.h
)


source_group("Core" FILES ${CORE})
source_group("Components" FILES ${COMPONENTS})
source_group("Systems" FILES ${SYSTEMS})
source_group("AtlasComponents" FILES ${ATLAS_COMPONENTS})
source_group("Utils" FILES ${UTILS})
source_group("Map" FILES ${MAP})
source_group("EventSystems" FILES ${EVENT_SYSTEMS})
source_group("MenuStateMachine" FILES ${MENU_STATE_MACHINE})
source_group("UiComponents" FILES ${UI_COMPONENTS})
source_group("UiSystems" FILES ${UI_SYSTEMS})
source_group("Ui" FILES ${UI})
source_group("Sounds" FILES ${Sounds})

add_definitions(-D_ASSET_PATH=\"${ASSET_DIR}\")
include_directories(./Core ./Components ./Systems ./Map ./Utils ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(./AtlasComponents ./Core ./Components ./Systems ./EventSystems ./MenuStateMachine ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(./UiComponents ./UiSystems ./Ui ./Sounds ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(qlp ${SOURCES} ${CORE} ${EVENT_SYSTEMS} ${SYSTEMS} ${MAP} ${COMPONENTS} ${UTILS} ${ATLAS_COMPONENTS} ${MENU_STATE_MACHINE} ${UI_COMPONENTS} ${UI_SYSTEMS} ${UI} ${Sounds})

target_link_libraries(qlp PRIVATE nlohmann_json sfml-graphics sfml-audio ImGui-SFML::ImGui-SFML glm::glm box2d Boost::asio qlp-proto)

if (WIN32)
    target_link_libraries(qlp PRIVATE ws2_32)
    set(CMAKE_AR lib.exe)
    set(CMAKE_ASM_MASM_CREATE_STATIC_LIBRARY "<CMAKE_AR> /OUT:<TARGET> <LINK_FLAGS> <OBJECTS>")
endif ()

target_compile_features(qlp PRIVATE cxx_std_23)

if (WIN32)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH_DIR "x64")
    else ()
        set(ARCH_DIR "x86")
    endif ()

    set(BOX2D_SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/box2d-build/bin")
    set(ADDITIONAL_DLL_DIR "${CMAKE_BINARY_DIR}/bin")
    set(OPENAL_DLL_PATH "${SFML_SOURCE_DIR}/extlibs/bin/${ARCH_DIR}/openal32.dll")

    if (EXISTS ${BOX2D_SOURCE_DIR})
        message(STATUS "Box2d source dir found: ${BOX2D_SOURCE_DIR}. Copying Box2d dll.")
        add_custom_command(
                TARGET qlp
                COMMENT "Copy box2d dll"
                PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${BOX2D_SOURCE_DIR} $<TARGET_FILE_DIR:qlp>
                VERBATIM
        )
    else ()
        message(WARNING "Directory does not exist: ${BOX2D_SOURCE_DIR}")
    endif ()

    if (EXISTS ${OPENAL_DLL_PATH})
        message(STATUS "OpenAL source dir found: ${OPENAL_DLL_PATH}. Copying OpenAL dll.")
        add_custom_command(
                TARGET qlp
                COMMENT "Copy openAL dll"
                PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${OPENAL_DLL_PATH}"
                "$<TARGET_FILE_DIR:qlp>/openal32.dll"
                COMMENT "Copy OpenAL32.dll"
                VERBATIM
        )
    else ()
        message(WARNING "Directory does not exist: ${OPENAL_DLL_PATH}")
    endif ()

    if (EXISTS "${ADDITIONAL_DLL_DIR}")
        message(STATUS "Additional Dll dir found: ${ADDITIONAL_DLL_DIR}. Copying libabsl dlls.")
        add_custom_command(
                TARGET qlp
                COMMENT "Copy libabsl dlls"
                PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${ADDITIONAL_DLL_DIR} $<TARGET_FILE_DIR:qlp>
                VERBATIM
        )
    else ()
        message(WARNING "Directory does not exist: ${ADDITIONAL_DLL_DIR}")
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
