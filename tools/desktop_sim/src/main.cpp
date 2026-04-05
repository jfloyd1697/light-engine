#include <SDL.h>
#include <SDL_ttf.h>

#include <cstdio>
#include <string>
#include <vector>

#include "light/LightController.h"
#include "light/LightProfileLoader.h"
#include "light/LightEngine.h"
#include "light/Layout.h"

namespace {

constexpr int kWindowWidth = 1000;
constexpr int kWindowHeight = 820;

constexpr int kCanvasX = 30;
constexpr int kCanvasY = 30;
constexpr int kCanvasSize = 640;
constexpr int kCanvasW = kCanvasSize;
constexpr int kCanvasH = kCanvasSize;

const char* kFontPath = "C:/Windows/Fonts/consola.ttf";

struct AppState {
    bool running = true;
    bool showHelp = true;
    std::string currentEvent = "idle";
    bool spaceHeld = false;
};

static void setColor(SDL_Renderer* renderer, const light::Rgb& c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
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
    int w = 0;
    int h = 0;
    SDL_Texture* tex = makeText(renderer, font, text, color, w, h);
    if (!tex) return;

    SDL_Rect dst{x, y, w, h};
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
}

static light::Rgb sampleCanvasColor(float u, float v, const light::LightEngine& engine) {
    const light::Rgb* canvas = engine.canvas();
    const int cw = engine.canvasWidth();
    const int ch = engine.canvasHeight();

    if (!canvas || cw <= 0 || ch <= 0) {
        return light::Rgb(0, 0, 0);
    }

    int x = static_cast<int>(u * static_cast<float>(cw - 1));
    int y = static_cast<int>(v * static_cast<float>(ch - 1));

    if (x < 0) x = 0;
    if (x >= cw) x = cw - 1;
    if (y < 0) y = 0;
    if (y >= ch) y = ch - 1;

    return canvas[y * cw + x];
}

static int canvasPixelX(float u, const light::LightEngine& engine) {
    const int cw = engine.canvasWidth();
    const int ch = engine.canvasHeight();
    const int cellW = kCanvasW / cw;
    const int cellH = kCanvasH / ch;
    const int cell = (cellW < cellH) ? cellW : cellH;
    const int drawW = cell * cw;
    const int offsetX = kCanvasX + (kCanvasW - drawW) / 2;
    return offsetX + static_cast<int>(u * (drawW - 1));
}

static int canvasPixelY(float v, const light::LightEngine& engine) {
    const int cw = engine.canvasWidth();
    const int ch = engine.canvasHeight();
    const int cellW = kCanvasW / cw;
    const int cellH = kCanvasH / ch;
    const int cell = (cellW < cellH) ? cellW : cellH;
    const int drawH = cell * ch;
    const int offsetY = kCanvasY + (kCanvasH - drawH) / 2;
    return offsetY + static_cast<int>(v * (drawH - 1));
}

static void triggerEvent(light::LightController& controller,
                         AppState& app,
                         const std::string& eventName,
                         uint32_t nowMs) {
    controller.trigger(eventName, nowMs);
    app.currentEvent = eventName;
}

static void handleKey(SDL_Keycode key,
                      AppState& app,
                      light::LightController& controller,
                      uint32_t nowMs) {
    switch (key) {
        case SDLK_ESCAPE:
            app.running = false;
            break;
        case SDLK_SPACE:
            if (!app.spaceHeld) {
                app.spaceHeld = true;
                std::printf("SPACE DOWN -> charge_start\n");
                triggerEvent(controller, app, "charge_start", SDL_GetTicks());
            }
            if (app.spaceHeld) {
                app.spaceHeld = false;
                std::printf("SPACE UP -> charge_release\n");
                triggerEvent(controller, app, "charge_release", SDL_GetTicks());
            }

        case SDLK_h:
            app.showHelp = !app.showHelp;
            break;
        case SDLK_1:
            triggerEvent(controller, app, "idle", nowMs);
            break;
        case SDLK_2:
            triggerEvent(controller, app, "fire", nowMs);
            break;
        case SDLK_3:
            triggerEvent(controller, app, "burst_fire", nowMs);
            break;
        case SDLK_4:
            triggerEvent(controller, app, "reload_start", nowMs);
            break;
        case SDLK_5:
            triggerEvent(controller, app, "charge_start", nowMs);
            break;
        case SDLK_6:
            triggerEvent(controller, app, "charge_release", nowMs);
            break;
        case SDLK_7:
            triggerEvent(controller, app, "overheat", nowMs);
            break;
        case SDLK_8:
            triggerEvent(controller, app, "empty", nowMs);
            break;
        case SDLK_9:
            triggerEvent(controller, app, "reload_complete", nowMs);
            break;
        default:
            break;
    }
}

static void drawCanvas(SDL_Renderer* renderer, const light::LightEngine& engine) {
    SDL_Rect area{kCanvasX, kCanvasY, kCanvasW, kCanvasH};

    SDL_SetRenderDrawColor(renderer, 18, 18, 22, 255);
    SDL_RenderFillRect(renderer, &area);

    const int cw = engine.canvasWidth();
    const int ch = engine.canvasHeight();

    const int cellW = kCanvasW / cw;
    const int cellH = kCanvasH / ch;
    const int cell = (cellW < cellH) ? cellW : cellH;

    const int drawW = cell * cw;
    const int drawH = cell * ch;
    const int offsetX = kCanvasX + (kCanvasW - drawW) / 2;
    const int offsetY = kCanvasY + (kCanvasH - drawH) / 2;

    for (int y = 0; y < ch; ++y) {
        for (int x = 0; x < cw; ++x) {
            const float u = static_cast<float>(x) / static_cast<float>(cw - 1);
            const float v = static_cast<float>(y) / static_cast<float>(ch - 1);
            const light::Rgb c = sampleCanvasColor(u, v, engine);
            setColor(renderer, c);

            SDL_Rect r{
                offsetX + x * cell,
                offsetY + y * cell,
                cell,
                cell
            };
            SDL_RenderFillRect(renderer, &r);
        }
    }

    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    SDL_RenderDrawRect(renderer, &area);
}

static void drawLedsOnCanvas(SDL_Renderer* renderer,
                             TTF_Font* font,
                             const light::LightEngine& engine) {
    const light::LayoutView layout = light::circuitPlaygroundRingLayout();
    const light::Rgb* pixels = engine.pixels();
    const int count = static_cast<int>(engine.pixelCount());
    const int ledRadius = 10;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    for (int i = 0; i < count; ++i) {
        const float u = layout.points[i].u;
        const float v = layout.points[i].v;

        const int x = canvasPixelX(u, engine);
        const int y = canvasPixelY(v, engine);
        const light::Rgb c = pixels[i];

        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 190);
        for (int dy = -ledRadius; dy <= ledRadius; ++dy) {
            for (int dx = -ledRadius; dx <= ledRadius; ++dx) {
                if (dx * dx + dy * dy <= ledRadius * ledRadius) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int dy = -ledRadius; dy <= ledRadius; ++dy) {
            for (int dx = -ledRadius; dx <= ledRadius; ++dx) {
                const int d2 = dx * dx + dy * dy;
                if (d2 <= ledRadius * ledRadius &&
                    d2 >= (ledRadius - 1) * (ledRadius - 1)) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }

        drawText(renderer, font, x + 12, y - 8, std::to_string(i), SDL_Color{255, 255, 255, 255});
    }
}

static void drawLedPanel(SDL_Renderer* renderer,
                         TTF_Font* font,
                         const light::LightEngine& engine) {
    const light::LayoutView layout = light::circuitPlaygroundRingLayout();
    const light::Rgb* pixels = engine.pixels();
    const int count = static_cast<int>(engine.pixelCount());
    const int ledRadius = 18;

    const int panelX = 700;
    const int panelY = 20;
    const int panelW = 260;
    const int panelH = 220;
    const int previewX = 735;
    const int previewY = 55;
    const int previewW = 150;
    const int previewH = 150;

    SDL_Rect panel{panelX, panelY, panelW, panelH};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 170);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 90, 90, 90, 255);
    SDL_RenderDrawRect(renderer, &panel);
    drawText(renderer, font, panelX + 15, panelY + 10, "LED Preview", SDL_Color{240, 240, 240, 255});

