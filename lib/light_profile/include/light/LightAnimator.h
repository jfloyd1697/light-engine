#pragma once
#include <string>
#include <vector>
#include "light/LightEngine.h"
#include "light/LightProfile.h"

namespace light {

struct ActiveAnimation {
    std::string animationName;
    std::string layer;
    std::string blend = "replace";
    bool loop = false;
    uint32_t startMs = 0;
    uint32_t phaseMs = 0;
};

class LightAnimator {
public:
    void setProfile(const LightProfile* profile) { m_profile = profile; }
    void setEngine(LightEngine* engine) { m_engine = engine; }

    void stopLayer(const std::string& layer);
    void stopAll();
    void playAnimation(const std::string& name, const std::string& layer, const std::string& blend, bool loop, uint32_t nowMs);
    void update(uint32_t nowMs);

private:
    void applyAnimation(const AnimationDefinition& anim, const ActiveAnimation& active, uint32_t nowMs);
    void applyStep(const LightStepDefinition& step, uint32_t elapsedMs);
    Rgb resolveColor(const std::string& name) const;
    void applyTargetFill(const std::string& target, const Rgb& color, float intensity, bool additive);

    const LightProfile* m_profile = nullptr;
    LightEngine* m_engine = nullptr;
    std::vector<ActiveAnimation> m_active;
};

}
