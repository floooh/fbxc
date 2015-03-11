fips_begin_app(fbxc cmdline)
    fips_files(Main.cc Main.h)
    fips_libs(${FBXSDK_LIBRARY})
fips_end_app()
