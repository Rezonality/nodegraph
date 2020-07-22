project(NodeGraph_App VERSION 0.1.0.0)

set(APP_NAME NodeGraph_App)

# Set additional project information
set(COMPANY "NodeGraph")
set(COPYRIGHT "Copyright (c) 2019 Chris Maughan. All rights reserved.")
set(IDENTIFIER "com.nodegraph")

set(APP_ROOT ${CMAKE_CURRENT_LIST_DIR})

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/demo_common.cmake)

set(NODEGRAPH_APP_SOURCE
    ${APP_ROOT}/main.cpp
    ${APP_ROOT}/CMakeLists.txt
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
    ${CMAKE_CURRENT_LIST_DIR} 
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

if (WIN32)
    # No longer needed?
    #target_link_libraries (${DEMO_NAME} 
    #    PRIVATE
    # ${MUTILS_LIB_DIR}/freetype2/lib/freetype$<$<CONFIG:DEBUG>:d>.lib
    #    )
    copy_existing_files(${PROJECT_NAME} "${RESOURCE_DEPLOY_FILES}" ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )
endif()

# Install the font
install(FILES ${MUTILS_INCLUDE_DIR}/imgui/misc/fonts/Cousine-Regular.ttf
    COMPONENT binaries
    DESTINATION ${CMAKE_INSTALL_BINDIR}/imgui
    )

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

