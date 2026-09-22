/*
 * Copyright (c) 2025 - 2026 ThorVG project. All rights reserved.

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    struct Star {
        tvg::Shape* shape;
        float brightness, speed, phase;
    };

    struct Spark {
        float vx, vy, vz, life, phase;
    };

    enum class BurstPattern { Sphere, Ring, Spiral, Willow };

    struct SparkLayer {
        std::array<tvg::Shape*, 4> trails;
        tvg::Shape* core;
    };

    struct Firework {
        std::array<SparkLayer, 3> layers;
        tvg::Shape* flash;
        tvg::Shape* skyLight;
        std::array<Spark, 150> sparks;
        float x, y, drift, launch, duration;
        BurstPattern pattern;
        float drag, gravity, spin, trailStep;
        float age = 0;
        uint8_t r, g, b;
        bool active = false;
    };

    std::array<Star, 1000> stars;
    std::array<Firework, 6> fireworks;
    size_t previousElapsed = 0;
    size_t nextPattern = 0;

    static float random(float min, float max)
    {
        return min + (max - min) * (float(std::rand()) / RAND_MAX);
    }

    void drawStars(float time)
    {
        for (auto& star : stars) {
            const auto twinkle = 0.5f + 0.5f * std::sin(time * star.speed + star.phase);
            const auto opacity = uint8_t(star.brightness * (0.25f + 0.75f * twinkle * twinkle));
            if (star.shape->opacity() != opacity) star.shape->opacity(opacity);
        }
    }

    void ignite(Firework& firework)
    {
        firework.pattern = static_cast<BurstPattern>(nextPattern++ % 4);
        firework.drag = 1.25f;
        firework.gravity = 0.026f;
        firework.spin = 0;
        firework.trailStep = 0.14f;
        static constexpr uint8_t colors[][3] = {
            {255, 183, 65}, {255, 65, 110}, {75, 185, 255},
            {160, 105, 255}, {70, 255, 185}, {255, 225, 155}
        };
        const auto& color = colors[std::rand() % 6];
        firework.r = color[0];
        firework.g = color[1];
        firework.b = color[2];
        auto light = tvg::RadialGradient::gen();
        light->radial(0, 0, size().h * 0.315f, 0, 0, 0);
        tvg::Fill::ColorStop stops[] = {
            {0, color[0], color[1], color[2], 150},
            {0.25f, color[0], color[1], color[2], 110},
            {0.5f, color[0], color[1], color[2], 45},
            {0.75f, color[0], color[1], color[2], 8},
            {1, color[0], color[1], color[2], 0}
        };
        light->colorStops(stops, 5);
        firework.skyLight->fill(light);
        firework.x = random(0.17f, 0.83f);
        firework.y = random(0.16f, 0.40f);
        firework.drift = random(-0.055f, 0.055f);
        firework.launch = random(1.0f, 1.45f);
        firework.duration = random(2.8f, 3.8f) * 1.25f;
        if (firework.pattern == BurstPattern::Willow) {
            firework.duration *= 1.2f;
            firework.drag = 0.85f;
            firework.gravity = 0.022f;
            firework.trailStep = 0.28f;
        } else if (firework.pattern == BurstPattern::Spiral) {
            firework.spin = random(0, 1) < 0.5f ? -0.8f : 0.8f;
            firework.trailStep = 0.18f;
        }
        const auto velocity = random(0.16f, 0.25f);
        const auto rotation = random(0, 6.2831853f);
        const auto tilt = random(0.55f, 0.95f);
        for (size_t i = 0; i < firework.sparks.size(); ++i) {
            auto angle = random(0, 6.2831853f);
            const auto depth = random(-1, 1);
            auto speed = velocity * std::sqrt(1 - depth * depth) * random(0.8f, 1.1f);
            auto life = random(0.65f, 1.0f) * firework.duration;
            switch (firework.pattern) {
                case BurstPattern::Sphere:
                    break;
                case BurstPattern::Ring:
                    angle = rotation + 6.2831853f * float(i) / firework.sparks.size();
                    speed = velocity * random(0.97f, 1.03f);
                    life = random(0.85f, 1.0f) * firework.duration;
                    break;
                case BurstPattern::Spiral: {
                    // Three curved arms, with sparks spread along each arm's radius.
                    const auto t = float(i / 3) / ((firework.sparks.size() - 1) / 3);
                    angle = rotation + (i % 3) * 2.0943951f + t * 4.5f;
                    speed = velocity * (0.18f + 0.82f * t);
                    life = random(0.85f, 1.0f) * firework.duration;
                    break;
                }
                case BurstPattern::Willow:
                    angle = random(-3.1415927f, 0);
                    speed = velocity * random(0.45f, 1.0f);
                    life = random(0.85f, 1.0f) * firework.duration;
                    break;
            }
            auto vx = std::cos(angle) * speed;
            auto vy = std::sin(angle) * speed;
            auto vz = velocity * depth;
            if (firework.pattern == BurstPattern::Ring || firework.pattern == BurstPattern::Spiral) {
                // Tilt the pattern's plane: its opposite sides move toward and away from us.
                vz = vy * std::sin(tilt);
                vy *= std::cos(tilt);
            } else if (firework.pattern == BurstPattern::Willow) {
                const auto lateral = std::sqrt(1 - depth * depth);
                vx *= lateral;
                vy *= lateral;
                vz = speed * depth;
            }
            firework.sparks[i] = {vx, vy, vz, life, random(0, 6.2831853f)};
        }
        firework.active = true;
    }

    float perspective(const Firework& firework, float vz, float age)
    {
        const auto travel = (1.0f - std::exp(-firework.drag * age)) / firework.drag;
        // Positive depth recedes into the screen; keep the camera plane safely out of reach.
        return 0.5f / std::max(0.2f, 0.5f + vz * travel);
    }

    // Analytic drag and gravity keep the trajectories independent of frame rate.
    tvg::Point position(const Firework& firework, const Spark& spark, float age)
    {
        const auto travel = (1.0f - std::exp(-firework.drag * age)) / firework.drag;
        const auto turn = firework.spin * travel;
        const auto vx = spark.vx * std::cos(turn) - spark.vy * std::sin(turn);
        const auto vy = spark.vx * std::sin(turn) + spark.vy * std::cos(turn);
        const auto scale = float(size().h);
        const auto projection = perspective(firework, spark.vz, age);
        return {firework.x * size().w + vx * travel * scale * projection,
                firework.y * scale + (vy * travel + firework.gravity * age * age) * scale * projection};
    }

    void drawFirework(Firework& firework, float dt)
    {
        firework.age += dt;
        if (firework.age < 0) return;
        if (!firework.active) ignite(firework);

        for (auto& layer : firework.layers) {
            for (auto trail : layer.trails) trail->reset();
            layer.core->reset();
        }
        firework.flash->opacity(0);
        firework.skyLight->opacity(0);

        const auto scale = size().h / 1280.0f;
        const auto burstAge = firework.age - firework.launch;
        if (burstAge < 0) {
            const auto t = firework.age / firework.launch;
            const auto rocket = [&](float time) {
                return tvg::Point{(firework.x - firework.drift * (1 - time)) * size().w,
                                  (0.92f - (0.92f - firework.y) * (2 * time - time * time)) * size().h};
            };
            const auto head = rocket(t);
            for (size_t i = 0; i < firework.layers[1].trails.size(); ++i) {
                auto trail = firework.layers[1].trails[i];
                const auto tail = rocket(std::max(0.0f, t - 0.045f * (i + 1)));
                trail->moveTo(head.x, head.y);
                trail->lineTo(tail.x, tail.y);
                trail->strokeFill(255, 180, 70, uint8_t(150 / (i + 1)));
                trail->strokeWidth((2.0f + i) * scale);
            }
            firework.layers[1].core->appendCircle(head.x, head.y, 2.5f * scale, 2.5f * scale);
            firework.layers[1].core->fill(255, 250, 220);
            firework.layers[1].core->opacity(255);
            return;
        }

        if (burstAge > firework.duration) {
            firework.active = false;
            firework.age = -random(0.15f, 0.9f);
            return;
        }

        // Let the flash brighten as the sparks separate, then fade quickly.
        const auto flashRise = 0.12f;
        const auto flashIntensity = std::min(1.0f, burstAge / flashRise) *
                                    std::exp(-9.0f * std::max(0.0f, burstAge - flashRise));
        firework.flash->translate(firework.x * size().w, firework.y * size().h);
        firework.flash->opacity(uint8_t(190 * flashIntensity));
        // Fade the atmospheric light out completely over the burst's lifetime.
        const auto skyFade = std::clamp((burstAge - flashRise) / (firework.duration - flashRise), 0.0f, 1.0f);
        const auto skyOpacity = (50 + 25 * std::min(1.0f, burstAge / flashRise)) *
                                (1 - skyFade * skyFade * (3 - 2 * skyFade));
        // A zero-velocity spark follows the same gravitational fall as the burst's center.
        const auto skyPosition = position(firework, Spark{}, burstAge);
        firework.skyLight->translate(skyPosition.x, skyPosition.y);
        firework.skyLight->opacity(uint8_t(skyOpacity));
        const auto cooling = std::min(1.0f, burstAge / firework.duration);
        const auto r = uint8_t(firework.r + (255 - firework.r) * cooling);
        const auto g = uint8_t(firework.g + (145 - firework.g) * cooling);
        const auto b = uint8_t(firework.b + (45 - firework.b) * cooling);

        for (size_t depth = 0; depth < firework.layers.size(); ++depth) {
            auto& layer = firework.layers[depth];
            const auto projection = perspective(firework, (1 - int(depth)) * 0.16f, burstAge);
            const auto brightness = std::min(1.0f, projection * projection);
            for (size_t i = 0; i < layer.trails.size(); ++i) {
                layer.trails[i]->strokeFill(r, g, b, uint8_t((230 - 48 * i) * (1 - cooling * cooling) * brightness));
                layer.trails[i]->strokeWidth((2.4f - 0.4f * i) * scale * projection);
            }
            layer.core->fill(255, uint8_t(245 - 65 * cooling), uint8_t(225 - 140 * cooling));
            layer.core->opacity(uint8_t(255 * std::min(1.0f, (1 - cooling) * 3) * brightness));
        }
        for (const auto& spark : firework.sparks) {
            if (burstAge >= spark.life) continue;
            auto& layer = firework.layers[spark.vz > 0.06f ? 0 : (spark.vz < -0.06f ? 2 : 1)];
            const auto fade = 1.0f - burstAge / spark.life;
            const auto shimmer = 0.75f + 0.25f * std::sin(burstAge * 22 + spark.phase);
            const auto head = position(firework, spark, burstAge);
            const auto radius = (0.55f + 1.3f * fade) * scale * shimmer * perspective(firework, spark.vz, burstAge);
            layer.core->appendCircle(head.x, head.y, radius, radius);
            // Sample the curved trajectory into progressively dimmer tail segments.
            auto from = head;
            for (size_t i = 0; i < layer.trails.size(); ++i) {
                const auto time = std::max(0.0f, burstAge - (i + 1) * firework.trailStep * std::sqrt(fade));
                const auto to = position(firework, spark, time);
                layer.trails[i]->moveTo(from.x, from.y);
                layer.trails[i]->lineTo(to.x, to.y);
                from = to;
            }
        }
    }

    void drawCityGlow(tvg::Canvas* canvas)
    {
        const auto w = float(size().w);
        const auto h = float(size().h);
        auto haze = tvg::Shape::gen();
        haze->appendRect(0, 0, w, h);
        auto gradient = tvg::LinearGradient::gen();
        gradient->linear(0, h, 0, 0.38f * h);
        tvg::Fill::ColorStop stops[] = {
            {0, 185, 126, 75, 48},
            {0.25f, 155, 112, 85, 30},
            {0.60f, 85, 85, 115, 12},
            {1, 55, 65, 100, 0}
        };
        gradient->colorStops(stops, 4);
        haze->fill(gradient);
        canvas->add(haze);

        // Soft pools of light rise above the busiest parts of the city.
        for (float x : {0.24f, 0.53f, 0.81f}) {
            auto glow = tvg::Shape::gen();
            glow->appendRect(0, 0, w, h);
            auto radial = tvg::RadialGradient::gen();
            radial->radial(x * w, 0.94f * h, 0.36f * h, x * w, 0.94f * h, 0);
            tvg::Fill::ColorStop glowStops[] = {
                {0, 235, 166, 90, 32},
                {0.35f, 200, 136, 80, 20},
                {0.7f, 135, 100, 85, 7},
                {1, 100, 85, 90, 0}
            };
            radial->colorStops(glowStops, 4);
            glow->fill(radial);
            canvas->add(glow);
        }
    }

    void drawMountains(tvg::Canvas* canvas)
    {
        const auto w = float(size().w);
        const auto h = float(size().h);
        auto hills = tvg::Shape::gen();
        // Small, uneven ridges sit on top of the broader rolling hills.
        for (int i = 0; i <= 120; ++i) {
            const auto x = i / 120.0f;
            const auto y = 0.81f - 0.045f * std::sin(x * 9.0f + 0.2f)
                                - 0.024f * std::sin(x * 19.0f - 0.6f)
                                + 0.007f * std::sin(x * 83.0f + 0.8f)
                                + 0.004f * std::sin(x * 173.0f);
            if (i == 0) hills->moveTo(x * w, y * h);
            else hills->lineTo(x * w, y * h);
        }
        hills->lineTo(w, h);
        hills->lineTo(0, h);
        hills->close();
        hills->fill(7, 11, 18);
        canvas->add(hills);
    }

    void drawLandscape(tvg::Canvas* canvas)
    {
        const auto w = float(size().w);
        const auto h = float(size().h);
        // Batch buildings and windows into a few static paths for each depth layer.
        std::srand(73);
        for (int layer = 0; layer < 2; ++layer) {
            auto buildings = tvg::Shape::gen();
            buildings->fill(layer == 0 ? 11 : 6, layer == 0 ? 16 : 10, layer == 0 ? 27 : 19);
            std::array<tvg::Shape*, 3> windows;
            for (auto& window : windows) window = tvg::Shape::gen();
            windows[0]->fill(235, 184, 105, layer == 0 ? 65 : 125);
            windows[1]->fill(255, 220, 155, layer == 0 ? 85 : 165);
            windows[2]->fill(165, 191, 220, layer == 0 ? 55 : 95);

            const auto base = (layer == 0 ? 0.94f : 0.98f) * h;
            for (float x = -0.01f * w; x < w;) {
                const auto width = random(0.012f, 0.032f) * w;
                const auto height = random(0.035f, layer == 0 ? 0.13f : 0.19f) * h;
                const auto top = base - height;
                buildings->appendRect(x, top, width, h - top);
                if (random(0, 1) < 0.3f) {
                    buildings->appendRect(x + width * 0.3f, top - 0.008f * h, width * 0.4f, 0.009f * h);
                    if (height > 0.12f * h) {
                        buildings->appendRect(x + width * 0.5f, top - 0.025f * h, 0.0006f * w, 0.025f * h);
                    }
                }
                for (float wy = top + 0.009f * h; wy < base - 0.005f * h; wy += 0.012f * h) {
                    for (float wx = x + 0.003f * w; wx < x + width - 0.003f * w; wx += 0.0045f * w) {
                        if (random(0, 1) < 0.38f) {
                            windows[std::rand() % windows.size()]->appendRect(wx, wy, 0.0015f * w, 0.0035f * h);
                        }
                    }
                }
                x += width + random(0.002f, 0.006f) * w;
            }
            canvas->add(buildings);
            for (auto window : windows) canvas->add(window);
        }

        // A faint warm veil lights the lower facades without hiding the windows.
        auto streetHaze = tvg::Shape::gen();
        streetHaze->appendRect(0, 0.80f * h, w, 0.20f * h);
        auto streetGradient = tvg::LinearGradient::gen();
        streetGradient->linear(0, h, 0, 0.80f * h);
        tvg::Fill::ColorStop streetStops[] = {
            {0, 205, 144, 80, 24},
            {0.45f, 175, 120, 75, 12},
            {1, 140, 100, 75, 0}
        };
        streetGradient->colorStops(streetStops, 3);
        streetHaze->fill(streetGradient);
        canvas->add(streetHaze);

        auto foreground = tvg::Shape::gen();
        foreground->moveTo(0, 0.91f * h);
        foreground->cubicTo(0.15f * w, 0.87f * h, 0.22f * w, 0.99f * h, 0.45f * w, 0.98f * h);
        foreground->cubicTo(0.65f * w, 0.96f * h, 0.82f * w, 0.90f * h, w, 0.94f * h);
        foreground->lineTo(w, h);
        foreground->lineTo(0, h);
        foreground->close();
        foreground->fill(4, 8, 13);
        canvas->add(foreground);
        std::srand(100);
    }

    using tvgexam::Example::Example;

    bool content(tvg::Canvas* canvas, const tvg::toolkit::App::Size& size) override
    {
        auto sky = tvg::Shape::gen();
        sky->appendRect(0, 0, size.w, size.h);
        sky->fill(3, 5, 14);
        canvas->add(sky);

        // Build the star geometry once, behind the atmospheric light and fireworks.
        std::srand(42);
        const auto starScale = size.h / 1280.0f;
        for (auto& star : stars) {
            star.shape = tvg::Shape::gen();
            const auto x = random(0.005f, 0.995f) * size.w;
            const auto y = random(0.005f, 0.995f) * size.h;
            const auto weight = random(0, 1);
            const auto radius = (0.45f + 1.1f * weight * weight) * starScale;
            star.shape->appendCircle(x, y, radius, radius);
            const auto warmth = random(0, 1);
            star.shape->fill(uint8_t(195 + 60 * warmth), uint8_t(215 + 30 * warmth), 255);
            star.brightness = random(90, 230);
            star.speed = random(0.7f, 2.4f);
            star.phase = random(0, 6.2831853f);
            canvas->add(star.shape);
        }
        drawStars(0);
        std::srand(100);

        drawCityGlow(canvas);
        drawMountains(canvas);

        // Keep every atmospheric light behind all of the fireworks.
        for (auto& firework : fireworks) {
            auto light = tvg::Shape::gen();
            const auto radius = size.h * 0.315f;
            light->appendCircle(0, 0, radius, radius);
            light->opacity(0);
            light->blend(tvg::BlendMethod::Lighten);
            canvas->add(light);
            firework.skyLight = light;
        }

        for (size_t i = 0; i < fireworks.size(); ++i) {
            auto& firework = fireworks[i];
            firework.age = -float(i) * 0.85f;
            auto flash = tvg::Shape::gen();
            const auto radius = size.h * 0.095f;
            flash->appendCircle(0, 0, radius, radius);
            auto glow = tvg::RadialGradient::gen();
            glow->radial(0, 0, radius, 0, 0, 0);
            tvg::Fill::ColorStop stops[] = {{0, 255, 245, 220, 230},
                                          {0.15f, 255, 195, 100, 80},
                                          {1, 255, 140, 60, 0}};
            glow->colorStops(stops, 3);
            flash->fill(glow);
            flash->opacity(0);
            canvas->add(flash);
            firework.flash = flash;
            // Draw receding sparks first, with only a few paths per depth layer.
            for (auto& layer : firework.layers) {
                for (auto& trail : layer.trails) {
                    trail = tvg::Shape::gen();
                    trail->fill(0, 0, 0, 0);
                    trail->strokeCap(tvg::StrokeCap::Round);
                    trail->blend(tvg::BlendMethod::Add);
                    canvas->add(trail);
                }
                layer.core = tvg::Shape::gen();
                layer.core->blend(tvg::BlendMethod::Add);
                canvas->add(layer.core);
            }
        }

        // The foreground silhouettes occlude stars and falling sparks near the ground.
        drawLandscape(canvas);

        return true;
    }

    bool update(tvg::Canvas* canvas, size_t elapsed) override
    {
        tvgexam::Example::update(canvas, elapsed);

        const auto dt = elapsed >= previousElapsed ? (elapsed - previousElapsed) * 0.001f : 0.0f;
        previousElapsed = elapsed;

        drawStars(elapsed * 0.001f);
        for (auto& firework : fireworks) drawFirework(firework, dt);

        canvas->update();
        return true;
    }
};

/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    auto params = tvgexam::options(argc, argv, {2440, 1280});
    return tvgexam::run(new UserExample(params), params);
}
