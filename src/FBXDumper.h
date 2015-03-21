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
    /// dump textures
    static void DumpTextures(FbxManager* fbxManager, FbxScene* fbxScene, cJSON* json);
    /// dump materials
    static void DumpMaterials(FbxManager* fbxManager, FbxScene* fbxScene, cJSON* json);
    /// dump node hierarcht
    static void DumpHierarchy(FbxManager* fbxManager, FbxScene* fbxScene, FbxNode* fbxNode, cJSON* json);
    /// dump connections
    static void DumpConnections(FbxManager* fbxManager, FbxScene* fbxScene, cJSON* json);
    /// dump material => texture connections
    static void DumpMaterialTextureConnections(FbxManager* fbxManager, FbxScene* fbxScnee, cJSON* json);
    /// check and dump an FbxDouble3 property connection
    static void DumpPropertyConnection(const FbxPropertyT<FbxDouble3>& prop, const FbxCriteria& crit, const char* name, cJSON* jsonNode);
};

} // namespace FBXC