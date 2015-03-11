#
# Helper function for finding the FBX SDK.
#
# sets: FBXSDK_FOUND, 
#       FBXSDK_DIR, 
#       FBXSDK_LIBRARY, 
#       FBXSDK_LIBRARY_DEBUG
#       FBXSDK_INCLUDE_DIR
#
set(_fbxsdk_version "2015.1")
message("Looking for FBX SDK version: ${_fbxsdk_version}")

if (FIPS_HOST_OSX)
    set(_fbxsdk_approot "/Applications/Autodesk/FBX SDK")
    set(_fbxsdk_libdir_debug "lib/clang/debug")
    set(_fbxsdk_libdir_release "lib/clang/release")
    set(_fbxsdk_libname_debug "libfbxsdk.a")
    set(_fbxsdk_libname_release "libfbxsdk.a")
elseif (FIPS_HOST_WINDOWS)
    message(FATAL_ERROR "FIXME: find FBX SDK on Windows")
elseif (FIPS_HOST_LINUX)
    message(FATAL_ERROR" FIXME: find FBX SDK on Linux")
endif()

# should point the the FBX SDK installation dir
set(_fbxsdk_root "${_fbxsdk_approot}/${_fbxsdk_version}")

# find header dir and libs
find_path(FBXSDK_INCLUDE_DIR "fbxsdk.h" 
          PATHS ${_fbxsdk_root} 
          PATH_SUFFIXES "include")
message("FBXSDK_INCLUDE_DIR: ${FBXSDK_INCLUDE_DIR}")
find_library(FBXSDK_LIBRARY ${_fbxsdk_libname_release}
             PATHS ${_fbxsdk_root}
             PATH_SUFFIXES ${_fbxsdk_libdir_release})
message("FBXSDK_LIBRARY: ${FBXSDK_LIBRARY}")
find_library(FBXSDK_LIBRARY_DEBUG ${_fbxsdk_libname_debug}
             PATHS ${_fbxsdk_root}
             PATH_SUFFIXES ${_fbxsdk_libdir_debug})
message("FBXSDK_LIBRARY_DEBUG: ${FBXSDK_LIBRARY_DEBUG}")

if (FBXSDK_INCLUDE_DIR AND FBXSDK_LIBRARY AND FBXSDK_LIBRARY_DEBUG)
    set(FBXSDK_FOUND YES)
else()
    set(FBXSDK_FOUND NO)
endif()

