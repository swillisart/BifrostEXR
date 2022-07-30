#-
# ===========================================================================
# Copyright 2022 Autodesk, Inc. All rights reserved.
#
# Use of this software is subject to the terms of the Autodesk license
# agreement provided at the time of installation or download, or which
# otherwise accompanies this software in either electronic or hard copy form.
# ===========================================================================
#+

cmake_minimum_required(VERSION 3.17)

include(CMakePrintHelpers)

if( NOT BIFROST_LOCATION )
    message( FATAL_ERROR "Bifrost: BIFROST_LOCATION cmake variable must be defined.")
endif()

# Project Name
project(
    ExrImage
    VERSION 0.2.0
    LANGUAGES CXX
)

set( CMAKE_VERBOSE_MAKEFILE TRUE)


if( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}" CACHE PATH   "install path" FORCE)
endif()

set(CMAKE_MODULE_PATH "${BIFROST_LOCATION}/sdk/cmake;${CMAKE_MODULE_PATH}")
find_package(Bifrost REQUIRED)

include(${BIFROST_LOCATION}/sdk/cmake/utils.cmake)

add_subdirectory(src)
