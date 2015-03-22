//------------------------------------------------------------------------------
//  FBXDumper.cc
//------------------------------------------------------------------------------
#include "FBXDumper.h"
#include "Log.h"
#include "cJSON.h"
#include <cstdlib>
#include <map>

namespace FBXC {

//------------------------------------------------------------------------------
void
FBXDumper::Dump(FbxManager* fbxManager, FbxScene* fbxScene, const std::string& fbxPath) {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "file", cJSON_CreateString(fbxPath.c_str()));
    DumpMetaData(fbxScene, root);
    DumpTextures(fbxScene, root);
    DumpMaterials(fbxScene, root);
    DumpMeshes(fbxScene, root);
    DumpNodes(fbxScene, nullptr, root);
    char* str = cJSON_Print(root);
    Log::Info(str);
    Log::Info("\n\n");
    std::free(str);
    cJSON_Delete(root);
}

//------------------------------------------------------------------------------
void
FBXDumper::DumpUserProperties(FbxObject* fbxObject, cJSON* jsonNode) {
    assert(fbxObject && jsonNode);
    
    // count user properties
    int numUserProps = 0;
    FbxProperty prop = fbxObject->GetFirstProperty();
    while (prop.IsValid()) {
        if (prop.GetFlag(FbxPropertyFlags::eUserDefined)) {
            numUserProps++;
        }
        prop = fbxObject->GetNextProperty(prop);
    }
    
    if (numUserProps > 0) {
        cJSON* jsonProperties = cJSON_CreateObject();
        cJSON_AddItemToObject(jsonNode, "userproperties", jsonProperties);    
        prop = fbxObject->GetFirstProperty();
        while (prop.IsValid()) {
            if (prop.GetFlag(FbxPropertyFlags::eUserDefined)) {
                switch (prop.GetPropertyDataType().GetType()) {
                    case EFbxType::eFbxChar:
                    case EFbxType::eFbxUChar:
                    case EFbxType::eFbxShort:
                    case EFbxType::eFbxUShort:
                    case EFbxType::eFbxInt:
                    case EFbxType::eFbxUInt:
                    case EFbxType::eFbxEnum:
                        cJSON_AddItemToObject(jsonProperties, prop.GetName(), cJSON_CreateNumber(prop.Get<int>()));
                        break;
                    case EFbxType::eFbxBool:
                        cJSON_AddItemToObject(jsonProperties, prop.GetName(), cJSON_CreateBool(prop.Get<bool>()));
                        break;
                    case EFbxType::eFbxHalfFloat:
                    case EFbxType::eFbxFloat:
                    case EFbxType::eFbxDouble:
                        cJSON_AddItemToObject(jsonProperties, prop.GetName(), cJSON_CreateNumber(prop.Get<double>()));
                        break;
                    case EFbxType::eFbxDouble2:
                        cJSON_AddItemToObject(jsonProperties, prop.GetName(), cJSON_CreateDoubleArray(prop.Get<FbxDouble2>().mData, 2));
                        break;
                    case EFbxType::eFbxDouble3:
                        cJSON_AddItemToObject(jsonProperties, prop.GetName(), cJSON_CreateDoubleArray(prop.Get<FbxDouble3>().mData, 3));
                        break;
                    case EFbxType::eFbxDouble4:
                        cJSON_AddItemToObject(jsonProperties, prop.GetName(), cJSON_CreateDoubleArray(prop.Get<FbxDouble4>().mData, 4));
                        break;
                    case EFbxType::eFbxString:
                        cJSON_AddItemToObject(jsonProperties, prop.GetName(), cJSON_CreateString(prop.Get<FbxString>().Buffer()));
                        break;
                    default:
                        cJSON_AddItemToObject(jsonProperties, prop.GetName(), cJSON_CreateString("invalid"));
                        break;
                }
            }
            prop = fbxObject->GetNextProperty(prop);
        }
    }
}

