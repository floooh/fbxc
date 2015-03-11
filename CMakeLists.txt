#
# project: fbxc
#
if (NOT FIPS_IMPORT)
    cmake_minimum_required(VERSION 2.8)
    get_filename_component(FIPS_ROOT_DIR "../fips" ABSOLUTE)
    include("${FIPS_ROOT_DIR}/cmake/fips.cmake")
    fips_setup()
    fips_project(fbxc)
endif()

# find the FBX SDK
include("cmake/find_fbxsdk.cmake")
if (NOT FBXSDK_FOUND)
    message(FATAL_ERROR "FBX SDK not found!")
endif()

fips_include_directories(${FBXSDK_INCLUDE_DIR})
fips_add_subdirectory(src)

if (NOT FIPS_IMPORT)
    fips_finish()
endif()
