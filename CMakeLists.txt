cmake_minimum_required(VERSION 3.2)

message(STATUS " CMakeLists: NodeGraph")

option(BUILD_TESTS "Build Tests" ON)

# Global Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_DEBUG_POSTFIX "-debug")
set(CMAKE_RELWITHDEBINFO_POSTFIX "-reldbg")

project(NodeGraph
    LANGUAGES CXX C
    VERSION 0.5.0
)

find_package(MUtils REQUIRED)

# Used Packages
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(${MUTILS_INCLUDE_DIR}/cmake/utils.cmake)

set(NODEGRAPH_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(M3RDPARTY_DIR ${CMAKE_CURRENT_LIST_DIR}/m3rdparty)
set(NODEGRAPH_INCLUDE_DIR include/)

configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/config_app.h.cmake ${CMAKE_BINARY_DIR}/config_app.h)

include(cmake/all.cmake)
include(src/CMakeLists.txt)
include(app/CMakeLists.txt)
include(tests/CMakeLists.txt)

# Make the CMake bits that ensure find_package does the right thing
install(EXPORT nodegraph-targets
    FILE nodegraph-targets.cmake
    NAMESPACE NodeGraph::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nodegraph
)

configure_package_config_file(
    ${NODEGRAPH_ROOT}/cmake/nodegraph-config.cmake.in
    ${CMAKE_BINARY_DIR}/cmake/nodegraph-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nodegraph
)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/cmake/nodegraph-config-version.cmake
    VERSION 1.0
    COMPATIBILITY AnyNewerVersion
)

install(
    FILES
        ${CMAKE_BINARY_DIR}/cmake/nodegraph-config.cmake
        ${CMAKE_BINARY_DIR}/cmake/nodegraph-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nodegraph
)

