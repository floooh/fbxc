#pragma once
//------------------------------------------------------------------------------
/**
    @class FBXC::PropertyMap
    @brief a dictionary of key/value properties
*/
#include <cassert>
#include <map>
#include "Value.h"

namespace FBXC {

class PropertyMap {
public:
    /// add a value to the property map
    template<typename TYPE> void Add(const std::string& key, TYPE value);
    /// return true if property map contains key
    bool Contains(const std::string& key) const;
    /// return value by key (must be contained)
    const Value& operator[](const std::string& key) const;
    /// get content map
    const std::map<std::string, Value>& Content() const;

private:
    std::map<std::string, Value> content;
};

//------------------------------------------------------------------------------
template<typename TYPE> void
PropertyMap::Add(const std::string& key, TYPE value) {
    assert(!this->Contains(key));
    Value val;
    val.Set(value);
    this->content[key] = val;
}

} // namespace FBXC

