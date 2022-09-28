cmake_minimum_required(VERSION 3.21)
project(plugin-minimal)


add_library(plugin-minimal MODULE main.cpp)
target_link_libraries(plugin-minimal PRIVATE clap-core)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_options(plugin-minimal PRIVATE /EXPORT:clap_entry)
    set_target_properties(plugin-minimal PROPERTIES SUFFIX ".clap" PREFIX "")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_options(plugin-minimal
        PRIVATE
            -exported_symbols_list ${CMAKE_SOURCE_DIR}/resources/mac/symbols.txt
    )

    set_target_properties(plugin-minimal
        PROPERTIES
            BUNDLE True
            BUNDLE_EXTENSION clap
            MACOSX_BUNDLE_GUI_IDENTIFIER com.my_company.plugin-minimal
            MACOSX_BUNDLE_BUNDLE_NAME plugin-minimal
            MACOSX_BUNDLE_BUNDLE_VERSION "1"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "1"
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/resources/mac/plugins.plist.in
    )
endif()