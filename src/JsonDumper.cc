//------------------------------------------------------------------------------
//  JsonDumper.cc
//------------------------------------------------------------------------------
#include "JsonDumper.h"
#include <cstdlib>

namespace FBXC {

//------------------------------------------------------------------------------
std::string
JsonDumper::Dump(const ProxyScene& scene) {
    cJSON* jsonRoot = cJSON_CreateObject();
    
    DumpProperties(scene.Properties, jsonRoot);
    DumpTextures(scene, jsonRoot);
    DumpMaterials(scene, jsonRoot);
    DumpMeshes(scene, jsonRoot);
    
    char* rawStr = cJSON_Print(jsonRoot);
    std::string jsonStr(rawStr);
    std::free(rawStr);
    cJSON_Delete(jsonRoot);
    return jsonStr;
}

//------------------------------------------------------------------------------
void
JsonDumper::DumpProperties(const PropertyMap& props, cJSON* jsonNode) {
    for (const auto& kvp : props.Content()) {
        const std::string& key = kvp.first;
        const Value& value = kvp.second;
        switch (value.type) {
            case Value::Bool:
                cJSON_AddItemToObject(jsonNode, key.c_str(), cJSON_CreateBool(value.Get<bool>()));
                break;
            case Value::Id:
                cJSON_AddItemToObject(jsonNode, key.c_str(), cJSON_CreateNumber(value.Get<std::uint64_t>()));
                break;
            case Value::Int:
                cJSON_AddItemToObject(jsonNode, key.c_str(), cJSON_CreateNumber(value.Get<std::int32_t>()));
                break;
            case Value::Float:
                cJSON_AddItemToObject(jsonNode, key.c_str(), cJSON_CreateNumber(value.Get<double>()));
                break;
            case Value::Float2:
                {
                    FbxDouble2 v = value.Get<FbxDouble2>();
                    cJSON_AddItemToObject(jsonNode, key.c_str(), cJSON_CreateDoubleArray(v.mData, 2));
                }
                break;
            case Value::Float3:
                {
                    FbxDouble3 v = value.Get<FbxDouble3>();
                    cJSON_AddItemToObject(jsonNode, key.c_str(), cJSON_CreateDoubleArray(v.mData, 3));
                }
                break;
            case Value::Float4:
                {
                    FbxDouble4 v = value.Get<FbxDouble4>();
                    cJSON_AddItemToObject(jsonNode, key.c_str(), cJSON_CreateDoubleArray(v.mData, 4));
                }
                break;
            case Value::String:
                cJSON_AddItemToObject(jsonNode, key.c_str(), cJSON_CreateString(value.strValue.c_str()));
                break;
                
            case Value::Array:
                {
                    cJSON* jsonArray = cJSON_CreateArray();
                    cJSON_AddItemToObject(jsonNode, key.c_str(), jsonArray);
                    for (const auto& arrayVal : value.arrayValue) {
                        switch (arrayVal.type) {
                            case Value::Bool:
                                cJSON_AddItemToArray(jsonArray, cJSON_CreateBool(arrayVal.Get<bool>()));
                                break;
                            case Value::Id:
                                cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(arrayVal.Get<std::uint64_t>()));
                                break;
                            case Value::Int:
                                cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(arrayVal.Get<std::int32_t>()));
                                break;
                            case Value::Float:
                                cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(arrayVal.Get<double>()));
                                break;
                            case Value::Float2:
                                {
                                    FbxDouble2 v = arrayVal.Get<FbxDouble2>();
                                    cJSON_AddItemToArray(jsonArray, cJSON_CreateDoubleArray(v.mData, 2));
                                }
                                break;
                            case Value::Float3:
                                {
                                    FbxDouble3 v = arrayVal.Get<FbxDouble3>();
                                    cJSON_AddItemToArray(jsonArray, cJSON_CreateDoubleArray(v.mData, 3));
                                }
                                break;
                            case Value::Float4:
                                {
                                    FbxDouble4 v = arrayVal.Get<FbxDouble4>();
                                    cJSON_AddItemToArray(jsonArray, cJSON_CreateDoubleArray(v.mData, 4));
                                }
                                break;
                            case Value::String:
                                cJSON_AddItemToArray(jsonArray, cJSON_CreateString(arrayVal.strValue.c_str()));
                                break;
                            default:
                                // nested array type not supported
                                break;
                        }
                    }
                }
                break;
                
            default:
                // void type, do nothing
                break;
        }
    }
}

//------------------------------------------------------------------------------
void
JsonDumper::DumpUserProperties(const ProxyObject& obj, cJSON* jsonNode) {
    if (obj.UserProperties.Content().size() > 0) {
        cJSON* jsonUserProps = cJSON_CreateObject();
        cJSON_AddItemToObject(jsonNode, "userproperties", jsonUserProps);
        DumpProperties(obj.UserProperties, jsonUserProps);
    }
}

//------------------------------------------------------------------------------
void
JsonDumper::DumpTextures(const ProxyScene& scene, cJSON* jsonNode) {
    cJSON* jsonTextures = cJSON_CreateArray();
    cJSON_AddItemToObject(jsonNode, "textures", jsonTextures);
    for (const auto& tex : scene.Textures) {
        cJSON* jsonTex = cJSON_CreateObject();
        cJSON_AddItemToArray(jsonTextures, jsonTex);
        DumpProperties(tex.Properties, jsonTex);
        DumpUserProperties(tex, jsonTex);
    }
}

//------------------------------------------------------------------------------
void
JsonDumper::DumpMaterials(const ProxyScene& scene, cJSON* jsonNode) {
    cJSON* jsonMaterials = cJSON_CreateArray();
    cJSON_AddItemToObject(jsonNode, "materials", jsonMaterials);
    for (const auto& mat : scene.Materials) {
        cJSON* jsonMat = cJSON_CreateObject();
        cJSON_AddItemToArray(jsonMaterials, jsonMat);
        DumpProperties(mat.Properties, jsonMat);
        DumpUserProperties(mat, jsonMat);
    }
}

//------------------------------------------------------------------------------
void
JsonDumper::DumpMeshes(const ProxyScene& scene, cJSON* jsonNode) {
    cJSON* jsonMeshes = cJSON_CreateArray();
    cJSON_AddItemToObject(jsonNode, "meshes", jsonMeshes);
    for (const auto& mesh : scene.Meshes) {
        cJSON* jsonMesh = cJSON_CreateObject();
        cJSON_AddItemToArray(jsonMeshes, jsonMesh);
        DumpProperties(mesh.Properties, jsonMesh);
        DumpUserProperties(mesh, jsonMesh);
    }
}

} // namespace FBXC
