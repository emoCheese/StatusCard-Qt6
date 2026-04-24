cmake_minimum_required(VERSION 3.16)

project(StatusCardKit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_library(StatusCardKit SHARED
    statuscardkit_export.h
    statusmapper.h
    statusmapper.cpp
    statuscard.h
    statuscard.cpp
    statuscardgrid.h
    statuscardgrid.cpp
    statuscardqss.h
    StatusCardKitConfig.cmake.in
)

target_link_libraries(StatusCardKit
    PUBLIC
        Qt6::Core
        Qt6::Widgets
)

target_include_directories(StatusCardKit PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(StatusCardKit PRIVATE STATUSCARDKIT_LIBRARY)

include(GNUInstallDirs)

install(TARGETS StatusCardKit
    EXPORT StatusCardKitTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES
    statuscardkit_export.h
    statusmapper.h
    statuscard.h
    statuscardgrid.h
    statuscardqss.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/StatusCardKit
)

install(EXPORT StatusCardKitTargets
    FILE StatusCardKitTargets.cmake
    NAMESPACE StatusCardKit::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/StatusCardKit
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/StatusCardKitConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/StatusCardKitConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/StatusCardKit
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/StatusCardKitConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/StatusCardKit
)
