#include "light/LightEngine.h"
#include <math.h>

namespace light {
namespace {

static float clamp01(float x) {
    if (x < 0.0f) return 0.0f;
    if (x > 1.0f) return 1.0f;
    return x;
}

static int clampInt(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static const LedPoint kCpxRing[] = {
    LedPoint(0, 0.50f, 0.05f, 1.0f),
    LedPoint(1, 0.78f, 0.14f, 1.0f),
    LedPoint(2, 0.93f, 0.38f, 1.0f),
    LedPoint(3, 0.93f, 0.62f, 1.0f),
    LedPoint(4, 0.78f, 0.86f, 1.0f),
    LedPoint(5, 0.50f, 0.95f, 1.0f),
    LedPoint(6, 0.22f, 0.86f, 1.0f),
    LedPoint(7, 0.07f, 0.62f, 1.0f),
    LedPoint(8, 0.07f, 0.38f, 1.0f),
    LedPoint(9, 0.22f, 0.14f, 1.0f)
};

} // namespace

LightEngine::LightEngine()
    : m_layout(kCpxRing),
      m_mode(EngineMode::Animation),
      m_count(10),
      m_state(LightEvent::Idle),
      m_stateStartMs(0),
      m_chargeActive(false),
      m_cursorU(0.5f),
      m_cursorV(0.5f) {
    clearManual();
    clearCanvas();
    for (size_t i = 0; i < kMaxPixels; ++i) {
        m_pixels[i] = Rgb(0, 0, 0);
    }
}

void LightEngine::setLayout(const LedPoint* points, size_t count) {
    m_layout = points;
    m_count = (count < kMaxPixels) ? count : kMaxPixels;
}

void LightEngine::setMode(EngineMode mode) {
    m_mode = mode;
}

void LightEngine::setManualPixel(size_t index, Rgb color) {
    if (index >= kMaxPixels) return;
    m_manual[index] = color;
}

void LightEngine::clearManual() {
    for (size_t i = 0; i < kMaxPixels; ++i) {
        m_manual[i] = Rgb(0, 0, 0);
    }
}

void LightEngine::handleEvent(LightEvent event) {
    m_state = event;
    m_stateStartMs = 0;

    if (event == LightEvent::ChargeStart) {
        m_chargeActive = true;
    } else if (event == LightEvent::ChargeStop) {
        m_chargeActive = false;
        m_state = LightEvent::Idle;
    }
}

void LightEngine::setCursor(float u, float v) {
    m_cursorU = clamp01(u);
    m_cursorV = clamp01(v);
}

void LightEngine::clearCanvas() {
    fillCanvas(Rgb(0, 0, 0));
}

void LightEngine::fillCanvas(Rgb c) {
    for (int i = 0; i < kCanvasWidth * kCanvasHeight; ++i) {
        m_canvas[i] = c;
    }
}

void LightEngine::setCanvasPixel(int x, int y, Rgb c) {
    if (x < 0 || y < 0 || x >= kCanvasWidth || y >= kCanvasHeight) return;
    m_canvas[y * kCanvasWidth + x] = c;
}

void LightEngine::addCanvasPixel(int x, int y, Rgb c) {
    if (x < 0 || y < 0 || x >= kCanvasWidth || y >= kCanvasHeight) return;
    Rgb& dst = m_canvas[y * kCanvasWidth + x];
    dst = addSaturate(dst, c);
}

void LightEngine::drawBlob(float u, float v, float radius, Rgb color, float intensity) {
    int cx = static_cast<int>(u * (kCanvasWidth - 1));
    int cy = static_cast<int>(v * (kCanvasHeight - 1));
    float rx = radius * kCanvasWidth;
    float ry = radius * kCanvasHeight;

    int minX = clampInt(static_cast<int>(cx - rx - 1), 0, kCanvasWidth - 1);
    int maxX = clampInt(static_cast<int>(cx + rx + 1), 0, kCanvasWidth - 1);
    int minY = clampInt(static_cast<int>(cy - ry - 1), 0, kCanvasHeight - 1);
    int maxY = clampInt(static_cast<int>(cy + ry + 1), 0, kCanvasHeight - 1);

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            float du = (x - cx) / (rx > 0.001f ? rx : 1.0f);
            float dv = (y - cy) / (ry > 0.001f ? ry : 1.0f);
            float d = sqrtf(du * du + dv * dv);
            if (d > 1.0f) continue;

            float falloff = 1.0f - d;
            falloff = 0.35f + 0.65f * falloff;
            falloff *= falloff;
            addCanvasPixel(x, y, scale(color, intensity * falloff));
        }
    }
}

