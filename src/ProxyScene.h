#pragma once
//------------------------------------------------------------------------------
/**
    @class FBXC::ProxyScene
    @brief proxy object for an FbxScene
*/
#include "ProxyNode.h"
#include <vector>

namespace FBXC {

class ProxyScene : public ProxyObject {
public:
    std::vector<ProxyObject> Textures;
    std::vector<ProxyObject> Materials;
    std::vector<ProxyObject> Meshes;
    
    ProxyNode Nodes;
};

} // namespace FBXC