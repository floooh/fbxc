#pragma once
//------------------------------------------------------------------------------
/**
    @class FBXC::FBXDumper
    @brief helper class to dump scene structure in JSON format to stdout
*/
#include <fbxsdk.h>
#include <string>
#include "cJSON.h"

namespace FBXC {

class FBXDumper {
public:
    /// main method, dump everything to stdout
    static void Dump(FbxManager* fbxManager, FbxScene* fbxScene, const std::string& fbxPath);
private:
    /// dump file meta
    static void DumpMetaData(FbxManager* fbxManager, FbxScene* fbxScene, cJSON* json);
    /// dump materials
    static void DumpMaterials(FbxManager* fbxManager, FbxScene* fbxScene, cJSON* json);
    /// dump node hierarcht
    static void DumpHierarchy(FbxManager* fbxManager, FbxScene* fbxScene, FbxNode* fbxNode, cJSON* jsonNode);
};

} // namespace FBXC