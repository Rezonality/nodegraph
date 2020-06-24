find_package(SDL2 REQUIRED)
#set_target_properties(SDL2::SDL2 PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)
#set_target_properties(MUtils::MUtils PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)

set(NODEGRAPH_MODEL
    ${NODEGRAPH_ROOT}/src/model/graph.cpp
    ${NODEGRAPH_ROOT}/src/model/node.cpp
    ${NODEGRAPH_ROOT}/src/model/pin.cpp

    ${NODEGRAPH_ROOT}/include/nodegraph/model/graph.h
    ${NODEGRAPH_ROOT}/include/nodegraph/model/node.h
    ${NODEGRAPH_ROOT}/include/nodegraph/model/pin.h
    ${NODEGRAPH_ROOT}/include/nodegraph/model/parameter.h
)

set(NODEGRAPH_VIEW
    ${NODEGRAPH_ROOT}/src/view/viewnode.cpp
    ${NODEGRAPH_ROOT}/src/view/graphview.cpp
    ${NODEGRAPH_ROOT}/src/view/canvas.cpp
    
    ${NODEGRAPH_ROOT}/include/nodegraph/view/canvas.h
    ${NODEGRAPH_ROOT}/include/nodegraph/view/viewnode.h
    ${NODEGRAPH_ROOT}/include/nodegraph/view/graphview.h
)

set(NODEGRAPH_SOURCE
    ${NODEGRAPH_ROOT}/CMakeLists.txt
    ${NODEGRAPH_MODEL}
    ${NODEGRAPH_VIEW}
)

list(APPEND TEST_SOURCES
#    ${NODEGRAPH_DIR}/src/model/nodegraph.test.cpp
)

add_library(NodeGraph STATIC ${NODEGRAPH_SOURCE})
add_library(NodeGraph::NodeGraph ALIAS NodeGraph)

target_link_libraries(NodeGraph 
PUBLIC
    MUtils::MUtils
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

