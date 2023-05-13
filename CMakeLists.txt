cmake_minimum_required(VERSION 3.2)

message(STATUS " CMakeLists: NodeGraph")

option(BUILD_NODEGRAPH_TESTS "Build Tests" OFF)
option(BUILD_NODEGRAPH_APP "Build APP" ON)

# Global Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_DEBUG_POSTFIX "-debug")
set(CMAKE_RELWITHDEBINFO_POSTFIX "-reldbg")

if (WIN32)
set(VCPKG_TARGET_TRIPLET x64-windows-static-md)
endif()

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(NodeGraph
    LANGUAGES CXX C
    VERSION 0.5.0
)

include(cmake/utils.cmake)

find_package(SDL2 REQUIRED)
find_package(fmt REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(imgui REQUIRED)

set(NODEGRAPH_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(M3RDPARTY_DIR ${CMAKE_CURRENT_LIST_DIR}/m3rdparty)
set(NODEGRAPH_INCLUDE_DIR include/)

configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/config_nodegraph_app.h.cmake ${CMAKE_BINARY_DIR}/config_nodegraph_app.h)

include(GNUInstallDirs)

include(cmake/all.cmake)

# Library
add_subdirectory(src)

# Support library and app
if (BUILD_NODEGRAPH_APP)
SET(ZING_DIR ${CMAKE_CURRENT_LIST_DIR}/libs/zing)
SET(ZING_LIBRARY_ONLY 1)
add_subdirectory(${ZING_DIR})
add_subdirectory(app)
endif()

# Tests
if (BUILD_NODEGRAPH_TESTS)
enable_testing()
add_subdirectory(tests)
endif()

# Make the CMake bits that ensure find_package does the right thing
install(EXPORT nodegraph-targets
    FILE nodegraph-targets.cmake
    NAMESPACE NodeGraph::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nodegraph
)

include(CMakePackageConfigHelpers)

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

