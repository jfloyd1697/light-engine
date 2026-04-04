#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>

#include "light/LightEngine.h"
#include "light/Rasterizer.h"
#include "light/effects/PulseBlobEffect.h"
#include "light/effects/StripeScrollEffect.h"

using namespace light;

namespace {

char brightnessChar(const Rgb& c) {
    const int value = static_cast<int>(c.r) + static_cast<int>(c.g) + static_cast<int>(c.b);
    if (value > 540) return '#';
    if (value > 300) return '*';
    if (value > 120) return '+';
    if (value > 20) return '.';
    return ' ';
}

void printCanvas(const Canvas2D& canvas) {
    for (int y = 0; y < canvas.height(); ++y) {
        for (int x = 0; x < canvas.width(); ++x) {
            std::cout << brightnessChar(canvas.getPixel(x, y));
        }
        std::cout << '\n';
    }
}

void printLedSamples(const Layout& layout, const std::vector<Rgb>& leds) {
    std::cout << "LED samples:\n";
    for (std::size_t i = 0; i < layout.leds().size(); ++i) {
        const auto& point = layout.leds()[i];
        const auto& c = leds[i];
        std::cout << "  [" << point.index << "] " << std::setw(11) << point.label
                  << " @ (" << std::fixed << std::setprecision(2) << point.u << ", " << point.v << ")"
                  << " -> rgb(" << static_cast<int>(c.r) << ", " << static_cast<int>(c.g) << ", " << static_cast<int>(c.b) << ")\n";
    }
}

} // namespace

int main() {
    auto pulse = std::make_shared<effects::PulseBlobEffect>();
    pulse->centerU = 0.5f;
    pulse->centerV = 0.5f;
    pulse->radius = 0.60f;
    pulse->color = {255, 100, 30};
    pulse->speedHz = 1.0f;

    auto stripes = std::make_shared<effects::StripeScrollEffect>();
    stripes->primary = {0, 90, 255};
    stripes->stripeWidth = 0.15f;

    Layout layout = Layout::makeCircuitPlaygroundRing();
    LightEngine engine(layout);
    engine.setAnimationField(pulse);
    engine.setManualField(stripes);

    std::cout << "=== Animation mode: pulse blob ===\n";
    engine.setMode(LightEngine::Mode::Animation);
    const uint32_t animTimeMs = 750;
    Canvas2D canvas = rasterize(*pulse, 32, 16, animTimeMs);
    printCanvas(canvas);
    printLedSamples(layout, engine.renderLeds(animTimeMs));

    std::cout << "\n=== Manual mode: scrolling stripes ===\n";
    engine.setMode(LightEngine::Mode::ManualField);
    const uint32_t manualTimeMs = 1250;
    canvas = rasterize(*stripes, 32, 8, manualTimeMs);
    printCanvas(canvas);
    printLedSamples(layout, engine.renderLeds(manualTimeMs));

    return 0;
}
