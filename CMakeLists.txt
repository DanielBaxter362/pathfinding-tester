cmake_minimum_required(VERSION 3.20)
project(PathfindingApp)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(PathfindingApp)

file(GLOB OG_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS "external/imgui/*.cpp")

target_sources(PathfindingApp 
	PRIVATE
	  ${OG_SOURCES}
	  ${IMGUI_SOURCES}
	  external/glad/glad.c
)

target_include_directories(PathfindingApp
	PRIVATE 
	  include/
	  include/fonts/
	  external/imgui/include
	  external/glad
	  external/glfw/include
)

target_link_libraries(PathfindingApp
	PRIVATE
	  ${CMAKE_SOURCE_DIR}/external/glfw/lib-vc2022/glfw3.lib
)