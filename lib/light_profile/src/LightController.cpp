#include "light/LightController.h"

namespace light {

void LightController::setProfile(const LightProfile* profile) {
    m_profile = profile;
    m_animator.setProfile(profile);
}

void LightController::bindEngine(LightEngine* engine) {
    m_animator.setEngine(engine);
}

void LightController::trigger(const std::string& eventName, uint32_t nowMs) {
    if (!m_profile) return;
    std::unordered_map<std::string, LightEventDefinition>::const_iterator it = m_profile->events.find(eventName);
    if (it == m_profile->events.end()) return;
    for (size_t i=0;i<it->second.actions.size();++i) {
        const LightAction& a = it->second.actions[i];
        if (a.op == "stop_layer") m_animator.stopLayer(a.layer);
        else if (a.op == "stop_all") m_animator.stopAll();
        else if (a.op == "play_animation") m_animator.playAnimation(a.name, a.layer, a.blend, a.loop, nowMs);
    }
}

void LightController::update(uint32_t nowMs) {
    m_animator.update(nowMs);
}

}
