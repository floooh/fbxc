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
    DumpMaterials(fbxManager, fbxScene, root);
    DumpHierarchy(fbxManager, fbxScene, nullptr, root);
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

} // namespace FBXC