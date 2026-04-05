#pragma once
#include <stddef.h>
#include <stdint.h>
#include "Color.h"
#include "Layout.h"

namespace light {

    enum class EngineMode {
        Animation,
        Manual
    };

    enum class LightEvent {
        Idle,
        Fire,
        Reload,
        Hit,
        ChargeStart,
        ChargeStop
    };

    class LightEngine {
    public:
        static constexpr size_t kMaxPixels = 16;
        static constexpr int kCanvasWidth = 32;
        static constexpr int kCanvasHeight = 16;

        LightEngine();

        void setLayout(const LedPoint* points, size_t count);
        void setMode(EngineMode mode);

        void setManualPixel(size_t index, Rgb color);
        void clearManual();

        void handleEvent(LightEvent event);
        void setCursor(float u, float v);

        void update(uint32_t nowMs);

        const Rgb* pixels() const { return m_pixels; }
        size_t pixelCount() const { return m_count; }

        const Rgb* canvas() const { return m_canvas; }
        int canvasWidth() const { return kCanvasWidth; }
        int canvasHeight() const { return kCanvasHeight; }

    private:
        void clearCanvas();
        void fillCanvas(Rgb c);
        void setCanvasPixel(int x, int y, Rgb c);
        void addCanvasPixel(int x, int y, Rgb c);
        void drawBlob(float u, float v, float radius, Rgb color, float intensity);
        void drawVerticalStripes(float phase, float width, Rgb a, Rgb b);
        void drawRingPulse(float strength, Rgb color);
        Rgb sampleCanvas(float u, float v) const;
        void renderAnimation(uint32_t nowMs);
        void mapToPixels();

        const LedPoint* m_layout;
        EngineMode m_mode;
        Rgb m_pixels[kMaxPixels];
        Rgb m_manual[kMaxPixels];
        size_t m_count;

        Rgb m_canvas[kCanvasWidth * kCanvasHeight];

        LightEvent m_state;
        uint32_t m_stateStartMs;
        bool m_chargeActive;
        float m_cursorU;
        float m_cursorV;
    };

}