//------------------------------------------------------------------------------
void
FBXDumper::DumpMetaData(FbxScene* fbxScene, cJSON* root) {
    assert(fbxScene && root);
    
    FbxDocumentInfo* info = fbxScene->GetSceneInfo();
    if (!info) {
        Log::Fatal("failed to get scene info from FBX scene\n");
    }
    
    cJSON* meta = cJSON_CreateObject();
    cJSON_AddItemToObject(meta, "title", cJSON_CreateString(info->mTitle.Buffer()));
    cJSON_AddItemToObject(meta, "subject", cJSON_CreateString(info->mSubject.Buffer()));
    cJSON_AddItemToObject(meta, "author", cJSON_CreateString(info->mAuthor.Buffer()));
    cJSON_AddItemToObject(meta, "keyword", cJSON_CreateString(info->mKeywords.Buffer()));
    cJSON_AddItemToObject(meta, "revision", cJSON_CreateString(info->mRevision.Buffer()));
    cJSON_AddItemToObject(meta, "comment", cJSON_CreateString(info->mComment.Buffer()));
    
    cJSON_AddItemToObject(root, "meta-data", meta);
}

//------------------------------------------------------------------------------
void
FBXDumper::DumpTextures(FbxScene* fbxScene, cJSON* jsonNode) {
    assert(fbxScene && jsonNode);
    
    // add textures json node
    cJSON* jsonTexArray = cJSON_CreateArray();
    cJSON_AddItemToObject(jsonNode, "textures", jsonTexArray);
    
    // loop over textures
    const int numTextures = fbxScene->GetTextureCount();
    for (int texIndex = 0; texIndex < numTextures; texIndex++) {
        FbxTexture* fbxTex = fbxScene->GetTexture(texIndex);
        
        cJSON* jsonTexNode = cJSON_CreateObject();
        cJSON_AddItemToArray(jsonTexArray, jsonTexNode);
        
        cJSON_AddItemToObject(jsonTexNode, "name", cJSON_CreateString(fbxTex->GetName()));
        cJSON_AddItemToObject(jsonTexNode, "id", cJSON_CreateNumber(fbxTex->GetUniqueID()));
        
        const char* type = "invalid";
        if (fbxTex->GetClassId().Is(FbxFileTexture::ClassId)) {
            FbxFileTexture* fbxFileTex = (FbxFileTexture*) fbxTex;
            type = "file";
            cJSON_AddItemToObject(jsonTexNode, "usematerial", cJSON_CreateBool(fbxFileTex->UseMaterial.Get()));
            cJSON_AddItemToObject(jsonTexNode, "usemipmap", cJSON_CreateBool(fbxFileTex->UseMipMap.Get()));
            cJSON_AddItemToObject(jsonTexNode, "filename", cJSON_CreateString(fbxFileTex->GetRelativeFileName()));
            static std::map<FbxFileTexture::EMaterialUse, const char*> matUseToStr = {
                { FbxFileTexture::eModelMaterial, "model" },
                { FbxFileTexture::eDefaultMaterial, "default" }
            };
            const FbxFileTexture::EMaterialUse matUse = fbxFileTex->GetMaterialUse();
            const char* matUseStr = "invalid";
            if (matUseToStr.find(matUse) != matUseToStr.end()) {
                matUseStr = matUseToStr[matUse];
            }
            cJSON_AddItemToObject(jsonTexNode, "materialuse", cJSON_CreateString(matUseStr));
            
        }
        else if (fbxTex->GetClassId().Is(FbxProceduralTexture::ClassId)) {
            type = "procedural";
        }
        cJSON_AddItemToObject(jsonTexNode, "type", cJSON_CreateString(type));
        cJSON_AddItemToObject(jsonTexNode, "swapuv", cJSON_CreateBool(fbxTex->GetSwapUV()));
        cJSON_AddItemToObject(jsonTexNode, "premultiplyalpha", cJSON_CreateBool(fbxTex->GetPremultiplyAlpha()));
        static std::map<FbxTexture::EAlphaSource, const char*> alphaSourceToStr = {
            { FbxTexture::eNone, "none" },
            { FbxTexture::eRGBIntensity, "rgbintensity" },
            { FbxTexture::eBlack, "black" }
        };
        const FbxTexture::EAlphaSource alphaSource = fbxTex->GetAlphaSource();
        const char* alphaSourceStr = "invalid";
        if (alphaSourceToStr.find(alphaSource) != alphaSourceToStr.end()) {
            alphaSourceStr = alphaSourceToStr[alphaSource];
        }
        cJSON_AddItemToObject(jsonTexNode, "alphasource", cJSON_CreateString(alphaSourceStr));
        int cropping[4] = {
            fbxTex->GetCroppingLeft(),
            fbxTex->GetCroppingTop(),
            fbxTex->GetCroppingRight(),
            fbxTex->GetCroppingBottom()
        };
        cJSON_AddItemToObject(jsonTexNode, "cropping", cJSON_CreateIntArray(cropping, 4));
        static std::map<FbxTexture::EMappingType, const char*> mappingTypeToStr = {
            { FbxTexture::eNull, "null" },
            { FbxTexture::ePlanar, "planar" },
            { FbxTexture::eSpherical, "spherical" },
            { FbxTexture::eCylindrical, "cylindrical" },
            { FbxTexture::eBox, "box" },
            { FbxTexture::eFace, "face" },
            { FbxTexture::eUV, "uv" },
            { FbxTexture::eEnvironment, "environment" }
        };
        const FbxTexture::EMappingType mappingType = fbxTex->GetMappingType();
        const char* mappingTypeStr = "invalid";
        if (mappingTypeToStr.find(mappingType) != mappingTypeToStr.end()) {
            mappingTypeStr = mappingTypeToStr[mappingType];
        }
        cJSON_AddItemToObject(jsonTexNode, "mappingtype", cJSON_CreateString(mappingTypeStr));
        static std::map<FbxTexture::EPlanarMappingNormal, const char*> pmnToStr = {
            { FbxTexture::ePlanarNormalX, "x" },
            { FbxTexture::ePlanarNormalY, "y" },
            { FbxTexture::ePlanarNormalZ, "z" }
        };
        const FbxTexture::EPlanarMappingNormal pmn = fbxTex->GetPlanarMappingNormal();
        const char* pmnStr = "invalid";
        if (pmnToStr.find(pmn) != pmnToStr.end()) {
            pmnStr = pmnToStr[pmn];
        }
        cJSON_AddItemToObject(jsonTexNode, "planarmappingnormal", cJSON_CreateString(pmnStr));
        static std::map<FbxTexture::ETextureUse, const char*> texUseToStr = {
            { FbxTexture::eStandard, "standard" },
            { FbxTexture::eShadowMap, "shadowmap" },
            { FbxTexture::eLightMap, "lightmap" },
            { FbxTexture::eSphericalReflectionMap, "sphericalreflectionmap" },
            { FbxTexture::eSphereReflectionMap, "spherereflectionmap" },
            { FbxTexture::eBumpNormalMap, "bumpnormalmap" }
        };
        const FbxTexture::ETextureUse texUse = fbxTex->GetTextureUse();
        const char* texUseStr = "invalid";
        if (texUseToStr.find(texUse) != texUseToStr.end()) {
            texUseStr = texUseToStr[texUse];
        }
        cJSON_AddItemToObject(jsonTexNode, "textureuse", cJSON_CreateString(texUseStr));
        static std::map<FbxTexture::EWrapMode, const char*> wrapModeToStr = {
            { FbxTexture::eRepeat, "repeat" },
            { FbxTexture::eClamp, "clamp" }
        };
        const FbxTexture::EWrapMode wrapModeU = fbxTex->GetWrapModeU();
        const FbxTexture::EWrapMode wrapModeV = fbxTex->GetWrapModeV();
        const char* wrapModeUStr = "invalid";
        const char* wrapModeVStr = "invalid";
        if (wrapModeToStr.find(wrapModeU) != wrapModeToStr.end()) {
            wrapModeUStr = wrapModeToStr[wrapModeU];
        }
        if (wrapModeToStr.find(wrapModeV) != wrapModeToStr.end()) {
            wrapModeVStr = wrapModeToStr[wrapModeV];
        }
        cJSON_AddItemToObject(jsonTexNode, "wrapmodeu", cJSON_CreateString(wrapModeUStr));
        cJSON_AddItemToObject(jsonTexNode, "wrapmodev", cJSON_CreateString(wrapModeVStr));
        static std::map<FbxTexture::EBlendMode, const char*> blendModeToStr = {
            { FbxTexture::eTranslucent, "translucent" },
            { FbxTexture::eAdditive, "additive" },
            { FbxTexture::eModulate, "modulate" },
            { FbxTexture::eModulate2, "modulate2" },
            { FbxTexture::eOver, "over" }
        };
        const FbxTexture::EBlendMode blendMode = fbxTex->GetBlendMode();
        const char* blendModeStr = "invalid";
        if (blendModeToStr.find(blendMode) != blendModeToStr.end()) {
            blendModeStr = blendModeToStr[blendMode];
        };
        cJSON_AddItemToObject(jsonTexNode, "blendmode", cJSON_CreateString(blendModeStr));
        
        cJSON_AddItemToObject(jsonTexNode, "alpha", cJSON_CreateNumber(fbxTex->Alpha.Get()));
        cJSON_AddItemToObject(jsonTexNode, "translation", cJSON_CreateDoubleArray(fbxTex->Translation.Get().mData, 3));
        cJSON_AddItemToObject(jsonTexNode, "rotation", cJSON_CreateDoubleArray(fbxTex->Rotation.Get().mData, 3));
        cJSON_AddItemToObject(jsonTexNode, "scaling", cJSON_CreateDoubleArray(fbxTex->Scaling.Get().mData, 3));
        cJSON_AddItemToObject(jsonTexNode, "rotationpivot", cJSON_CreateDoubleArray(fbxTex->RotationPivot.Get().mData, 3));
        cJSON_AddItemToObject(jsonTexNode, "scalingpivot", cJSON_CreateDoubleArray(fbxTex->ScalingPivot.Get().mData, 3));
        cJSON_AddItemToObject(jsonTexNode, "uvset", cJSON_CreateString(fbxTex->UVSet.Get().Buffer()));
        
        DumpUserProperties(fbxTex, jsonTexNode);
    }
}