void LightEngine::drawVerticalStripes(float phase, float width, Rgb a, Rgb b) {
    for (int y = 0; y < kCanvasHeight; ++y) {
        for (int x = 0; x < kCanvasWidth; ++x) {
            float u = static_cast<float>(x) / static_cast<float>(kCanvasWidth - 1);
            float p = u + phase;
            p = p - floorf(p);

            float period = width * 2.0f;
            float local = fmodf(p, period);
            setCanvasPixel(x, y, (local < width) ? a : b);
        }
    }
}

void LightEngine::drawRingPulse(float strength, Rgb color) {
    for (int y = 0; y < kCanvasHeight; ++y) {
        for (int x = 0; x < kCanvasWidth; ++x) {
            float u = static_cast<float>(x) / static_cast<float>(kCanvasWidth - 1);
            float v = static_cast<float>(y) / static_cast<float>(kCanvasHeight - 1);
            float du = u - 0.5f;
            float dv = v - 0.5f;
            float dist = sqrtf(du * du + dv * dv);
            float ring = 1.0f - fabsf(dist - 0.35f) / 0.12f;
            ring = clamp01(ring);
            addCanvasPixel(x, y, scale(color, ring * strength));
        }
    }
}

Rgb LightEngine::sampleCanvas(float u, float v) const {
    int x = clampInt(static_cast<int>(u * (kCanvasWidth - 1)), 0, kCanvasWidth - 1);
    int y = clampInt(static_cast<int>(v * (kCanvasHeight - 1)), 0, kCanvasHeight - 1);
    return m_canvas[y * kCanvasWidth + x];
}

void LightEngine::renderAnimation(uint32_t nowMs) {
    if (m_stateStartMs == 0) m_stateStartMs = nowMs;
    uint32_t dt = nowMs - m_stateStartMs;

    clearCanvas();

    switch (m_state) {
        case LightEvent::Idle: {
            float pulse = 0.5f + 0.5f * sinf(nowMs * 0.0025f);
            drawBlob(0.5f, 0.5f, 0.42f, Rgb(40, 140, 255), 0.55f + 0.45f * pulse);
            drawRingPulse(0.25f + 0.35f * pulse, Rgb(0, 80, 180));
            break;
        }

        case LightEvent::Fire: {
            float t = dt / 180.0f;
            if (t >= 1.0f) {
                m_state = m_chargeActive ? LightEvent::ChargeStart : LightEvent::Idle;
                m_stateStartMs = nowMs;
                renderAnimation(nowMs);
                return;
            }
            float strength = 1.0f - t;
            drawBlob(0.5f, 0.12f, 0.34f, Rgb(255, 220, 80), strength * 1.0f);
            drawBlob(0.5f, 0.28f, 0.24f, Rgb(255, 110, 20), strength * 0.9f);
            break;
        }

        case LightEvent::Reload: {
            float phase = nowMs * 0.0015f;
            drawVerticalStripes(phase, 0.10f, Rgb(0, 30, 90), Rgb(0, 200, 255));
            break;
        }

        case LightEvent::Hit: {
            float t = dt / 250.0f;
            if (t >= 1.0f) {
                m_state = m_chargeActive ? LightEvent::ChargeStart : LightEvent::Idle;
                m_stateStartMs = nowMs;
                renderAnimation(nowMs);
                return;
            }
            float strength = 1.0f - t;
            drawBlob(0.5f, 0.5f, 0.60f, Rgb(255, 40, 40), strength);
            break;
        }

        case LightEvent::ChargeStart: {
            float pulse = 0.5f + 0.5f * sinf(nowMs * 0.006f);
            drawBlob(m_cursorU, m_cursorV, 0.42f, Rgb(255, 40, 220), 0.60f + 0.40f * pulse);
            drawRingPulse(0.35f + 0.35f * pulse, Rgb(200, 20, 150));
            break;
        }

        case LightEvent::ChargeStop: {
            drawBlob(0.5f, 0.5f, 0.20f, Rgb(0, 40, 180), 0.5f);
            break;
        }
    }
}

void LightEngine::mapToPixels() {
    if (m_mode == EngineMode::Manual) {
        for (size_t i = 0; i < m_count; ++i) {
            m_pixels[i] = m_manual[i];
        }
        return;
    }

    for (size_t i = 0; i < m_count; ++i) {
        if (!m_layout) {
            m_pixels[i] = Rgb(0, 0, 0);
            continue;
        }
        const LedPoint& p = m_layout[i];
        m_pixels[i] = scale(sampleCanvas(p.u, p.v), p.brightnessScale);
    }
}

void LightEngine::update(uint32_t nowMs) {
    if (m_mode == EngineMode::Animation) {
        renderAnimation(nowMs);
    }
    mapToPixels();
}

}