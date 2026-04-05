#include <iomanip>
#include <iostream>
#include <string>

#include "light/Layout.h"
#include "light/LightController.h"
#include "light/LightProfileLoader.h"

static void printPixels(const light::LightEngine& engine, uint32_t t) {
    std::cout << "t=" << std::setw(4) << t << "ms ";
    const light::Rgb* p = engine.pixels();
    for (size_t i = 0; i < engine.pixelCount(); ++i) {
        std::cout << "[" << i << ":" << std::setw(3) << int(p[i].r) << "," << std::setw(3) << int(p[i].g) << "," << std::setw(3) << int(p[i].b) << "] ";
    }
    std::cout << "\n";
}

int main() {
    light::LightProfile profile;
    std::string error;
    if (!light::LightProfileLoader::loadFromFile("assets/lights/muzzle_lights.json", profile, error)) {
        std::cerr << "Failed to load profile: " << error << "\n";
        return 1;
    }

    light::LightEngine engine;
    light::LayoutView layout = light::circuitPlaygroundRingLayout();
    engine.setLayout(layout.points, layout.count);

    light::LightController controller;
    controller.setProfile(&profile);
    controller.bindEngine(&engine);

    controller.trigger("idle", 0);
    for (uint32_t t = 0; t <= 1000; t += 200) {
        controller.update(t);
        printPixels(engine, t);
    }

    controller.trigger("fire", 1100);
    for (uint32_t t = 1100; t <= 1400; t += 50) {
        controller.update(t);
        printPixels(engine, t);
    }

    controller.trigger("reload_start", 1500);
    for (uint32_t t = 1500; t <= 1900; t += 100) {
        controller.update(t);
        printPixels(engine, t);
    }

    controller.trigger("charge_start", 2000);
    for (uint32_t t = 2000; t <= 2600; t += 100) {
        controller.update(t);
        printPixels(engine, t);
    }

    controller.trigger("charge_release", 2700);
    for (uint32_t t = 2700; t <= 3000; t += 75) {
        controller.update(t);
        printPixels(engine, t);
    }

    return 0;
}
