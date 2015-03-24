#
# project: fbxc
#
if (NOT FIPS_IMPORT)
    cmake_minimum_required(VERSION 2.8)
    get_filename_component(FIPS_ROOT_DIR "../fips" ABSOLUTE)
    include("${FIPS_ROOT_DIR}/cmake/fips.cmake")

    # cpptoml requires exceptions enabled
    set(FIPS_EXCEPTIONS ON CACHE BOOL "Enable C++ exceptions" FORCE)
    
    fips_setup()
    fips_project(fbxc)
endif()

# find the FBX SDK
include("cmake/find_fbxsdk.cmake")
if (NOT FBXSDK_FOUND)
    message(FATAL_ERROR "FBX SDK not found!")
endif()

fips_include_directories(${FBXSDK_INCLUDE_DIR} src)
fips_add_subdirectory(src)
fips_begin_app(fbxc cmdline)
    fips_files(main_stub.cc)
    fips_deps(fbxc_lib)    
    #    fips_libs(${FBXSDK_LIBRARY})
fips_end_app()
target_link_libraries(fbxc debug ${FBXSDK_LIBRARY_DEBUG} optimized ${FBXSDK_LIBRARY})

# disable some warnings from the FBX SDK headers
if (FIPS_CLANG)
    set_target_properties(fbxc_lib PROPERTIES COMPILE_FLAGS "-Wno-missing-field-initializers")
    set_target_properties(fbxc PROPERTIES COMPILE_FLAGS "-Wno-missing-field-initializers")
endif()

if (NOT FIPS_IMPORT)
    fips_finish()
endif()
