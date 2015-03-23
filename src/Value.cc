//------------------------------------------------------------------------------
//  Value.cc
//------------------------------------------------------------------------------
#include "Value.h"
#include <cassert>
#include <fbxsdk.h>

namespace FBXC {

//------------------------------------------------------------------------------
Value::Value() :
    type(Void) {
    // empty
}

//------------------------------------------------------------------------------
template<> void
Value::Set(bool val) {
    this->type = Bool;
    this->boolValue = val;
}

//------------------------------------------------------------------------------
template<> void
Value::Set(std::int32_t val) {
    this->type = Int;
    this->intValue = val;
}

//------------------------------------------------------------------------------
template<> void
Value::Set(std::uint64_t val) {
    this->type = Id;
    this->idValue = val;
}

//------------------------------------------------------------------------------
template<> void
Value::Set(double val) {
    this->type = Float;
    this->floatValues[0] = val;
}

//------------------------------------------------------------------------------
template<> void
Value::Set(FbxDouble2 val) {
    this->type = Float2;
    for (int i = 0; i < 2; i++) {
        this->floatValues[i] = val.mData[i];
    }
}

//------------------------------------------------------------------------------
template<> void
Value::Set(FbxDouble3 val) {
    this->type = Float3;
    for (int i = 0; i < 3; i++) {
        this->floatValues[i] = val.mData[i];
    }
}

//------------------------------------------------------------------------------
template<> void
Value::Set(FbxDouble4 val) {
    this->type = Float4;
    for (int i = 0; i < 4; i++) {
        this->floatValues[i] = val.mData[i];
    }
}

//------------------------------------------------------------------------------
template<> void
Value::Set(const char* str) {
    assert(str);
    this->type = String;
    this->strValue = str;
}

//------------------------------------------------------------------------------
template<> void
Value::Set(FbxString str) {
    this->type = String;
    this->strValue = str.Buffer();
}

//------------------------------------------------------------------------------
template<> void
Value::Set(std::string str) {
    this->type = String;
    this->strValue = str;
}

//------------------------------------------------------------------------------
template<> bool
Value::Get() const {
    assert(Bool == this->type);
    return this->boolValue;
}

//------------------------------------------------------------------------------
template<> std::uint64_t
Value::Get() const {
    assert(Id == this->type);
    return this->idValue;
}

//------------------------------------------------------------------------------
template<> std::int32_t
Value::Get() const {
    assert(Int == this->type);
    return this->intValue;
}

//------------------------------------------------------------------------------
template<> double
Value::Get() const {
    assert(Float == this->type);
    return this->floatValues[0];
}

//------------------------------------------------------------------------------
template<> FbxDouble2
Value::Get() const {
    assert(Float2 == this->type);
    return FbxDouble2(this->floatValues[0], this->floatValues[1]);
}

//------------------------------------------------------------------------------
template<> FbxDouble3
Value::Get() const {
    assert(Float3 == this->type);
    return FbxDouble3(this->floatValues[0], this->floatValues[1], this->floatValues[2]);
}

//------------------------------------------------------------------------------
template<> FbxDouble4
Value::Get() const {
    assert(Float4 == this->type);
    return FbxDouble4(this->floatValues[0],
                      this->floatValues[1],
                      this->floatValues[2],
                      this->floatValues[3]);
}

} // namespace FBXC
