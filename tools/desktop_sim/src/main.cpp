#include <SDL.h>
#include <SDL_ttf.h>

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "light/LightEngine.h"
#include "light/Layout.h"

namespace {

constexpr int kWindowWidth = 1000;
constexpr int kWindowHeight = 700;

constexpr int kCanvasX = 30;
constexpr int kCanvasY = 30;
constexpr int kCanvasW = 640;
constexpr int kCanvasH = 480;

constexpr int kLedPanelX = 720;
constexpr int kLedPanelY = 40;
constexpr int kLedRadius = 18;

const char* kFontPath = "C:/Windows/Fonts/consola.ttf";

struct AppState {
    bool running = true;
    bool showHelp = true;
    bool manualMode = false;
    float cursorU = 0.5f;
    float cursorV = 0.5f;
    std::string currentEvent = "Idle";
};

static void setColor(SDL_Renderer* r, const light::Rgb& c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, 255);
}

static light::Rgb sampleCanvasColor(float u, float v, const light::LightEngine& engine, uint32_t nowMs) {
    // Reuse the engine's active rendered LED colors only for LED view.
    // For the canvas, create a simple visualization matching current event feel.
    // This keeps the overlay work independent of deeper engine internals.
    (void)engine;

    const float du = u - 0.5f;
    const float dv = v - 0.5f;
    const float dist = std::sqrt(du * du + dv * dv);
    const float pulse = 0.5f + 0.5f * std::sin(nowMs / 250.0f);

    float strength = 1.0f - dist * 1.6f;
    if (strength < 0.0f) strength = 0.0f;
    strength *= (0.4f + 0.6f * pulse);

    return light::Rgb(
        static_cast<uint8_t>(30 + 120 * strength),
        static_cast<uint8_t>(20 + 60 * strength),
        static_cast<uint8_t>(40 + 180 * strength)
    );
}

static SDL_Texture* makeText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color, int& w, int& h) {
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surf) return nullptr;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    w = surf->w;
    h = surf->h;
    SDL_FreeSurface(surf);
    return tex;
}

static void drawText(SDL_Renderer* renderer, TTF_Font* font, int x, int y, const std::string& text, SDL_Color color) {
    int w = 0, h = 0;
    SDL_Texture* tex = makeText(renderer, font, text, color, w, h);
    if (!tex) return;

    SDL_Rect dst{x, y, w, h};
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
}

static void handleKey(SDL_Keycode key, AppState& app, light::LightEngine& engine) {
    switch (key) {
        case SDLK_ESCAPE:
            app.running = false;
            break;

        case SDLK_h:
            app.showHelp = !app.showHelp;
            break;

        case SDLK_a:
            app.manualMode = false;
            engine.setMode(light::EngineMode::Animation);
            app.currentEvent = "Animation";
            break;

        case SDLK_m:
            app.manualMode = true;
            engine.setMode(light::EngineMode::Manual);
            app.currentEvent = "Manual";
            break;

        case SDLK_1:
            engine.handleEvent(light::LightEvent::Idle);
            app.manualMode = false;
            app.currentEvent = "Idle";
            break;

        case SDLK_2:
        case SDLK_SPACE:
            engine.handleEvent(light::LightEvent::Fire);
            app.manualMode = false;
            app.currentEvent = "Fire";
            break;

        case SDLK_3:
            engine.handleEvent(light::LightEvent::Reload);
            app.manualMode = false;
            app.currentEvent = "Reload";
            break;

        case SDLK_4:
            engine.handleEvent(light::LightEvent::Hit);
            app.manualMode = false;
            app.currentEvent = "Hit";
            break;

        case SDLK_5:
            engine.handleEvent(light::LightEvent::ChargeStart);
            app.manualMode = false;
            app.currentEvent = "ChargeStart";
            break;

        case SDLK_6:
            engine.handleEvent(light::LightEvent::ChargeStop);
            app.manualMode = false;
            app.currentEvent = "ChargeStop";
            break;

        case SDLK_c:
            engine.clearManual();
            app.currentEvent = "Clear";
            break;

        case SDLK_LEFT:
            app.cursorU -= 0.03f;
            if (app.cursorU < 0.0f) app.cursorU = 0.0f;
            engine.setCursor(app.cursorU, app.cursorV);
            break;

        case SDLK_RIGHT:
            app.cursorU += 0.03f;
            if (app.cursorU > 1.0f) app.cursorU = 1.0f;
            engine.setCursor(app.cursorU, app.cursorV);
            break;

        case SDLK_UP:
            app.cursorV -= 0.03f;
            if (app.cursorV < 0.0f) app.cursorV = 0.0f;
            engine.setCursor(app.cursorU, app.cursorV);
            break;

        case SDLK_DOWN:
            app.cursorV += 0.03f;
            if (app.cursorV > 1.0f) app.cursorV = 1.0f;
            engine.setCursor(app.cursorU, app.cursorV);
            break;

        case SDLK_q:
            if (app.manualMode) engine.setManualPixel(0, light::Rgb(255, 0, 0));
            break;

        case SDLK_w:
            if (app.manualMode) engine.setManualPixel(1, light::Rgb(0, 255, 0));
            break;

        case SDLK_e:
            if (app.manualMode) engine.setManualPixel(2, light::Rgb(0, 0, 255));
            break;

        default:
            break;
    }
}

