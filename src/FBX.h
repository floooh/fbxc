#pragma once
//------------------------------------------------------------------------------
/**
    @class FBXC::FBX
    @brief access to global FBX functionality
*/
#include <fbxsdk.h>
#include <string>
#include "ProxyScene.h"

namespace FBXC {

class FBX {
public:
    /// constructor
    FBX();
    /// destructor
    ~FBX();
    
    /// setup the FBX SDK
    void Setup();
    /// discard everything
    void Discard();
    /// return true if object has been setup
    bool IsValid() const;
    
    /// load an FBX file
    void Load(const std::string& path);
    /// dump the FBX scene structure
    void Dump();
    
    
private:
    bool isValid;
    std::string filePath;
    FbxManager* fbxManager = nullptr;
    FbxIOSettings* fbxIoSettings = nullptr;
    FbxScene* fbxScene = nullptr;
    ProxyScene proxyScene;
};

//------------------------------------------------------------------------------
inline bool
FBX::IsValid() const {
    return this->isValid;
}

} // namespace FBXC