#pragma once
//------------------------------------------------------------------------------
/**
    @class FBXC::Value
    @brief multi-type value of a property
*/
#include <cstdint>
#include <string>
#include <vector>

namespace FBXC {

class Value {
public:
    /// types
    enum Type {
        Void,
        Bool,           // bool value
        Id,             // 64 bit unsigned integer used as unique id
        Int,            // 32 bit signed integer
        Float,          // scalar floating point value (double precision!)
        Float2,         // 2D floating point vector
        Float3,         // 3D floating point vector
        Float4,         // 4D floating point vector
        String,         // simple string
        Array,          // an array of values
    };

    /// default constructor
    Value();
    
    /// set value
    template<typename TYPE> void Set(TYPE t);
    /// get value
    template<typename TYPE> TYPE Get() const;
    
    Type type;
    std::string strValue;
    union {
        bool boolValue;
        std::int32_t intValue;
        std::uint64_t idValue;
        double floatValues[4];
    };
    std::vector<Value> arrayValue;
};

} // namespace FBXC
