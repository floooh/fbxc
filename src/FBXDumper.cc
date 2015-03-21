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
    DumpMetaData(fbxManager, fbxScene, root);
    DumpTextures(fbxManager, fbxScene, root);
    DumpMaterials(fbxManager, fbxScene, root);
    DumpHierarchy(fbxManager, fbxScene, nullptr, root);
    DumpConnections(fbxManager, fbxScene, root);
    char* str = cJSON_Print(root);
    Log::Info(str);
    Log::Info("\n\n");
    std::free(str);
    cJSON_Delete(root);
}

//------------------------------------------------------------------------------
void
FBXDumper::DumpMetaData(FbxManager* fbxManager, FbxScene* fbxScene, cJSON* root) {
    assert(fbxManager && fbxScene && root);
    
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
FBXDumper::DumpTextures(FbxManager* fbxManager, FbxScene* fbxScene, cJSON* jsonNode) {
    assert(fbxManager && fbxScene && jsonNode);
    
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
    }
}

//------------------------------------------------------------------------------
void
FBXDumper::DumpMaterials(FbxManager* fbxManager, FbxScene* fbxScene, cJSON* jsonNode) {
    assert(fbxManager && fbxScene && jsonNode);
    
    // add materials json root node
    cJSON* jsonMatArray = cJSON_CreateArray();
    cJSON_AddItemToObject(jsonNode, "materials", jsonMatArray);
    
    // loop over materials
    const int numMaterials = fbxScene->GetMaterialCount();
    for (int matIndex = 0; matIndex < numMaterials; matIndex++) {
        FbxSurfaceMaterial* fbxMat = fbxScene->GetMaterial(matIndex);

        cJSON* jsonMatNode = cJSON_CreateObject();
        cJSON_AddItemToArray(jsonMatArray, jsonMatNode);
        
        cJSON_AddItemToObject(jsonMatNode, "name", cJSON_CreateString(fbxMat->GetName()));
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
                // FIXME: hmm I guess the color attributes can be attached to textures...?
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
            }
            if (fbxMat->GetClassId().Is(FbxSurfacePhong::ClassId)) {
                const FbxSurfacePhong* phong = (FbxSurfacePhong*) fbxMat;
                cJSON_AddItemToObject(jsonMatNode, "specular", cJSON_CreateDoubleArray(phong->Specular.Get().mData, 3));
                cJSON_AddItemToObject(jsonMatNode, "specularfactor", cJSON_CreateNumber(phong->SpecularFactor.Get()));
                cJSON_AddItemToObject(jsonMatNode, "shininess", cJSON_CreateNumber(phong->Shininess.Get()));
                cJSON_AddItemToObject(jsonMatNode, "reflection", cJSON_CreateDoubleArray(phong->Reflection.Get().mData, 3));
                cJSON_AddItemToObject(jsonMatNode, "reflectionfactor", cJSON_CreateNumber(phong->ReflectionFactor.Get()));
            }
        }
    }
}

//------------------------------------------------------------------------------
void
FBXDumper::DumpHierarchy(FbxManager* fbxManager, FbxScene* fbxScene, FbxNode* fbxNode, cJSON* jsonNode) {
    assert(fbxManager && fbxScene && jsonNode);
    
    // add hierarchy json root node
    if (nullptr == fbxNode) {
        cJSON* hierarchy = cJSON_CreateObject();
        cJSON_AddItemToObject(jsonNode, "hierarchy", hierarchy);
        jsonNode = hierarchy;
        fbxNode = fbxScene->GetRootNode();
    }
    assert(fbxNode);

    // display node attributes
    cJSON_AddItemToObject(jsonNode, "name", cJSON_CreateString(fbxNode->GetName()));
    FbxNodeAttribute* fbxNodeAttr = fbxNode->GetNodeAttribute();
    if (fbxNodeAttr) {
        static std::map<FbxNodeAttribute::EType, const char*> typeMap = {
            { FbxNodeAttribute::eUnknown, "unknown" },
            { FbxNodeAttribute::eNull, "null" },
            { FbxNodeAttribute::eMarker, "marker" },
            { FbxNodeAttribute::eSkeleton, "skeleton" },
            { FbxNodeAttribute::eMesh, "mesh" },
            { FbxNodeAttribute::eNurbs, "nurbs" },
            { FbxNodeAttribute::ePatch, "patch" },
            { FbxNodeAttribute::eCamera, "camera" },
            { FbxNodeAttribute::eCameraStereo, "camerastereo" },
            { FbxNodeAttribute::eCameraSwitcher, "cameraswitcher" },
            { FbxNodeAttribute::eLight, "light" },
            { FbxNodeAttribute::eOpticalReference, "opticalreference" },
            { FbxNodeAttribute::eOpticalMarker, "opticalmarker" },
            { FbxNodeAttribute::eNurbsCurve, "nurbscurve" },
            { FbxNodeAttribute::eTrimNurbsSurface, "trimnurbssurface" },
            { FbxNodeAttribute::eBoundary, "boundary" },
            { FbxNodeAttribute::eNurbsSurface, "nurbssurface" },
            { FbxNodeAttribute::eShape, "shape" },
            { FbxNodeAttribute::eLODGroup, "lodgroup" },
            { FbxNodeAttribute::eSubDiv, "subdiv" },
            { FbxNodeAttribute::eCachedEffect, "cachedeffect" },
            { FbxNodeAttribute::eLine, "line" }
        };
        const FbxNodeAttribute::EType fbxNodeType = fbxNodeAttr->GetAttributeType();
        if (typeMap.find(fbxNodeType) != typeMap.end()) {
            cJSON_AddItemToObject(jsonNode, "type", cJSON_CreateString(typeMap[fbxNodeType]));
        }
        else {
            cJSON_AddItemToObject(jsonNode, "type", cJSON_CreateString("invalid"));
        }
    }
    else {
        cJSON_AddItemToObject(jsonNode, "type", cJSON_CreateString("none"));
    }
    cJSON_AddItemToObject(jsonNode, "visible", cJSON_CreateBool(fbxNode->GetVisibility()));
    
    // recurse into children
    if (fbxNode->GetChildCount() > 0) {
        cJSON* jsonChildNodes = cJSON_CreateArray();
        cJSON_AddItemToObject(jsonNode, "children", jsonChildNodes);
        for (int i = 0; i < fbxNode->GetChildCount(); i++) {
            FbxNode* fbxChildNode = fbxNode->GetChild(i);
            cJSON* jsonChild = cJSON_CreateObject();
            cJSON_AddItemToArray(jsonChildNodes, jsonChild);
            
            // recurse into children
            DumpHierarchy(fbxManager, fbxScene, fbxChildNode, jsonChild);
        }
    }
}

