#pragma once
//------------------------------------------------------------------------------
/**
    @class ProxyNode
    @brief proxy for an FBX scene hierarchy node
*/
#include "ProxyObject.h"
#include <vector>

namespace FBXC {

class ProxyNode : public ProxyObject {
public:
    std::vector<ProxyNode> Children;
};

} // namespace FBXC