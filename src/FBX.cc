//------------------------------------------------------------------------------
//  FBX.cc
//------------------------------------------------------------------------------
#include "FBX.h"
#include "Log.h"
#include "FBXDumper.h"

namespace FBXC {

//------------------------------------------------------------------------------
FBX::FBX() {
    // empty
}

//------------------------------------------------------------------------------
FBX::~FBX() {
    if (this->isValid) {
        this->Discard();
    }
}

//------------------------------------------------------------------------------
void
FBX::Setup() {
    assert(!this->isValid);
    assert(nullptr == this->fbxManager);
    assert(nullptr == this->fbxIoSettings);
    assert(nullptr == this->fbxScene);
    
    this->fbxManager = FbxManager::Create();
    if (nullptr == this->fbxManager) {
        Log::Fatal("failed to  FbxManager\n");
    }
    this->fbxIoSettings = FbxIOSettings::Create(this->fbxManager, IOSROOT);
    if (nullptr == this->fbxIoSettings) {
        Log::Fatal("failed to create FbxIOSettings object\n");
    }
    this->fbxManager->SetIOSettings(this->fbxIoSettings);
    this->fbxScene = FbxScene::Create(this->fbxManager, "scene");
    if (nullptr == this->fbxScene) {
        Log::Fatal("failed to create FbxScene object\n");
    }
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
FBX::Discard() {
    assert(this->isValid);
    assert(nullptr != this->fbxManager);
    this->fbxManager->Destroy();
    this->fbxManager = nullptr;
    this->fbxIoSettings = nullptr;
    this->fbxScene = nullptr;
    this->isValid = false;
}

//------------------------------------------------------------------------------
void
FBX::Load(const std::string& fbxPath) {
    assert(nullptr != this->fbxManager);
    assert(nullptr != this->fbxScene);
    this->filePath = fbxPath;
    
    // setup the importer
    FbxImporter* fbxImporter = FbxImporter::Create(this->fbxManager, "importer");
    bool initResult = fbxImporter->Initialize(fbxPath.c_str(), -1, this->fbxIoSettings);
    if (!initResult) {
        FbxString error = fbxImporter->GetStatus().GetErrorString();
        Log::Fatal("FbxImporter setup failed with '%s'\n", fbxPath.c_str(), error.Buffer());
    }
    
    // import the file, NOTE: we could tweak import settings via fbxIoSettings here
    bool importResult = fbxImporter->Import(this->fbxScene);
    if (!importResult) {
        FbxString error = fbxImporter->GetStatus().GetErrorString();
        Log::Fatal("importing failed with '%s'\n", fbxPath.c_str(), error.Buffer());
    }
    fbxImporter->Destroy();
}

//------------------------------------------------------------------------------
void
FBX::Dump() {
    assert(nullptr != this->fbxManager);
    assert(nullptr != this->fbxScene);
    FBXDumper::Dump(this->fbxManager, this->fbxScene, this->filePath);
}

} // namespace FBXC

