cmake_minimum_required(VERSION 3.21)
project(plugin-imgui)

find_package(imgui CONFIG)
find_package(glfw3 CONFIG)
find_package(glew CONFIG)

add_library(plugin-imgui MODULE)

target_sources(plugin-imgui
    PRIVATE
        src/main_clap.cpp

        src/audio_plugin.cpp
        src/audio_plugin.hpp
        src/gain_plugin.cpp
        src/gain_plugin.hpp
        src/gui.cpp
        src/imgui_base.cpp
        # src/imgui_mac.mm
        src/imgui_win32.cpp

        ${CMAKE_BINARY_DIR}/bindings/imgui_impl_glfw.h
        ${CMAKE_BINARY_DIR}/bindings/imgui_impl_glfw.cpp

        ${CMAKE_BINARY_DIR}/bindings/imgui_impl_opengl3.h
        ${CMAKE_BINARY_DIR}/bindings/imgui_impl_opengl3.cpp
)

target_include_directories(plugin-imgui PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(plugin-imgui PRIVATE clap-core imgui::imgui GLEW::glew_s glfw::glfw)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_options(plugin-imgui PRIVATE /EXPORT:clap_entry)
    set_target_properties(plugin-imgui PROPERTIES SUFFIX ".clap" PREFIX "")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_options(plugin-imgui
        PRIVATE
            -exported_symbols_list ${CMAKE_SOURCE_DIR}/resources/mac/symbols.txt
    )

    set_target_properties(plugin-imgui
        PROPERTIES
            BUNDLE True
            BUNDLE_EXTENSION clap
            MACOSX_BUNDLE_GUI_IDENTIFIER com.tobanteAudio.clap-imgui
            MACOSX_BUNDLE_BUNDLE_NAME clap-imgui
            MACOSX_BUNDLE_BUNDLE_VERSION "1"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "1"
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/resources/mac/plugins.plist.in
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(plugin-imgui PRIVATE src/imgui_linux.cpp)
    target_link_libraries(plugin-imgui PRIVATE -Wl,--version-script=${CMAKE_SOURCE_DIR}/resources/linux/plug.version)
    target_link_libraries(plugin-imgui PRIVATE -Wl,-z,defs)
    set_target_properties(plugin-imgui PROPERTIES SUFFIX ".clap" PREFIX "")
endif()
