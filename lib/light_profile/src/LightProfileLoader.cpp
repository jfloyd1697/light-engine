#include "light/LightProfileLoader.h"
#include "light/MiniJson.h"
#include <fstream>
#include <sstream>

namespace light {
namespace {

Rgb parseColor(const JsonValue& v) {
    if (!v.isArray() || v.size() != 3) return Rgb(0,0,0);
    return Rgb(static_cast<uint8_t>(v.at(0).asInt()), static_cast<uint8_t>(v.at(1).asInt()), static_cast<uint8_t>(v.at(2).asInt()));
}

LightStepDefinition parseStep(const JsonValue& v) {
    LightStepDefinition s;
    if (v.has("kind")) s.kind = v.at("kind").asString();
    if (v.has("target")) s.target = v.at("target").asString();
    if (v.has("color")) s.color = v.at("color").asString();
    if (v.has("intensity")) s.intensity = v.at("intensity").asFloat(1.0f);
    if (v.has("duration_ms")) s.durationMs = v.at("duration_ms").asUInt();
    if (v.has("min_intensity")) s.minIntensity = v.at("min_intensity").asFloat();
    if (v.has("max_intensity")) s.maxIntensity = v.at("max_intensity").asFloat(1.0f);
    if (v.has("period_ms")) s.periodMs = v.at("period_ms").asUInt(1000);
    if (v.has("waveform")) s.waveform = v.at("waveform").asString();
    if (v.has("update_ms")) s.updateMs = v.at("update_ms").asUInt(50);
    if (v.has("segment_length")) s.segmentLength = v.at("segment_length").asInt(1);
    if (v.has("step_ms")) s.stepMs = v.at("step_ms").asUInt(100);
    if (v.has("count")) s.repeatCount = v.at("count").asInt();
    if (v.has("gap_ms")) s.gapMs = v.at("gap_ms").asUInt();
    if (v.has("steps") && v.at("steps").isArray()) {
        for (size_t i=0;i<v.at("steps").size();++i) s.steps.push_back(parseStep(v.at("steps").at(i)));
    }
    if (v.has("item")) {
        LightStepDefinition item = parseStep(v.at("item"));
        if (!item.steps.empty()) s.itemSteps = item.steps; else s.itemSteps.push_back(item);
    }
    return s;
}

}

bool LightProfileLoader::loadFromJsonText(const std::string& jsonText, LightProfile& outProfile, std::string& outError) {
    JsonValue root;
    if (!MiniJsonParser::parse(jsonText, root, outError)) return false;
    if (!root.isObject()) { outError = "Root must be an object"; return false; }
    outProfile = LightProfile{};
    if (root.has("layout")) outProfile.layoutName = root.at("layout").asString();
    if (root.has("palette") && root.at("palette").isObject()) {
        for (std::map<std::string, JsonValue>::const_iterator it = root.at("palette").objectValue.begin(); it != root.at("palette").objectValue.end(); ++it) {
            outProfile.palette[it->first] = parseColor(it->second);
        }
    }
    if (root.has("animations") && root.at("animations").isObject()) {
        for (std::map<std::string, JsonValue>::const_iterator it = root.at("animations").objectValue.begin(); it != root.at("animations").objectValue.end(); ++it) {
            AnimationDefinition a; a.name = it->first; a.root = parseStep(it->second); outProfile.animations[a.name] = a;
        }
    }
    if (root.has("events") && root.at("events").isObject()) {
        for (std::map<std::string, JsonValue>::const_iterator it = root.at("events").objectValue.begin(); it != root.at("events").objectValue.end(); ++it) {
            LightEventDefinition e;
            const JsonValue& ev = it->second;
            if (ev.has("actions") && ev.at("actions").isArray()) {
                for (size_t i=0;i<ev.at("actions").size();++i) {
                    const JsonValue& a = ev.at("actions").at(i);
                    LightAction act;
                    if (a.has("op")) act.op = a.at("op").asString();
                    if (a.has("name")) act.name = a.at("name").asString();
                    if (a.has("layer")) act.layer = a.at("layer").asString();
                    if (a.has("blend")) act.blend = a.at("blend").asString("replace");
                    if (a.has("loop")) act.loop = a.at("loop").asBool(false);
                    e.actions.push_back(act);
                }
            }
            outProfile.events[it->first] = e;
        }
    }
    return true;
}

bool LightProfileLoader::loadFromFile(const std::string& path, LightProfile& outProfile, std::string& outError) {
    std::ifstream in(path.c_str(), std::ios::in | std::ios::binary);
    if (!in) { outError = "Failed to open file: " + path; return false; }
    std::ostringstream ss; ss << in.rdbuf();
    return loadFromJsonText(ss.str(), outProfile, outError);
}

}
