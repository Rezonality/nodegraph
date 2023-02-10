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

if (WIN32)
set(VCPKG_TARGET_TRIPLET x64-windows-static-md)
endif()

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(NodeGraph
    LANGUAGES CXX C
    VERSION 0.5.0
)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_CURRENT_LIST_DIR}/../vcpkg/packages/mutils_${VCPKG_TARGET_TRIPLET}/lib/cmake/mutils)

find_package(SDL2 REQUIRED)
find_package(gsl-lite REQUIRED)
find_package(fmt REQUIRED)
find_package(gl3w CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(unofficial-concurrentqueue REQUIRED)
find_package(tinyfiledialogs REQUIRED)
find_package(nanovg REQUIRED)
find_package(imgui REQUIRED)
find_package(MUtils REQUIRED)

include(${MUTILS_INCLUDE_DIR}/cmake/utils.cmake)

set(NODEGRAPH_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(M3RDPARTY_DIR ${CMAKE_CURRENT_LIST_DIR}/m3rdparty)
set(NODEGRAPH_INCLUDE_DIR include/)

configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/config_app.h.cmake ${CMAKE_BINARY_DIR}/config_app.h)

include(GNUInstallDirs)

enable_testing()

include(cmake/all.cmake)
add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(tests)

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

