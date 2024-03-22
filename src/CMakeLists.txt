set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(ZLIB_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/zlib-1.3.1/)

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
		Core/Coordinator.cpp
		Core/ComponentArray.h
)

set(COMPONENTS
		Components/RenderComponent.h
)

set(SYSTEMS
		Systems/RenderSystem.cpp
		Systems/RenderSystem.h
)

set(ATLAS_COMPONENTS
		AtlasComponents/Sprite.cpp
		AtlasComponents/Sprite.h
		AtlasComponents/SpriteBatch.cpp
		AtlasComponents/SpriteBatch.h
		AtlasComponents/Texture.cpp
		AtlasComponents/Texture.h
		AtlasComponents/TextureAtlas.cpp
		AtlasComponents/TextureAtlas.h
)

source_group("Core" FILES ${CORE})
source_group("Components" FILES ${COMPONENTS})
source_group("Systems" FILES ${SYSTEMS})
source_group("AtlasComponents" FILES ${ATLAS_COMPONENTS})

include_directories( ./AtlasComponents ./Core ./Components ./Systems ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(sf ${SOURCES} ${SYSTEMS} ${CORE} ${COMPONENTS} ${ATLAS_COMPONENTS})
target_link_libraries(sf PRIVATE sfml-graphics ImGui-SFML::ImGui-SFML ${ZLIB_LIBRARY_DIR}/libz.so)

add_compile_definitions(USE_ZLIB)
target_compile_features(sf PRIVATE cxx_std_23)

if(WIN32)
	# Copy OpenAL DLL
	add_custom_command(
			TARGET sf
			COMMENT "Copy OpenAL DLL"
			PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:sf>
			VERBATIM
	)
endif()