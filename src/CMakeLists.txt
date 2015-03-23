fips_begin_lib(fbxc_lib)
    fips_files(
        Log.h
        Main.cc Main.h
        FBX.cc FBX.h
        Value.cc Value.h
        PropertyMap.cc PropertyMap.h
        ProxyObject.h
        ProxyNode.h
        ProxyScene.h
        ProxyBuilder.cc ProxyBuilder.h
        JsonDumper.cc JsonDumper.h
    )
    fips_libs(cjson)
fips_end_lib()