    for (int i = 0; i < count; ++i) {
        const float u = layout.points[i].u;
        const float v = layout.points[i].v;
        const int x = previewX + static_cast<int>(u * previewW);
        const int y = previewY + static_cast<int>(v * previewH);
        const light::Rgb c = pixels[i];

        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 220);
        for (int dy = -ledRadius; dy <= ledRadius; ++dy) {
            for (int dx = -ledRadius; dx <= ledRadius; ++dx) {
                if (dx * dx + dy * dy <= ledRadius * ledRadius) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int dy = -ledRadius; dy <= ledRadius; ++dy) {
            for (int dx = -ledRadius; dx <= ledRadius; ++dx) {
                const int d2 = dx * dx + dy * dy;
                if (d2 <= ledRadius * ledRadius &&
                    d2 >= (ledRadius - 1) * (ledRadius - 1)) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }

        drawText(renderer, font, x + 22, y - 8, std::to_string(i), SDL_Color{255, 255, 255, 255});
    }
}

static void drawOverlay(SDL_Renderer* renderer, TTF_Font* font, const AppState& app) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect status{20, 690, 700, 90};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 170);
    SDL_RenderFillRect(renderer, &status);
    SDL_SetRenderDrawColor(renderer, 90, 90, 90, 255);
    SDL_RenderDrawRect(renderer, &status);

    SDL_Color white{240, 240, 240, 255};
    drawText(renderer, font, 32, 705, "Current event: " + app.currentEvent, white);
    drawText(renderer, font, 32, 735, "1 idle  2 fire  3 burst  4 reload  5 charge  6 release  7 overheat  8 empty  9 reload_complete  Hold Space = charge, release = shot, H help", white);

    if (!app.showHelp) return;

    SDL_Rect help{730, 260, 245, 240};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, &help);
    SDL_SetRenderDrawColor(renderer, 90, 90, 90, 255);
    SDL_RenderDrawRect(renderer, &help);

    std::vector<std::string> lines = {
        "1 Idle",
        "2 Fire",
        "3 Burst Fire",
        "4 Reload Start",
        "5 Charge Start",
        "6 Charge Release",
        "7 Overheat",
        "8 Empty",
        "9 Reload Complete",
        "Hold Space = Charge",
        "Release Space = Shot",
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

    light::LightProfile profile;
    std::string error;
    if (!light::LightProfileLoader::loadFromFile("assets/lights/muzzle_lights.json", profile, error)) {
        std::fprintf(stderr, "Failed to load light profile: %s\n", error.c_str());
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    light::LightEngine engine;
    light::LayoutView layout = light::circuitPlaygroundRingLayout();
    engine.setLayout(layout.points, layout.count);

    light::LightController controller;
    controller.bindEngine(&engine);
    controller.setProfile(&profile);

    AppState app;
    triggerEvent(controller, app, "idle", 0);

    while (app.running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                app.running = false;
            } else if (event.type == SDL_KEYDOWN) {
                const SDL_Keycode key = event.key.keysym.sym;

                if (key == SDLK_SPACE) {
                    if (!app.spaceHeld) {
                        app.spaceHeld = true;
                        triggerEvent(controller, app, "charge_start", SDL_GetTicks());
                    }
                } else if (event.key.repeat == 0) {
                    handleKey(key, app, controller, SDL_GetTicks());
                }
            } else if (event.type == SDL_KEYUP) {
                const SDL_Keycode key = event.key.keysym.sym;

                if (key == SDLK_SPACE) {
                    if (app.spaceHeld) {
                        app.spaceHeld = false;
                        triggerEvent(controller, app, "charge_release", SDL_GetTicks());
                    }
                }
            }
        }

        const uint32_t nowMs = SDL_GetTicks();
        controller.update(nowMs);

        SDL_SetRenderDrawColor(renderer, 28, 28, 34, 255);
        SDL_RenderClear(renderer);

        drawCanvas(renderer, engine);
        drawLedsOnCanvas(renderer, font, engine);
        drawLedPanel(renderer, font, engine);
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