static void drawCanvas(SDL_Renderer* renderer, const light::LightEngine& engine, uint32_t nowMs, const AppState& app) {
    SDL_Rect area{kCanvasX, kCanvasY, kCanvasW, kCanvasH};

    SDL_SetRenderDrawColor(renderer, 18, 18, 22, 255);
    SDL_RenderFillRect(renderer, &area);

    const int cell = 8;
    for (int y = 0; y < kCanvasH; y += cell) {
        for (int x = 0; x < kCanvasW; x += cell) {
            float u = static_cast<float>(x) / static_cast<float>(kCanvasW - 1);
            float v = static_cast<float>(y) / static_cast<float>(kCanvasH - 1);
            light::Rgb c = sampleCanvasColor(u, v, engine, nowMs);
            setColor(renderer, c);
            SDL_Rect r{kCanvasX + x, kCanvasY + y, cell, cell};
            SDL_RenderFillRect(renderer, &r);
        }
    }

    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    SDL_RenderDrawRect(renderer, &area);

    int cx = kCanvasX + static_cast<int>(app.cursorU * kCanvasW);
    int cy = kCanvasY + static_cast<int>(app.cursorV * kCanvasH);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, cx - 10, cy, cx + 10, cy);
    SDL_RenderDrawLine(renderer, cx, cy - 10, cx, cy + 10);
}

static void drawLeds(SDL_Renderer* renderer, const light::LightEngine& engine) {
    const light::LayoutView layout = light::circuitPlaygroundRingLayout();
    const light::Rgb* pixels = engine.pixels();
    const int count = static_cast<int>(engine.pixelCount());

    for (int i = 0; i < count; ++i) {
        float u = layout.points[i].u;
        float v = layout.points[i].v;

        int x = kLedPanelX + static_cast<int>((1.0f - u) * 180.0f);
        int y = kLedPanelY + static_cast<int>((1.0f - v) * 180.0f);

        light::Rgb c = pixels[i];
        setColor(renderer, c);

        for (int dy = -kLedRadius; dy <= kLedRadius; ++dy) {
            for (int dx = -kLedRadius; dx <= kLedRadius; ++dx) {
                if (dx * dx + dy * dy <= kLedRadius * kLedRadius) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
        for (int dy = -kLedRadius; dy <= kLedRadius; ++dy) {
            for (int dx = -kLedRadius; dx <= kLedRadius; ++dx) {
                int d2 = dx * dx + dy * dy;
                if (d2 <= kLedRadius * kLedRadius && d2 >= (kLedRadius - 1) * (kLedRadius - 1)) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }
    }
}

static void drawOverlay(SDL_Renderer* renderer, TTF_Font* font, const AppState& app) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect status{20, 530, 700, 90};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 170);
    SDL_RenderFillRect(renderer, &status);
    SDL_SetRenderDrawColor(renderer, 90, 90, 90, 255);
    SDL_RenderDrawRect(renderer, &status);

    SDL_Color white{240, 240, 240, 255};
    char buf[128];
    std::snprintf(buf, sizeof(buf), "Mode: %s   Event: %s   Cursor: (%.2f, %.2f)",
                  app.manualMode ? "Manual" : "Animation",
                  app.currentEvent.c_str(),
                  app.cursorU, app.cursorV);
    drawText(renderer, font, 32, 545, buf, white);
    drawText(renderer, font, 32, 575, "Press H to toggle help", white);

    if (!app.showHelp) return;

    SDL_Rect help{730, 260, 245, 250};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, &help);
    SDL_SetRenderDrawColor(renderer, 90, 90, 90, 255);
    SDL_RenderDrawRect(renderer, &help);

    std::vector<std::string> lines = {
        "1 Idle",
        "2 / Space Fire",
        "3 Reload",
        "4 Hit",
        "5 Charge Start",
        "6 Charge Stop",
        "A Animation Mode",
        "M Manual Mode",
        "C Clear Manual",
        "Q/W/E Set LEDs 0/1/2",
        "Arrows Move Cursor",
        "H Toggle Help",
        "Esc Quit"
    };

    int y = 274;
    for (const auto& line : lines) {
        drawText(renderer, font, 744, y, line, white);
        y += 18;
    }
}

} // namespace

int main(int, char**) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        std::fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Light Demo",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        kWindowWidth,
        kWindowHeight,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont(kFontPath, 16);
    if (!font) {
        std::fprintf(stderr, "TTF_OpenFont failed: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    light::LightEngine engine;
    light::LayoutView layout = light::circuitPlaygroundRingLayout();
    engine.setLayout(layout.points, layout.count);
    engine.handleEvent(light::LightEvent::Idle);

    AppState app;

    while (app.running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                app.running = false;
            } else if (event.type == SDL_KEYDOWN) {
                handleKey(event.key.keysym.sym, app, engine);
            }
        }

        uint32_t nowMs = SDL_GetTicks();
        engine.update(nowMs);

        SDL_SetRenderDrawColor(renderer, 28, 28, 34, 255);
        SDL_RenderClear(renderer);

        drawCanvas(renderer, engine, nowMs, app);
        drawLeds(renderer, engine);
        drawOverlay(renderer, font, app);

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}