//------------------------------------------------------------------------------
void
FBXDumper::DumpConnections(FbxManager* fbxManager, FbxScene* fbxScene, cJSON* jsonNode) {
    assert(fbxManager && fbxScene && jsonNode);
    
    cJSON* jsonConnNode = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonNode, "connections", jsonConnNode);
    
    // dump material -> texture connections
    DumpMaterialTextureConnections(fbxManager, fbxScene, jsonConnNode);
}

//------------------------------------------------------------------------------
void
FBXDumper::DumpPropertyConnection(const FbxPropertyT<FbxDouble3>& prop, const FbxCriteria& criteria, const char* name, cJSON* jsonNode) {
    FbxObject* srcObj = prop.GetSrcObject(criteria);
    if (srcObj) {
        cJSON_AddItemToObject(jsonNode, name, cJSON_CreateString(srcObj->GetName()));
    }
}

//------------------------------------------------------------------------------
void
FBXDumper::DumpMaterialTextureConnections(FbxManager* fbxManager, FbxScene* fbxScene, cJSON* jsonNode) {
    assert(fbxManager && fbxScene && jsonNode);
    
    cJSON* jsonMatTexNode = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonNode, "material_textures", jsonMatTexNode);
    
    FbxCriteria texCriteria = FbxCriteria::ObjectType(FbxTexture::ClassId);
    const int numMaterials = fbxScene->GetMaterialCount();
    for (int matIndex = 0; matIndex < numMaterials; matIndex++) {
        FbxSurfaceMaterial* fbxMat = fbxScene->GetMaterial(matIndex);
        
        cJSON* jsonMatNode = cJSON_CreateObject();
        cJSON_AddItemToObject(jsonMatTexNode, fbxMat->GetName(), jsonMatNode);
        
        if (fbxMat->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
            const FbxSurfaceLambert* lamb = (FbxSurfaceLambert*) fbxMat;
            DumpPropertyConnection(lamb->Emissive, texCriteria, "emissive", jsonMatNode);
            DumpPropertyConnection(lamb->Ambient, texCriteria, "ambient", jsonMatNode);
            DumpPropertyConnection(lamb->Diffuse, texCriteria, "diffuse", jsonMatNode);
            DumpPropertyConnection(lamb->NormalMap, texCriteria, "normalmap", jsonMatNode);
            DumpPropertyConnection(lamb->Bump, texCriteria, "bump", jsonMatNode);
            DumpPropertyConnection(lamb->TransparentColor, texCriteria, "transparentcolor", jsonMatNode);
            DumpPropertyConnection(lamb->DisplacementColor, texCriteria, "displacementcolor", jsonMatNode);
            DumpPropertyConnection(lamb->VectorDisplacementColor, texCriteria, "vectordisplacementcolor", jsonMatNode);
        }
        if (fbxMat->GetClassId().Is(FbxSurfacePhong::ClassId)) {
            const FbxSurfacePhong* phong = (FbxSurfacePhong*) fbxMat;
            DumpPropertyConnection(phong->Specular, texCriteria, "specular", jsonMatNode);
            DumpPropertyConnection(phong->Shininess, texCriteria, "shininess", jsonMatNode);
            DumpPropertyConnection(phong->Reflection, texCriteria, "reflection", jsonMatNode);
        }
    }
}

} // namespace FBXC