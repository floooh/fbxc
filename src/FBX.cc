//------------------------------------------------------------------------------
//  FBX.cc
//------------------------------------------------------------------------------
#include "FBX.h"
#include <cassert>

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
bool
FBX::Setup() {
    assert(!this->isValid);
    assert(nullptr == this->fbxManager);
    assert(nullptr == this->fbxScene);
    assert(this->error.empty());
    
    this->fbxManager = FbxManager::Create();
    if (nullptr == this->fbxManager) {
        this->error = "failed to setup FbxManager";
        return false;
    }
    
    this->isValid = true;
    return true;
}

//------------------------------------------------------------------------------
void
FBX::Discard() {
    assert(this->isValid);
    assert(nullptr != this->fbxManager);
    this->fbxManager->Destroy();
    this->fbxManager = nullptr;
    this->fbxScene = nullptr;
    this->isValid = false;
}

} // namespace FBXC