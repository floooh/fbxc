#pragma once
//------------------------------------------------------------------------------
/**
    @class FBXC::JsonDumper
    @brief dump a ProxyScene to JSON
*/
#include "ProxyScene.h"
#include "cJSON.h"

namespace FBXC {

class JsonDumper {
public:
    /// dump ProxyScene to string
    static std::string Dump(const ProxyScene& scene);
    
private:
    /// dump a property key/values to json object
    static void DumpProperties(const PropertyMap& props, cJSON* jsonNode);
    /// dump user properties of an object
    static void DumpUserProperties(const ProxyObject& obj, cJSON* jsonNode);
    /// dump textures in scene
    static void DumpTextures(const ProxyScene& scene, cJSON* jsonNode);
    /// dump materials in scene
    static void DumpMaterials(const ProxyScene& scene, cJSON* jsonNode);
    /// dump meshes in scene
    static void DumpMeshes(const ProxyScene& scene, cJSON* jsonNode);
};

} // namespace FBXC