cmake_minimum_required(VERSION 3.21)
project(clap-examples CXX)

find_program(CCACHE ccache)
if(CCACHE)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
file(
    DOWNLOAD
        "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
        "${CMAKE_BINARY_DIR}/conan.cmake"
    TLS_VERIFY
        ON
)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_add_remote(NAME conancenter INDEX 0 URL https://center.conan.io)
conan_cmake_autodetect(settings)
conan_cmake_install(
    CONANFILE
        conanfile.txt
    PATH_OR_REFERENCE
        ${CMAKE_CURRENT_SOURCE_DIR}
    BUILD
        missing
    SETTINGS
        ${settings}
)

find_package(imgui CONFIG)
find_package(glfw3 CONFIG)
find_package(glew CONFIG)

add_executable(clap-examples
    src/main.cpp
    src/opengl_shader.cpp
    src/file_manager.cpp
    src/opengl_shader.h
    src/file_manager.h

    bindings/imgui_impl_glfw.cpp
    bindings/imgui_impl_opengl3.cpp
    bindings/imgui_impl_glfw.h
    bindings/imgui_impl_opengl3.h

    assets/simple-shader.vs
    assets/simple-shader.fs
)

add_custom_command(TARGET clap-examples
    POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/simple-shader.vs $<TARGET_FILE_DIR:clap-examples>
    COMMAND
        ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/simple-shader.fs $<TARGET_FILE_DIR:clap-examples>
)

target_include_directories(clap-examples PRIVATE ${CMAKE_BINARY_DIR})
target_compile_definitions(clap-examples PRIVATE IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
target_link_libraries(clap-examples PRIVATE imgui::imgui GLEW::glew_s glfw::glfw)
