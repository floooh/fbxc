//------------------------------------------------------------------------------
//  FBXDumper.cc
//------------------------------------------------------------------------------
#include "FBXDumper.h"
#include "Log.h"
#include "cJSON.h"
#include <cstdlib>

namespace FBXC {

//------------------------------------------------------------------------------
void
FBXDumper::Dump(FbxManager* fbxManager, FbxScene* fbxScene, const std::string& fbxPath) {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "path", cJSON_CreateString(fbxPath.c_str()));
    DumpMetaData(fbxManager, fbxScene, root);
    DumpHierarchy(fbxManager, fbxScene, nullptr, root);
    char* str = cJSON_Print(root);
    Log::Info(str);
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
FBXDumper::DumpHierarchy(FbxManager* fbxManager, FbxScene* fbxScene, FbxNode* fbxNode, cJSON* json) {
    assert(fbxManager && fbxScene && json);
    
    if (nullptr == fbxNode) {
        cJSON* hierarchy = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "hierarchy", hierarchy);
        json = hierarchy;
        fbxNode = fbxScene->GetRootNode();
    }
    assert(fbxNode);

    if (fbxNode->GetChildCount() > 0) {
        cJSON* jsonChildNodes = cJSON_CreateArray();
        cJSON_AddItemToObject(json, "children", jsonChildNodes);
        for (int i = 0; i < fbxNode->GetChildCount(); i++) {
            FbxNode* fbxChildNode = fbxNode->GetChild(i);
            cJSON* jsonChild = cJSON_CreateObject();
            cJSON_AddItemToObject(jsonChild, "name", cJSON_CreateString(fbxChildNode->GetName()));
            cJSON_AddItemToArray(jsonChildNodes, jsonChild);
            
            // recurse into children
            DumpHierarchy(fbxManager, fbxScene, fbxChildNode, jsonChild);
        }
    }
}

} // namespace FBXC