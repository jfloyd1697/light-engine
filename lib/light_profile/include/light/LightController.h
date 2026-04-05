#pragma once
#include <string>
#include "light/LightAnimator.h"
#include "light/LightProfile.h"

namespace light {

class LightController {
public:
    void setProfile(const LightProfile* profile);
    void bindEngine(LightEngine* engine);

    void trigger(const std::string& eventName, uint32_t nowMs);
    void update(uint32_t nowMs);

private:
    const LightProfile* m_profile = nullptr;
    LightAnimator m_animator;
};

}
