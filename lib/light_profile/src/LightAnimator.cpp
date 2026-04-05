#include "light/LightAnimator.h"
#include <algorithm>
#include <math.h>
#include <stdlib.h>

namespace light {

void LightAnimator::stopLayer(const std::string& layer) {
    m_active.erase(std::remove_if(m_active.begin(), m_active.end(), [&](const ActiveAnimation& a){ return a.layer == layer; }), m_active.end());
}

void LightAnimator::stopAll() { m_active.clear(); }

void LightAnimator::playAnimation(const std::string& name, const std::string& layer, const std::string& blend, bool loop, uint32_t nowMs) {
    ActiveAnimation a; a.animationName = name; a.layer = layer; a.blend = blend; a.loop = loop; a.startMs = nowMs; a.phaseMs = 0; m_active.push_back(a);
}

Rgb LightAnimator::resolveColor(const std::string& name) const {
    if (!m_profile) return Rgb(0,0,0);
    std::unordered_map<std::string,Rgb>::const_iterator it = m_profile->palette.find(name);
    if (it == m_profile->palette.end()) return Rgb(255,255,255);
    return it->second;
}

void LightAnimator::applyTargetFill(const std::string& target, const Rgb& color, float intensity, bool additive) {
    if (!m_engine) return;
    const int cw = m_engine->canvasWidth();
    const int ch = m_engine->canvasHeight();
    Rgb final = scale(color, intensity);
    for (int y=0;y<ch;++y) {
        for (int x=0;x<cw;++x) {
            bool on = false;
            if (target == "all" || target == "ring") on = true;
            else if (target == "left") on = x < cw / 2;
            else if (target == "right") on = x >= cw / 2;
            else if (target == "top") on = y < ch / 2;
            else if (target == "bottom") on = y >= ch / 2;
            else if (target == "even") on = ((x + y) % 2) == 0;
            else if (target == "odd") on = ((x + y) % 2) == 1;
            if (!on) continue;
            if (additive) m_engine->addCanvasPixel(x, y, final); else m_engine->setCanvasPixel(x, y, final);
        }
    }
}

void LightAnimator::applyStep(const LightStepDefinition& step, uint32_t elapsedMs) {
    const bool additive = true;
    if (step.kind == "fill") {
        applyTargetFill(step.target, resolveColor(step.color), step.intensity, additive);
        return;
    }
    if (step.kind == "off") {
        applyTargetFill(step.target, Rgb(0,0,0), 1.0f, false);
        return;
    }
    if (step.kind == "fade_out") {
        float duration = step.durationMs == 0 ? 1.0f : static_cast<float>(step.durationMs);
        float t = 1.0f - std::min(1.0f, static_cast<float>(elapsedMs) / duration);
        applyTargetFill(step.target.empty() ? "all" : step.target, Rgb(255,255,255), t * 0.1f, additive);
        return;
    }
    if (step.kind == "pulse") {
        float phase = step.periodMs == 0 ? 0.0f : static_cast<float>(elapsedMs % step.periodMs) / static_cast<float>(step.periodMs);
        float wave = 0.5f + 0.5f * sinf(phase * 2.0f * 3.1415926f);
        float intensity = step.minIntensity + (step.maxIntensity - step.minIntensity) * wave;
        applyTargetFill(step.target, resolveColor(step.color), intensity, additive);
        return;
    }
    if (step.kind == "flicker") {
        uint32_t bucket = step.updateMs == 0 ? elapsedMs : (elapsedMs / step.updateMs);
        float pseudo = fmodf(sinf(bucket * 12.9898f) * 43758.5453f, 1.0f);
        if (pseudo < 0.0f) pseudo += 1.0f;
        float intensity = step.minIntensity + (step.maxIntensity - step.minIntensity) * pseudo;
        applyTargetFill(step.target, resolveColor(step.color), intensity, additive);
        return;
    }
    if (step.kind == "chase") {
        if (!m_engine) return;
        const int cw = m_engine->canvasWidth();
        const int ch = m_engine->canvasHeight();
        int seg = step.segmentLength <= 0 ? 1 : step.segmentLength;
        int idx = step.stepMs == 0 ? 0 : static_cast<int>((elapsedMs / step.stepMs) % cw);
        Rgb c = resolveColor(step.color);
        for (int y=0;y<ch;++y) {
            for (int x=0;x<cw;++x) {
                bool on = false;
                for (int i=0;i<seg;++i) if (((idx + i) % cw) == x) on = true;
                if (on) m_engine->addCanvasPixel(x, y, c);
            }
        }
        return;
    }
    if (step.kind == "sequence") {
        uint32_t cursor = 0;
        for (size_t i=0;i<step.steps.size();++i) {
            const LightStepDefinition& child = step.steps[i];
            uint32_t d = child.durationMs == 0 ? 1 : child.durationMs;
            if (elapsedMs < cursor + d) { applyStep(child, elapsedMs - cursor); return; }
            cursor += d;
        }
        if (!step.steps.empty()) applyStep(step.steps.back(), step.steps.back().durationMs);
        return;
    }
    if (step.kind == "repeat") {
        if (step.itemSteps.empty()) return;
        uint32_t itemDuration = 0;
        for (size_t i=0;i<step.itemSteps.size();++i) itemDuration += (step.itemSteps[i].durationMs == 0 ? 1 : step.itemSteps[i].durationMs);
        itemDuration += step.gapMs;
        if (itemDuration == 0) return;
        int count = step.repeatCount <= 0 ? 1 : step.repeatCount;
        uint32_t total = itemDuration * static_cast<uint32_t>(count);
        uint32_t local = elapsedMs % total;
        uint32_t cycle = local % itemDuration;
        if (cycle >= itemDuration - step.gapMs) return;
        LightStepDefinition seq; seq.kind = "sequence"; seq.steps = step.itemSteps; applyStep(seq, cycle);
        return;
    }
}

void LightAnimator::applyAnimation(const AnimationDefinition& anim, const ActiveAnimation& active, uint32_t nowMs) {
    uint32_t elapsed = nowMs - active.startMs;
    applyStep(anim.root, elapsed);
}

void LightAnimator::update(uint32_t nowMs) {
    if (!m_profile || !m_engine) return;
    m_engine->clearCanvas();
    std::vector<ActiveAnimation> survivors;
    for (size_t i=0;i<m_active.size();++i) {
        const ActiveAnimation& active = m_active[i];
        std::unordered_map<std::string, AnimationDefinition>::const_iterator it = m_profile->animations.find(active.animationName);
        if (it == m_profile->animations.end()) continue;
        applyAnimation(it->second, active, nowMs);
        survivors.push_back(active);
    }
    m_active.swap(survivors);
    m_engine->mapCanvasToPixels();
}

}
