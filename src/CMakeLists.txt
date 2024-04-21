set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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
)
set(SYSTEMS
	Systems/RenderSystem.cpp
	Systems/RenderSystem.h	
	Systems/PlayerMovementSystem.cpp
	Systems/PlayerMovementSystem.h
)
source_group("Core" FILES ${CORE})
source_group("Components" FILES ${COMPONENTS})
source_group("Systems" FILES ${SYSTEMS})

include_directories(./Core ./Components ./Systems ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(sf ${SOURCES} ${CORE} ${SYSTEMS} ${COMPONENTS})
target_link_libraries(sf PRIVATE sfml-graphics ImGui-SFML::ImGui-SFML)
target_compile_features(sf PRIVATE cxx_std_23)

if(WIN32)
    add_custom_command(
        TARGET sf
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:sf>
        VERBATIM)
endif()