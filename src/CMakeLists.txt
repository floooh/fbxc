fips_begin_lib(fbxc_lib)
    fips_files(
        Main.cc Main.h
        FBX.cc FBX.h
        FBXDumper.cc FBXDumper.h
    )
    fips_libs(cjson)
fips_end_lib()
