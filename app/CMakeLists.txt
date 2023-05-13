project(NodeGraph_App VERSION 0.1.0.0)

set(APP_NAME NodeGraph_App)

# Set additional project information
set(COMPANY "NodeGraph")
set(COPYRIGHT "Copyright (c) 2019 Chris Maughan. All rights reserved.")
set(IDENTIFIER "com.nodegraph")

set(NODEGRAPH_APP_ROOT ${CMAKE_CURRENT_LIST_DIR})

find_package(portaudio CONFIG REQUIRED)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/nodegraph_app_common.cmake)

set(NODEGRAPH_APP_SOURCE
    ${NODEGRAPH_APP_ROOT}/main.cpp
    ${NODEGRAPH_APP_ROOT}/demo.cpp
    ${NODEGRAPH_APP_ROOT}/CMakeLists.txt
    ${NODEGRAPH_APP_ROOT}/nodes/node_oscillator.cpp
    ${NODEGRAPH_APP_ROOT}/nodes/node_oscillator.h
    )

set(RESOURCE_FOLDER ${CMAKE_CURRENT_LIST_DIR}/res)
add_project_meta(META_FILES_TO_INCLUDE)

add_executable (${APP_NAME}
    ${OS_BUNDLE}
    ${NODEGRAPH_APP_SOURCE}
    ${META_FILES_TO_INCLUDE}
    )

target_include_directories(${APP_NAME}
    PRIVATE
    #${ZING_DIR}/libs
    #${ZING_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_BINARY_DIR}
    )

target_link_libraries (${APP_NAME}
    PRIVATE
    NodeGraph::NodeGraph
    imgui::imgui
    SDL2::SDL2main
    Zing::Zing
    ${PLATFORM_LINKLIBS}
    )

target_compile_definitions(${APP_NAME}
    PUBLIC
    NO_LIBSNDFILE
    _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)

if (WIN32)
    copy_existing_files(${PROJECT_NAME} "${RESOURCE_DEPLOY_FILES}" ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )
endif()

if (APPLE)
    install(TARGETS ${DEMO_NAME}
        EXPORT nodegraph-targets
        COMPONENT binaries
        BUNDLE DESTINATION . COMPONENT Runtime
        RUNTIME DESTINATION bin COMPONENT Runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${LIBLEGACY_INCLUDE_DIRS})
else()
    install(TARGETS ${DEMO_NAME}
        EXPORT nodegraph-targets
        COMPONENT binaries
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/imgui
        INCLUDES DESTINATION ${LIBLEGACY_INCLUDE_DIRS}
        )
endif()

source_group ("Source" FILES ${NODEGRAPH_APP_SOURCE})

