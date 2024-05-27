project(NodeGraph
    LANGUAGES CXX C
    VERSION 0.5.0
)

find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(date CONFIG REQUIRED)

set(NODEGRAPH_VULKAN_SOURCE
    ${NODEGRAPH_ROOT}/src/vulkan/vulkan_imgui_texture.cpp
    ${NODEGRAPH_ROOT}/include/nodegraph/vulkan/vulkan_imgui_texture.h
)

set(NODEGRAPH_SOURCE
    ${NODEGRAPH_ROOT}/src/canvas.cpp
    ${NODEGRAPH_ROOT}/src/fonts.cpp
    ${NODEGRAPH_ROOT}/src/canvas_imgui.cpp
    ${NODEGRAPH_ROOT}/src/widgets/widget.cpp
    ${NODEGRAPH_ROOT}/src/widgets/node.cpp
    ${NODEGRAPH_ROOT}/src/widgets/widget_slider.cpp
    ${NODEGRAPH_ROOT}/src/widgets/widget_waveslider.cpp
    ${NODEGRAPH_ROOT}/src/widgets/widget_label.cpp
    ${NODEGRAPH_ROOT}/src/widgets/widget_knob.cpp
    ${NODEGRAPH_ROOT}/src/widgets/widget_socket.cpp
    ${NODEGRAPH_ROOT}/src/widgets/layout.cpp
    ${NODEGRAPH_ROOT}/project.natvis

    ${NODEGRAPH_ROOT}/include/nodegraph/canvas.h
    ${NODEGRAPH_ROOT}/include/nodegraph/canvas_imgui.h
    ${NODEGRAPH_ROOT}/include/nodegraph/theme.h
    
    ${NODEGRAPH_ROOT}/include/nodegraph/widgets/widget.h
    ${NODEGRAPH_ROOT}/include/nodegraph/widgets/node.h
    ${NODEGRAPH_ROOT}/include/nodegraph/widgets/widget_slider.h
    ${NODEGRAPH_ROOT}/include/nodegraph/widgets/widget_waveslider.h
    ${NODEGRAPH_ROOT}/include/nodegraph/widgets/widget_label.h
    ${NODEGRAPH_ROOT}/include/nodegraph/widgets/widget_knob.h
    ${NODEGRAPH_ROOT}/include/nodegraph/widgets/widget_socket.h
    ${NODEGRAPH_ROOT}/include/nodegraph/widgets/layout.h
)

set(NODEGRAPH_SOURCE
    ${NODEGRAPH_SOURCE}
    ${NODEGRAPH_VULKAN_SOURCE}
    ${NODEGRAPH_ROOT}/CMakeLists.txt
)

list(APPEND TEST_SOURCES
#    ${NODEGRAPH_DIR}/src/model/nodegraph.test.cpp
)

add_library(NodeGraph STATIC ${NODEGRAPH_SOURCE})
add_library(NodeGraph::NodeGraph ALIAS NodeGraph)

target_link_libraries(NodeGraph
PRIVATE
PUBLIC
    Zing::Zing
    glm::glm
    Vulkan::Vulkan
    date::date
    date::date-tz)

target_include_directories(NodeGraph
PRIVATE
    ${CMAKE_BINARY_DIR}
PUBLIC
    $<BUILD_INTERFACE:${NODEGRAPH_ROOT}/include>
    $<BUILD_INTERFACE:${NODEGRAPH_ROOT}/libs>
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

source_group ("nodegraph" FILES ${NODEGRAPH_SOURCE})
source_group ("vulkan" FILES ${NODEGRAPH_VULKAN_SOURCE})

