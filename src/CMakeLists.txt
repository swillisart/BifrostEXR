#-
# ===========================================================================
# Copyright 2021 Autodesk, Inc. All rights reserved.
#
# Use of this software is subject to the terms of the Autodesk license
# agreement provided at the time of installation or download, or which
# otherwise accompanies this software in either electronic or hard copy form.
# ===========================================================================
#+

# Build and install the library
add_library( ExrImageOps SHARED)
target_compile_definitions( ExrImageOps PRIVATE IMAGE_BUILD_NODEDEF_DLL )
target_sources( ExrImageOps PUBLIC ExrImage.cpp)
target_link_libraries( ExrImageOps PUBLIC Bifrost::AminoCppOpSDK )
target_link_libraries( ExrImageOps PUBLIC Bifrost::AminoCore )

install( TARGETS ExrImageOps DESTINATION lib )

# Generate and install the json files
set( headers_to_parse 
    ExrImage.h
)

bifrost_header_parser( 
    ExrImageJsonFiles  # Name of the target.
    "json"                 # Install dir - relative to install prefix.
    INCLUDES    ${CMAKE_CURRENT_SOURCE_DIR}
    INPUTS      ${headers_to_parse})

# Create config json file to load the bifrost pack
set( CONFIG_PACK_VERSION "${PROJECT_VERSION}")
set( CONFIG_PACK_NAME "${PROJECT_NAME}")
set( CONFIG_SHARED_LIBS "./lib")
set( CONFIG_JSON_FILES "./json")
set( config_json ${CMAKE_BINARY_DIR}/${PROJECT_NAME}PackConfig.json )
configure_file(PackConfig.json.in ${config_json} )
install(FILES ${config_json} DESTINATION . )
