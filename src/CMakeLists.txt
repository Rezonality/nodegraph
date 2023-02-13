project(NodeGraph
    LANGUAGES CXX C
    VERSION 0.5.0
)

find_package(glm CONFIG REQUIRED)

set(NODEGRAPH_SOURCE
    ${NODEGRAPH_ROOT}/src/canvas.cpp
    ${NODEGRAPH_ROOT}/src/canvas_imgui.cpp

    ${NODEGRAPH_ROOT}/include/nodegraph/canvas.h
    ${NODEGRAPH_ROOT}/include/nodegraph/canvas_imgui.h
    ${NODEGRAPH_ROOT}/include/nodegraph/imgui_glm.h
    ${NODEGRAPH_ROOT}/include/nodegraph/math_utils.h
    ${NODEGRAPH_ROOT}/include/nodegraph/style.h
)

set(NODEGRAPH_MODEL
    #${NODEGRAPH_ROOT}/src/model/graph.cpp
    #${NODEGRAPH_ROOT}/src/model/node.cpp
    #${NODEGRAPH_ROOT}/src/model/pin.cpp
    #${NODEGRAPH_ROOT}/src/model/parameter.cpp

    #${NODEGRAPH_ROOT}/include/nodegraph/model/graph.h
    #${NODEGRAPH_ROOT}/include/nodegraph/model/node.h
    #${NODEGRAPH_ROOT}/include/nodegraph/model/pin.h
    #${NODEGRAPH_ROOT}/include/nodegraph/model/parameter.h
)

set(NODEGRAPH_VIEW
    #${NODEGRAPH_ROOT}/src/view/viewnode.cpp
    #${NODEGRAPH_ROOT}/src/view/graphview.cpp
    #${NODEGRAPH_ROOT}/src/view/node_layout.cpp
    #${NODEGRAPH_ROOT}/src/view/canvas_imgui.cpp
    #${NODEGRAPH_ROOT}/src/view/canvas_vg.cpp

    #${NODEGRAPH_ROOT}/include/nodegraph/view/canvas_imgui.h
    #${NODEGRAPH_ROOT}/include/nodegraph/view/canvas_vg.h
    #${NODEGRAPH_ROOT}/include/nodegraph/view/viewnode.h
    #${NODEGRAPH_ROOT}/include/nodegraph/view/graphview.h
    #${NODEGRAPH_ROOT}/include/nodegraph/view/layout.h
    #${NODEGRAPH_ROOT}/include/nodegraph/view/layout_control.h
    #${NODEGRAPH_ROOT}/include/nodegraph/view/node_layout.h
)

set(NODEGRAPH_SOURCE
    ${NODEGRAPH_SOURCE}
    ${NODEGRAPH_ROOT}/CMakeLists.txt
)

list(APPEND TEST_SOURCES
#    ${NODEGRAPH_DIR}/src/model/nodegraph.test.cpp
)

add_library(NodeGraph STATIC ${NODEGRAPH_SOURCE})
add_library(NodeGraph::NodeGraph ALIAS NodeGraph)

target_link_libraries(NodeGraph
PUBLIC
    glm::glm
)

target_include_directories(NodeGraph
PRIVATE
PUBLIC
    $<BUILD_INTERFACE:${NODEGRAPH_ROOT}/include>
    $<INSTALL_INTERFACE:include>
)

# Set locations for components
set_target_properties(NodeGraph PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Install the binary
install(TARGETS NodeGraph
    EXPORT nodegraph-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${LIBLEGACY_INCLUDE_DIRS}
)

# Install the include files
install(DIRECTORY ${NODEGRAPH_ROOT}/include/nodegraph
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

source_group ("model" FILES ${NODEGRAPH_MODEL})
source_group ("view" FILES ${NODEGRAPH_VIEW})

