#pragma once
//------------------------------------------------------------------------------
/**
    @class FBXC::FBX
    @brief access to global FBX functionality
*/
#include <fbxsdk.h>
#include <string>

namespace FBXC {

class FBX {
public:
    /// constructor
    FBX();
    /// destructor
    ~FBX();
    
    /// setup the FBX SDK
    bool Setup();
    /// discard everything
    void Discard();
    /// return true if object has been setup
    bool IsValid() const;
    /// get last error
    const std::string& Error() const;
    
    /// load an FBX file
    bool Load(const std::string& path);
    
private:
    bool isValid;
    std::string error;
    FbxManager* fbxManager = nullptr;
    FbxScene* fbxScene = nullptr;
};

//------------------------------------------------------------------------------
inline bool
FBX::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
inline const std::string&
FBX::Error() const {
    return this->error;
}

} // namespace FBXC