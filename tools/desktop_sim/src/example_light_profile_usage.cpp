#include <iostream>
#include "light/LightController.h"
#include "light/LightProfileLoader.h"

int main() {
    light::LightProfile profile;
    std::string error;
    if (!light::LightProfileLoader::loadFromFile("assets/lights/muzzle_lights.json", profile, error)) {
        std::cerr << "Failed to load light profile: " << error << "\n";
        return 1;
    }
    light::LightEngine engine;
    light::LayoutView layout = light::circuitPlaygroundRingLayout();
    engine.setLayout(layout.points, layout.count);
    light::LightController controller;
    controller.setProfile(&profile);
    controller.bindEngine(&engine);
    controller.trigger("idle", 0);
    controller.trigger("fire", 100);
    std::cout << "Loaded layout: " << profile.layoutName << "\n";
    std::cout << "Animations: " << profile.animations.size() << "\n";
    std::cout << "Events: " << profile.events.size() << "\n";
    return 0;
}
