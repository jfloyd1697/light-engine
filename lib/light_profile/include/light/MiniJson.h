#pragma once
#include <stddef.h>
#include <stdint.h>
#include <map>
#include <string>
#include <vector>

namespace light {

class JsonValue {
public:
    enum class Type { Null, Bool, Number, String, Array, Object };

    Type type = Type::Null;
    bool boolValue = false;
    double numberValue = 0.0;
    std::string stringValue;
    std::vector<JsonValue> arrayValue;
    std::map<std::string, JsonValue> objectValue;

    bool isNull() const { return type == Type::Null; }
    bool isBool() const { return type == Type::Bool; }
    bool isNumber() const { return type == Type::Number; }
    bool isString() const { return type == Type::String; }
    bool isArray() const { return type == Type::Array; }
    bool isObject() const { return type == Type::Object; }

    bool has(const std::string& key) const;
    const JsonValue& at(const std::string& key) const;
    const JsonValue& at(size_t index) const;
    size_t size() const;

    std::string asString(const std::string& fallback = "") const;
    int asInt(int fallback = 0) const;
    uint32_t asUInt(uint32_t fallback = 0) const;
    float asFloat(float fallback = 0.0f) const;
    bool asBool(bool fallback = false) const;
};

class MiniJsonParser {
public:
    static bool parse(const std::string& text, JsonValue& outValue, std::string& outError);
};

}
