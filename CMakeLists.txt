cmake_minimum_required(VERSION 3.21)
project(imgui-glfw-example C CXX)

find_program(CCACHE ccache)
if(CCACHE)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_VISIBILITY_PRESET hidden)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

if(NOT EMSCRIPTEN)
    include(${CMAKE_SOURCE_DIR}/cmake/ta_conan.cmake)
endif()

include(FetchContent)
FetchContent_Declare(clap GIT_REPOSITORY "https://github.com/free-audio/clap" GIT_TAG "1.1.8")
FetchContent_MakeAvailable(clap)

if(EMSCRIPTEN)
    add_subdirectory(example/app-emscripten)
else()
    add_subdirectory(example/app)
    add_subdirectory(example/plugin-minimal)
    add_subdirectory(example/plugin-imgui)
endif()

if(NOT APPLE)
    add_subdirectory(example/app-sdl)
endif()

if(WIN32)
    add_subdirectory(src)
    add_subdirectory(example/app-cairo)
endif()
