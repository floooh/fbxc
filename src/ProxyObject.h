#pragma once
//------------------------------------------------------------------------------
/**
    @class FBXC::ProxyObject
    @brief simple FbxObject proxy with PropertyMap
*/
#include <cassert>
#include <fbxsdk.h>
#include "PropertyMap.h"

namespace FBXC {

class ProxyObject {
public:
    FbxObject* Object = nullptr;
    PropertyMap Properties;
    PropertyMap UserProperties;
    
    // safe-cast to specialized FBX object type
    template<typename TYPE> TYPE* As() {
        assert(this->Object && this->Object->GetClassId().Is(TYPE::ClassId));
        return (TYPE*) this->Object;
    };
};

} // namespace FBXC