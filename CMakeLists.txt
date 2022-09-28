cmake_minimum_required(VERSION 3.21)
project(imgui-glfw-example CXX)

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

include(FetchContent)
FetchContent_Declare(clap GIT_REPOSITORY "https://github.com/free-audio/clap" GIT_TAG "1.1.1")
FetchContent_MakeAvailable(clap)

add_executable(imgui-glfw-example
    src/app/main.cpp
    src/app/opengl_shader.cpp
    src/app/file_manager.cpp
    src/app/opengl_shader.h
    src/app/file_manager.h

    bindings/imgui_impl_glfw.cpp
    bindings/imgui_impl_opengl3.cpp
    bindings/imgui_impl_glfw.h
    bindings/imgui_impl_opengl3.h

    assets/simple-shader.vs
    assets/simple-shader.fs
)

add_custom_command(TARGET imgui-glfw-example
    POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/simple-shader.vs $<TARGET_FILE_DIR:imgui-glfw-example>
    COMMAND
        ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/simple-shader.fs $<TARGET_FILE_DIR:imgui-glfw-example>
)

target_include_directories(imgui-glfw-example PRIVATE ${CMAKE_BINARY_DIR})
target_compile_definitions(imgui-glfw-example PRIVATE IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
target_link_libraries(imgui-glfw-example PRIVATE imgui::imgui GLEW::glew_s glfw::glfw clap-core)


add_library(clap-plugin-template MODULE src/plugin/main.cpp)
target_link_libraries(clap-plugin-template PRIVATE clap-core)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(clap-plugin-template PRIVATE -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/src/linux-my_plug.version)
    target_link_libraries(clap-plugin-template PRIVATE -Wl,-z,defs)
    set_target_properties(clap-plugin-template PROPERTIES SUFFIX ".clap" PREFIX "")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_options(clap-plugin-template PRIVATE -exported_symbols_list ${CMAKE_CURRENT_SOURCE_DIR}/src/macos-symbols.txt)

    set_target_properties(clap-plugin-template PROPERTIES
                BUNDLE True
                BUNDLE_EXTENSION clap
                MACOSX_BUNDLE_GUI_IDENTIFIER com.my_company.my_plug
                MACOSX_BUNDLE_BUNDLE_NAME my_plug
                MACOSX_BUNDLE_BUNDLE_VERSION "1"
                MACOSX_BUNDLE_SHORT_VERSION_STRING "1"
                MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins.plist.in
                )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_options(clap-plugin-template PRIVATE /EXPORT:clap_entry)
    set_target_properties(clap-plugin-template PROPERTIES SUFFIX ".clap" PREFIX "")
endif()