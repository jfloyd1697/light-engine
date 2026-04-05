#pragma once
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "light/Color.h"

namespace light {

struct LightStepDefinition {
    std::string kind;
    std::string target;
    std::string color;
    float intensity = 1.0f;
    uint32_t durationMs = 0;

    float minIntensity = 0.0f;
    float maxIntensity = 1.0f;
    uint32_t periodMs = 1000;
    std::string waveform;

    uint32_t updateMs = 50;
    int segmentLength = 1;
    uint32_t stepMs = 100;

    int repeatCount = 0;
    uint32_t gapMs = 0;
    std::vector<LightStepDefinition> steps;
    std::vector<LightStepDefinition> itemSteps;
};

struct AnimationDefinition {
    std::string name;
    LightStepDefinition root;
};

struct LightAction {
    std::string op;
    std::string name;
    std::string layer;
    std::string blend = "replace";
    bool loop = false;
};

struct LightEventDefinition {
    std::vector<LightAction> actions;
};

struct LightProfile {
    std::string layoutName;
    std::unordered_map<std::string, Rgb> palette;
    std::unordered_map<std::string, AnimationDefinition> animations;
    std::unordered_map<std::string, LightEventDefinition> events;
};

}
