#pragma once
//------------------------------------------------------------------------------
/**
    @class FBXC::FBXDumper
    @brief helper class to dump scene structure in JSON format to stdout
*/
#include <fbxsdk.h>
#include <string>
#include <vector>
#include "cJSON.h"

namespace FBXC {

class FBXDumper {
public:
    /// main method, dump everything to stdout
    static void Dump(FbxManager* fbxManager, FbxScene* fbxScene, const std::string& fbxPath);
private:
    /// get the unique-ids of node attributes by type
    static std::vector<FbxUInt64> GetNodeAttributeUniqueIds(FbxNode* fbxNode, FbxNodeAttribute::EType type);
    /// dump file meta
    static void DumpMetaData(FbxScene* fbxScene, cJSON* json);
    /// dump textures
    static void DumpTextures(FbxScene* fbxScene, cJSON* json);
    /// check and dump an FbxDouble3 property connection, return true if connection exists
    static bool DumpPropertyConnection(const FbxPropertyT<FbxDouble3>& prop, const FbxCriteria& crit, const char* name, cJSON* jsonNode);
    /// dump materials
    static void DumpMaterials(FbxScene* fbxScene, cJSON* json);
    /// dump geometries
    static void DumpMeshes(FbxScene* fbxScene, cJSON* json);
    /// dump node hierarchy
    static void DumpNodes(FbxScene* fbxScene, FbxNode* fbxNode, cJSON* json);
    /// dump user properties of an FbxObject
    static void DumpUserProperties(FbxObject* fbxObject, cJSON* json);
    
};

} // namespace FBXC