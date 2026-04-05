#include "light/MiniJson.h"
#include <cctype>
#include <cstdlib>

namespace light {

bool JsonValue::has(const std::string& key) const {
    return isObject() && objectValue.find(key) != objectValue.end();
}
const JsonValue& JsonValue::at(const std::string& key) const { return objectValue.at(key); }
const JsonValue& JsonValue::at(size_t index) const { return arrayValue.at(index); }
size_t JsonValue::size() const { return isArray() ? arrayValue.size() : (isObject() ? objectValue.size() : 0); }
std::string JsonValue::asString(const std::string& fallback) const { return isString() ? stringValue : fallback; }
int JsonValue::asInt(int fallback) const { return isNumber() ? static_cast<int>(numberValue) : fallback; }
uint32_t JsonValue::asUInt(uint32_t fallback) const { return isNumber() ? static_cast<uint32_t>(numberValue) : fallback; }
float JsonValue::asFloat(float fallback) const { return isNumber() ? static_cast<float>(numberValue) : fallback; }
bool JsonValue::asBool(bool fallback) const { return isBool() ? boolValue : fallback; }

namespace {
class Parser {
public:
    explicit Parser(const std::string& s) : text(s) {}
    bool parse(JsonValue& out, std::string& err) {
        skip();
        if (!parseValue(out, err)) return false;
        skip();
        if (pos != text.size()) { err = "Unexpected trailing input"; return false; }
        return true;
    }
private:
    const std::string& text;
    size_t pos = 0;
    void skip() { while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) ++pos; }
    bool match(const char* s) {
        size_t i = 0; while (s[i]) { if (pos + i >= text.size() || text[pos+i] != s[i]) return false; ++i; }
        pos += i; return true;
    }
    bool parseValue(JsonValue& out, std::string& err) {
        skip(); if (pos >= text.size()) { err = "Unexpected end of input"; return false; }
        char c = text[pos];
        if (c == '{') return parseObject(out, err);
        if (c == '[') return parseArray(out, err);
        if (c == '"') return parseString(out, err);
        if (c == 't' || c == 'f') return parseBool(out, err);
        if (c == 'n') return parseNull(out, err);
        return parseNumber(out, err);
    }
    bool parseNull(JsonValue& out, std::string& err) { if (!match("null")) { err = "Invalid null"; return false; } out = JsonValue{}; return true; }
    bool parseBool(JsonValue& out, std::string& err) {
        if (match("true")) { out.type = JsonValue::Type::Bool; out.boolValue = true; return true; }
        if (match("false")) { out.type = JsonValue::Type::Bool; out.boolValue = false; return true; }
        err = "Invalid bool"; return false;
    }
    bool parseNumber(JsonValue& out, std::string& err) {
        size_t start = pos;
        if (text[pos] == '-') ++pos;
        while (pos < text.size() && std::isdigit(static_cast<unsigned char>(text[pos]))) ++pos;
        if (pos < text.size() && text[pos] == '.') { ++pos; while (pos < text.size() && std::isdigit(static_cast<unsigned char>(text[pos]))) ++pos; }
        std::string s = text.substr(start, pos - start);
        char* endptr = nullptr; double v = std::strtod(s.c_str(), &endptr);
        if (!endptr || *endptr != '\0') { err = "Invalid number"; return false; }
        out.type = JsonValue::Type::Number; out.numberValue = v; return true;
    }
    bool parseString(JsonValue& out, std::string& err) {
        if (text[pos] != '"') { err = "Expected string"; return false; }
        ++pos; std::string s;
        while (pos < text.size()) {
            char c = text[pos++];
            if (c == '"') { out.type = JsonValue::Type::String; out.stringValue = s; return true; }
            if (c == '\\') {
                if (pos >= text.size()) { err = "Invalid escape"; return false; }
                char e = text[pos++];
                switch (e) {
                    case '"': s.push_back('"'); break; case '\\': s.push_back('\\'); break; case '/': s.push_back('/'); break;
                    case 'b': s.push_back('\b'); break; case 'f': s.push_back('\f'); break; case 'n': s.push_back('\n'); break;
                    case 'r': s.push_back('\r'); break; case 't': s.push_back('\t'); break;
                    default: err = "Unsupported escape"; return false;
                }
            } else { s.push_back(c); }
        }
        err = "Unterminated string"; return false;
    }
    bool parseArray(JsonValue& out, std::string& err) {
        ++pos; out.type = JsonValue::Type::Array; skip();
        if (pos < text.size() && text[pos] == ']') { ++pos; return true; }
        while (true) {
            JsonValue item; if (!parseValue(item, err)) return false; out.arrayValue.push_back(item); skip();
            if (pos >= text.size()) { err = "Unterminated array"; return false; }
            if (text[pos] == ']') { ++pos; return true; }
            if (text[pos] != ',') { err = "Expected comma in array"; return false; }
            ++pos; skip();
        }
    }
    bool parseObject(JsonValue& out, std::string& err) {
        ++pos; out.type = JsonValue::Type::Object; skip();
        if (pos < text.size() && text[pos] == '}') { ++pos; return true; }
        while (true) {
            JsonValue key; if (!parseString(key, err)) return false; skip();
            if (pos >= text.size() || text[pos] != ':') { err = "Expected colon in object"; return false; }
            ++pos; skip(); JsonValue value; if (!parseValue(value, err)) return false; out.objectValue[key.stringValue] = value; skip();
            if (pos >= text.size()) { err = "Unterminated object"; return false; }
            if (text[pos] == '}') { ++pos; return true; }
            if (text[pos] != ',') { err = "Expected comma in object"; return false; }
            ++pos; skip();
        }
    }
};
}

bool MiniJsonParser::parse(const std::string& text, JsonValue& outValue, std::string& outError) {
    Parser parser(text);
    return parser.parse(outValue, outError);
}

}
