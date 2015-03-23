//------------------------------------------------------------------------------
//  PropertyMap.cc
//------------------------------------------------------------------------------
#include "PropertyMap.h"

namespace FBXC {

//------------------------------------------------------------------------------
bool
PropertyMap::Contains(const std::string& key) const {
    return this->content.find(key) != this->content.end();
}

//------------------------------------------------------------------------------
const Value&
PropertyMap::operator[](const std::string& key) const {
    return this->content.at(key);
}

//------------------------------------------------------------------------------
const std::map<std::string, Value>&
PropertyMap::Content() const {
    return this->content;
}

} // namespace FBXC