//------------------------------------------------------------------------------
bool
FBXDumper::DumpPropertyConnection(const FbxPropertyT<FbxDouble3>& prop, const FbxCriteria& criteria, const char* name, cJSON* jsonNode) {
    FbxObject* srcObj = prop.GetSrcObject(criteria);
    if (srcObj) {
        cJSON_AddItemToObject(jsonNode, name, cJSON_CreateString(srcObj->GetName()));
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
FBXDumper::DumpMaterials(FbxScene* fbxScene, cJSON* jsonNode) {
    assert(fbxScene && jsonNode);
    
    // add materials json root node
    cJSON* jsonMatArray = cJSON_CreateArray();
    cJSON_AddItemToObject(jsonNode, "materials", jsonMatArray);
    
    // property connection search criteria for connected textures
    FbxCriteria texCriteria = FbxCriteria::ObjectType(FbxTexture::ClassId);
    
    // loop over materials
    const int numMaterials = fbxScene->GetMaterialCount();
    for (int matIndex = 0; matIndex < numMaterials; matIndex++) {
        FbxSurfaceMaterial* fbxMat = fbxScene->GetMaterial(matIndex);

        cJSON* jsonMatNode = cJSON_CreateObject();
        cJSON_AddItemToArray(jsonMatArray, jsonMatNode);
        
        cJSON* jsonMatTexNode = cJSON_CreateObject();
        
        cJSON_AddItemToObject(jsonMatNode, "name", cJSON_CreateString(fbxMat->GetName()));
        cJSON_AddItemToObject(jsonMatNode, "id", cJSON_CreateNumber(fbxMat->GetUniqueID()));
        cJSON_AddItemToObject(jsonMatNode, "shadingmodel", cJSON_CreateString(fbxMat->ShadingModel.Get().Lower().Buffer()));
        cJSON_AddItemToObject(jsonMatNode, "multilayer", cJSON_CreateBool(fbxMat->MultiLayer.Get()));
        
        // hw shader material?
        const FbxImplementation* impl = GetImplementation(fbxMat, FBXSDK_IMPLEMENTATION_HLSL);
        const char* hwShaderType = nullptr;
        if (impl) {
            hwShaderType = "HLSL";
        }
        else {
            impl = GetImplementation(fbxMat, FBXSDK_IMPLEMENTATION_CGFX);
            if (impl) {
                hwShaderType = "CGFX";
            }
        }
        if (impl) {
            cJSON_AddItemToObject(jsonMatNode, "hwshadertype", cJSON_CreateString(hwShaderType));
            // FIXME: add iterate over shader attributes
        }
        else {
            if (fbxMat->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
                const FbxSurfaceLambert* lamb = (FbxSurfaceLambert*) fbxMat;
                
                cJSON_AddItemToObject(jsonMatNode, "emissive", cJSON_CreateDoubleArray(lamb->Emissive.Get().mData, 3));
                cJSON_AddItemToObject(jsonMatNode, "emissivefactor", cJSON_CreateNumber(lamb->EmissiveFactor.Get()));
                cJSON_AddItemToObject(jsonMatNode, "ambient", cJSON_CreateDoubleArray(lamb->Ambient.Get().mData, 3));
                cJSON_AddItemToObject(jsonMatNode, "ambientfactor", cJSON_CreateNumber(lamb->AmbientFactor.Get()));
                cJSON_AddItemToObject(jsonMatNode, "diffuse", cJSON_CreateDoubleArray(lamb->Diffuse.Get().mData, 3));
                cJSON_AddItemToObject(jsonMatNode, "diffusefactor", cJSON_CreateNumber(lamb->DiffuseFactor.Get()));
                cJSON_AddItemToObject(jsonMatNode, "normalmap", cJSON_CreateDoubleArray(lamb->NormalMap.Get().mData, 3));
                cJSON_AddItemToObject(jsonMatNode, "bump", cJSON_CreateDoubleArray(lamb->Bump.Get().mData, 3));
                cJSON_AddItemToObject(jsonMatNode, "bumpfactor", cJSON_CreateNumber(lamb->BumpFactor.Get()));
                cJSON_AddItemToObject(jsonMatNode, "transparentcolor", cJSON_CreateDoubleArray(lamb->TransparentColor.Get().mData, 3));
                cJSON_AddItemToObject(jsonMatNode, "transparencyfactor", cJSON_CreateNumber(lamb->TransparencyFactor.Get()));
                cJSON_AddItemToObject(jsonMatNode, "displacementcolor", cJSON_CreateDoubleArray(lamb->DisplacementColor.Get().mData, 3));
                cJSON_AddItemToObject(jsonMatNode, "displacementfactor", cJSON_CreateNumber(lamb->DisplacementFactor.Get()));
                cJSON_AddItemToObject(jsonMatNode, "vectordisplacementcolor", cJSON_CreateDoubleArray(lamb->VectorDisplacementColor.Get().mData, 3));
                cJSON_AddItemToObject(jsonMatNode, "vectordisplacementfactor", cJSON_CreateNumber(lamb->VectorDisplacementFactor.Get()));
                
                // texture connections
                DumpPropertyConnection(lamb->Emissive, texCriteria, "emissive", jsonMatTexNode);
                DumpPropertyConnection(lamb->Ambient, texCriteria, "ambient", jsonMatTexNode);
                DumpPropertyConnection(lamb->Diffuse, texCriteria, "diffuse", jsonMatTexNode);
                DumpPropertyConnection(lamb->NormalMap, texCriteria, "normalmap", jsonMatTexNode);
                DumpPropertyConnection(lamb->Bump, texCriteria, "bump", jsonMatTexNode);
                DumpPropertyConnection(lamb->TransparentColor, texCriteria, "transparentcolor", jsonMatTexNode);
                DumpPropertyConnection(lamb->DisplacementColor, texCriteria, "displacementcolor", jsonMatTexNode);
                DumpPropertyConnection(lamb->VectorDisplacementColor, texCriteria, "vectordisplacementcolor", jsonMatTexNode);
                
            }
            if (fbxMat->GetClassId().Is(FbxSurfacePhong::ClassId)) {
                const FbxSurfacePhong* phong = (FbxSurfacePhong*) fbxMat;
                cJSON_AddItemToObject(jsonMatNode, "specular", cJSON_CreateDoubleArray(phong->Specular.Get().mData, 3));
                cJSON_AddItemToObject(jsonMatNode, "specularfactor", cJSON_CreateNumber(phong->SpecularFactor.Get()));
                cJSON_AddItemToObject(jsonMatNode, "shininess", cJSON_CreateNumber(phong->Shininess.Get()));
                cJSON_AddItemToObject(jsonMatNode, "reflection", cJSON_CreateDoubleArray(phong->Reflection.Get().mData, 3));
                cJSON_AddItemToObject(jsonMatNode, "reflectionfactor", cJSON_CreateNumber(phong->ReflectionFactor.Get()));
                
                // texture connections
                DumpPropertyConnection(phong->Specular, texCriteria, "specular", jsonMatTexNode);
                DumpPropertyConnection(phong->Shininess, texCriteria, "shininess", jsonMatTexNode);
                DumpPropertyConnection(phong->Reflection, texCriteria, "reflection", jsonMatTexNode);
            }
            
            cJSON_AddItemToObject(jsonMatNode, "textures", jsonMatTexNode);
        }
        DumpUserProperties(fbxMat, jsonMatNode);
    }
}

//------------------------------------------------------------------------------
void
FBXDumper::DumpMeshes(FbxScene* fbxScene, cJSON* jsonNode) {
    assert(fbxScene && jsonNode);
    
    // add geometries json node
    cJSON* jsonMeshArray = cJSON_CreateArray();
    cJSON_AddItemToObject(jsonNode, "meshes", jsonMeshArray);
    
    const int numGeoms = fbxScene->GetGeometryCount();
    for (int geomIndex = 0; geomIndex < numGeoms; geomIndex++) {
        FbxGeometry* fbxGeom = fbxScene->GetGeometry(geomIndex);
        
        // only look at meshes
        if (fbxGeom->GetClassId().Is(FbxMesh::ClassId)) {
            FbxMesh* fbxMesh = (FbxMesh*) fbxGeom;

            cJSON* jsonMeshNode = cJSON_CreateObject();
            cJSON_AddItemToArray(jsonMeshArray, jsonMeshNode);
            
            // NOTE: meshes don't have names, so use unique id as identifier
            cJSON_AddItemToObject(jsonMeshNode, "id", cJSON_CreateNumber(fbxMesh->GetUniqueID()));
            cJSON_AddItemToObject(jsonMeshNode, "numpoints", cJSON_CreateNumber(fbxMesh->GetControlPointsCount()));
            cJSON_AddItemToObject(jsonMeshNode, "numpolygons", cJSON_CreateNumber(fbxMesh->GetPolygonCount()));
            
            cJSON* jsonLayerArray = cJSON_CreateArray();
            cJSON_AddItemToObject(jsonMeshNode, "layers", jsonLayerArray);
            const int numLayers = fbxMesh->GetLayerCount();
            for (int layerIndex = 0; layerIndex < numLayers; layerIndex++) {
                FbxLayer* fbxLayer = fbxMesh->GetLayer(layerIndex);
            
                cJSON* jsonLayer = cJSON_CreateObject();
                cJSON_AddItemToArray(jsonLayerArray, jsonLayer);
                
                cJSON_AddItemToObject(jsonLayer, "hasnormals", cJSON_CreateBool(fbxLayer->GetNormals() != nullptr));
                cJSON_AddItemToObject(jsonLayer, "hastangents", cJSON_CreateBool(fbxLayer->GetTangents() != nullptr));
                cJSON_AddItemToObject(jsonLayer, "hasbinormals", cJSON_CreateBool(fbxLayer->GetBinormals() != nullptr));
                cJSON_AddItemToObject(jsonLayer, "hasmaterials", cJSON_CreateBool(fbxLayer->GetMaterials() != nullptr));
                cJSON_AddItemToObject(jsonLayer, "haspolygongroups", cJSON_CreateBool(fbxLayer->GetPolygonGroups() != nullptr));
                cJSON_AddItemToObject(jsonLayer, "hasvertexcolor", cJSON_CreateBool(fbxLayer->GetVertexColors() != nullptr));
                cJSON_AddItemToObject(jsonLayer, "hasuserdata", cJSON_CreateBool(fbxLayer->GetUserData() != nullptr));
                cJSON_AddItemToObject(jsonLayer, "hasvisibility", cJSON_CreateBool(fbxLayer->GetVisibility() != nullptr));
                
                if (fbxLayer->GetUVSetCount() > 0) {
                    cJSON* jsonUvSets = cJSON_CreateArray();
                    cJSON_AddItemToObject(jsonLayer, "uvsets", jsonUvSets);
                    FbxArray<const FbxLayerElementUV*> uvSets = fbxLayer->GetUVSets();
                    for (int i = 0; i < uvSets.Size(); i++) {
                        cJSON_AddItemToArray(jsonUvSets, cJSON_CreateString(uvSets[i]->GetName()));
                    }
                }
            }
            DumpUserProperties(fbxMesh, jsonMeshNode);
        }
    }
}

//------------------------------------------------------------------------------
std::vector<FbxUInt64>
FBXDumper::GetNodeAttributeUniqueIds(FbxNode* fbxNode, FbxNodeAttribute::EType type) {
    std::vector<FbxUInt64> result;
    const int numNodeAttrs = fbxNode->GetNodeAttributeCount();
    for (int i = 0; i < numNodeAttrs; i++) {
        FbxNodeAttribute* fbxNodeAttr = fbxNode->GetNodeAttributeByIndex(i);
        if (fbxNodeAttr->GetAttributeType() == type) {
            result.push_back(fbxNode->GetUniqueID());
        }
    }
    return result;
}

//------------------------------------------------------------------------------
void
FBXDumper::DumpNodes(FbxScene* fbxScene, FbxNode* fbxNode, cJSON* jsonNode) {
    assert(fbxScene && jsonNode);
    
    // add hierarchy json root node
    if (nullptr == fbxNode) {
        cJSON* hierarchy = cJSON_CreateObject();
        cJSON_AddItemToObject(jsonNode, "nodes", hierarchy);
        jsonNode = hierarchy;
        fbxNode = fbxScene->GetRootNode();
    }
    assert(fbxNode);

    // display node attributes
    cJSON_AddItemToObject(jsonNode, "name", cJSON_CreateString(fbxNode->GetName()));
    cJSON_AddItemToObject(jsonNode, "id", cJSON_CreateNumber(fbxNode->GetUniqueID()));
    cJSON_AddItemToObject(jsonNode, "visible", cJSON_CreateBool(fbxNode->GetVisibility()));
    
    // meshes connected to this node
    std::vector<FbxUInt64> meshes = GetNodeAttributeUniqueIds(fbxNode, FbxNodeAttribute::eMesh);
    if (meshes.size() > 0) {
        cJSON* jsonMeshes = cJSON_CreateArray();
        cJSON_AddItemToObject(jsonNode, "meshes", jsonMeshes);
        for (auto id : meshes) {
            cJSON_AddItemToArray(jsonMeshes, cJSON_CreateNumber(id));
        }
    }
    DumpUserProperties(fbxNode, jsonNode);
    
    // recurse into children
    if (fbxNode->GetChildCount() > 0) {
        cJSON* jsonChildNodes = cJSON_CreateArray();
        cJSON_AddItemToObject(jsonNode, "nodes", jsonChildNodes);
        for (int i = 0; i < fbxNode->GetChildCount(); i++) {
            FbxNode* fbxChildNode = fbxNode->GetChild(i);
            cJSON* jsonChild = cJSON_CreateObject();
            cJSON_AddItemToArray(jsonChildNodes, jsonChild);
            
            // recurse into children
            DumpNodes(fbxScene, fbxChildNode, jsonChild);
        }
    }
}

} // namespace FBXC