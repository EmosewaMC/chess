cmake_minimum_required(VERSION 3.2.0)
project(tictactoe VERSION 0.1.0 LANGUAGES C CXX)

# check for macOS
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(MACOS TRUE)
endif()

# Our output dir
set(CMAKE_BINARY_DIR ${PROJECT_BINARY_DIR})

# TODO make this not have to override the build type directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# for filesystem functionality from C++20
set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

if(MACOS)
	find_package(glfw3 REQUIRED)
	include_directories(${GLFW_INCLUDE_DIRS})
endif()

include(CTest)
enable_testing()

if(MACOS)
	set(MAIN_FILE "src/main_macos.cpp")
	set(IMPL_FILE "src/imgui/imgui_impl_glfw.cpp")
else()
	set(MAIN_FILE "src/main_win32.cpp")
	set(IMPL_FILE "src/imgui/imgui_impl_win32.cpp")
endif()

add_executable(chess
	src/Application.cpp
	src/imgui/imgui_demo.cpp
	src/imgui/imgui_draw.cpp
	src/imgui/imgui_tables.cpp
	src/imgui/imgui_widgets.cpp
	src/imgui/imgui.cpp
	src/imgui/imgui_impl_opengl3.cpp
	src/classes/Bit.cpp
	src/classes/BitHolder.cpp
	src/classes/ChessSquare.cpp
	src/classes/Game.cpp
	src/classes/Chess.cpp
	src/classes/Sprite.cpp
	src/classes/Square.cpp
	src/Logger.cpp
	${MAIN_FILE}
	${IMPL_FILE}
)

target_include_directories(chess PRIVATE src)

target_compile_options(chess PRIVATE "/W4")

target_link_libraries(chess ${OPENGL_gl_LIBRARY})

if(MACOS)
	target_link_libraries(chess glfw)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)
