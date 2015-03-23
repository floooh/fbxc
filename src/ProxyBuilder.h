#pragma once
//------------------------------------------------------------------------------
/**
    @class FBXC::ProxyBuilder
    @brief populates a ProxyScene object from an FbxScene object
*/
#include "ProxyScene.h"
#include <fbxsdk.h>
#include <string>

namespace FBXC {

class ProxyBuilder {
public:
    /// populate ProxyScene object from FbxScene
    static void Build(FbxScene* fbxScene, const std::string& fbxPath, ProxyScene& outProxyScene);
    
private:
    /// build a property connection (e.g. when a texture is attached to a material property)
    static bool BuildPropertyConnection(const FbxPropertyT<FbxDouble3>& prop, const FbxCriteria& criteria, const char* name, PropertyMap& props);
    /// build user properties
    static void BuildUserProperties(FbxObject* fbxObject, ProxyObject& obj);
    /// build metadata information
    static void BuildMetaData(FbxScene* fbxScene, ProxyScene& scene);
    /// build texture array
    static void BuildTextures(FbxScene* fbxScene, ProxyScene& scene);
    /// build material array
    static void BuildMaterials(FbxScene* fbxScene, ProxyScene& scene);
    /// build mesh array
    static void BuildMeshes(FbxScene* fbxScene, ProxyScene& scene);
};

} // namespace FBXC