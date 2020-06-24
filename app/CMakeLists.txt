set(APP_NAME NodeGraph_App)

set(NODEGRAPH_APP_ROOT ${CMAKE_CURRENT_LIST_DIR}/../)

set(NODEGRAPH_APP_SOURCE
    ${NODEGRAPH_ROOT}/app/main.cpp
    ${NODEGRAPH_ROOT}/app/CMakeLists.txt
    )

if(WIN32)
    # Let windows know that the app is DPI Aware
    SET(MANIFEST_SOURCE ${NODEGRAPH_ROOT}/app/dpiawarescaling.manifest)
endif()

add_executable (${APP_NAME} WIN32
    ${NODEGRAPH_APP_SOURCE}
    ${MANIFEST_SOURCE}
    ) # Win32 ignored on non-windows

target_include_directories(${APP_NAME}
    PRIVATE
    ${NODEGRAPH_ROOT}/app
    ${CMAKE_BINARY_DIR}
    )

target_link_libraries (${APP_NAME}
    PRIVATE
    SDL2::SDL2main
    MUtils::MUtils
    NodeGraph::NodeGraph
    ${PLATFORM_LINKLIBS}
    )

add_dependencies(${APP_NAME} unittests)
source_group ("Source" FILES ${NODEGRAPH_APP_SOURCE})

