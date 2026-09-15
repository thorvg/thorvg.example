/*
 * Copyright (c) 2020 - 2026 ThorVG project. All rights reserved.

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
#include <chrono>
#include <random>
#include <vector>
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    using tvgexam::Example::Example;

    std::vector<tvg::Paint*> objects;
    struct Bounds
    {
        float cx, cy, halfW, halfH;
    };
    std::vector<Bounds> bounds;
    struct Body
    {
        double x = 0, y = 0, vx = 0, vy = 0, angle = 0, spin = 0;
        double gravity = 0;
        bool active = false, landed = false, sleeping = false;
        double restTime = 0;
    };
    std::vector<Body> bodies;
    size_t lastElapsed = 0;
    double accumulator = 0, simulationTime = 0, nextSpawn = 0;
    std::mt19937 random{std::random_device{}()};
    bool filled = false;
    size_t generationLimit = 0;

    bool cursorActive = false;
    tvg::Shape* cursorBox = nullptr;
    double cursorX = 0, cursorY = 0;
    std::chrono::steady_clock::time_point cursorTime;

    void cursorCollide(double x, double y, double vx, double vy, bool moving)
    {
        const auto halfBoxW = size().w / 32.0;
        const auto halfBoxH = size().h / 32.0;
        for (size_t i = 0; i < bodies.size(); ++i) {
            auto& b = bodies[i];
            const auto e = extent(i);
            const auto dx = b.x - x, dy = b.y - y;
            const auto overlapX = halfBoxW + e[0] - std::abs(dx);
            const auto overlapY = halfBoxH + e[1] - std::abs(dy);
            if (overlapX <= 0 || overlapY <= 0) continue;
            const auto nx = overlapX < overlapY ? (dx >= 0 ? 1.0 : -1.0) : 0.0;
            const auto ny = overlapX < overlapY ? 0.0 : (dy >= 0 ? 1.0 : -1.0);
            const auto penetration = std::min(overlapX, overlapY) + 0.1;
            b.x += nx * penetration;
            b.y += ny * penetration;
            b.sleeping = false;
            b.restTime = 0;
            if (moving) {
                const auto speed = std::hypot(vx, vy);
                const auto kick = std::min(950.0, 220.0 + speed * 0.45);
                // Spread objects away from the box and carry some cursor momentum.
                const auto length = std::max(std::hypot(dx, dy), 1.0);
                b.vx = std::clamp(nx * kick + dx / length * kick * 0.35 + vx * 0.25, -1400.0, 1400.0);
                b.vy = std::clamp(ny * kick + dy / length * kick * 0.35 + vy * 0.25, -1400.0, 1400.0);
                b.spin = std::clamp(b.spin + (nx * vy - ny * vx) * 0.01 + (dx * b.vy - dy * b.vx) * 0.003, -12.0, 12.0);
            } else {
                const auto approach = b.vx * nx + b.vy * ny;
                if (approach < 0) {
                    b.vx -= 1.45 * approach * nx;
                    b.vy -= 1.45 * approach * ny;
                }
            }
        }
    }

    bool motion(tvg::Canvas*, int32_t x, int32_t y) override
    {
        const auto now = std::chrono::steady_clock::now();
        if (x < 0 || y < 0 || x >= int32_t(size().w) || y >= int32_t(size().h)) {
            cursorActive = false;
            return false;
        }
        if (cursorActive) {
            const auto dx = x - cursorX, dy = y - cursorY;
            const auto seconds = std::max(std::chrono::duration<double>(now - cursorTime).count(), 1.0 / 240.0);
            const auto vx = std::clamp(dx / seconds, -2000.0, 2000.0);
            const auto vy = std::clamp(dy / seconds, -2000.0, 2000.0);
            // Sweep the whole path so a fast cursor cannot skip over objects.
            const int segments = std::max(1, int(std::ceil(std::hypot(dx, dy) / 10.0)));
            if (dx != 0 || dy != 0) {
                for (int i = 1; i <= segments; ++i) {
                    cursorCollide(cursorX + dx * i / segments, cursorY + dy * i / segments, vx, vy, true);
                }
            }
        } else {
            cursorCollide(x, y, 0, 0, true);
        }
        cursorX = x;
        cursorY = y;
        cursorTime = now;
        cursorActive = true;
        return false;
    }

    bool spawn(tvg::Canvas* canvas)
    {
        static const char* letters[] = {
            u8"가", u8"나", u8"다", u8"라", u8"마", u8"바", u8"사",
            u8"아", u8"자", u8"차", u8"카", u8"타", u8"파", u8"하"};
        static constexpr uint8_t colors[][3] = {
            {230, 55, 75}, {240, 110, 30}, {210, 155, 15}, {70, 165, 55}, {15, 155, 120}, {15, 150, 180}, {40, 100, 225}, {90, 65, 220}, {150, 55, 210}, {210, 50, 160}, {245, 75, 120}, {25, 125, 80}};
        const auto& color = colors[std::uniform_int_distribution<int>(0, 11)(random)];
        const auto objectScale = std::min(size().w, size().h) / 1600.0f;
        const auto objectSize = std::uniform_int_distribution<int>(20, 60)(random) * objectScale;
        tvg::Paint* object;
        if (std::bernoulli_distribution(0.5)(random)) {
            auto text = tvg::Text::gen();
            text->font("NanumGothicCoding");
            text->size(objectSize);
            text->text(letters[std::uniform_int_distribution<int>(0, 13)(random)]);
            text->fill(color[0], color[1], color[2]);
            object = text;
        } else {
            auto shape = tvg::Shape::gen();
            // Choose equally between triangle, quadrilateral, pentagon, star and circle.
            const auto kind = std::uniform_int_distribution<int>(0, 4)(random);
            const auto radius = objectSize * 0.5f;
            if (kind == 4) {
                shape->appendCircle(0, 0, radius, radius);
            } else {
                const bool star = kind == 3;
                const auto vertices = star ? 10 : kind + 3;
                constexpr double pi = 3.14159265358979323846;
                for (int i = 0; i < vertices; ++i) {
                    const auto angle = 2.0 * pi * i / vertices - pi * 0.5;
                    const auto vertexRadius = star && i % 2 != 0 ? radius * 0.45f : radius;
                    const auto x = float(vertexRadius * std::cos(angle));
                    const auto y = float(vertexRadius * std::sin(angle));
                    if (i == 0) shape->moveTo(x, y);
                    else shape->lineTo(x, y);
                }
                shape->close();
            }
            shape->fill(color[0], color[1], color[2]);
            object = shape;
        }
        float x, y, w, h;
        if (!tvgexam::verify(object->bounds(&x, &y, &w, &h))) return false;
        Bounds b{x + w * 0.5f, y + h * 0.5f, w * 0.5f, h * 0.5f};
        Body body;
        body.x = std::uniform_real_distribution<double>(w * 0.5, size().w - w * 0.5)(random);
        body.y = -h * 0.5;
        body.gravity = 1100;
        body.active = true;
        object->translate(float(body.x - b.cx), float(body.y - b.cy));
        if (!tvgexam::verify(canvas->add(object, cursorBox))) return false;
        objects.push_back(object);
        bounds.push_back(b);
        bodies.push_back(body);
        return true;
    }

    bool targetFilled()
    {
        //Reserve room for objects still falling or bouncing before spawning more.
        std::array<double, 32> heights{};
        const auto floorY = double(size().h);
        double pendingArea = 0;
        double generatedArea = 0;
        for (size_t i = 0; i < bodies.size(); ++i) {
            const auto& b = bodies[i];
            const auto area = 4.0 * bounds[i].halfW * bounds[i].halfH;
            generatedArea += area;
            if (!b.sleeping) {
                pendingArea += area;
                continue;
            }
            const auto e = extent(i);
            const auto first = std::clamp(int((b.x - e[0]) * 32 / size().w), 0, 31);
            const auto last = std::clamp(int((b.x + e[0]) * 32 / size().w), 0, 31);
            for (int column = first; column <= last; ++column)
                heights[column] = std::max(heights[column], floorY - (b.y - e[1]));
        }
        double total = 0;
        for (auto height : heights)
            total += height;
        constexpr double packing = 0.55;
        const auto reservedHeight = pendingArea / (size().w * packing);
        const auto targetHeight = floorY * 0.7;
        const auto peakHeight = *std::max_element(heights.begin(), heights.end());
        return total / heights.size() + reservedHeight >= targetHeight ||
               peakHeight + reservedHeight >= targetHeight ||
               generatedArea >= size().w * targetHeight * packing;
    }

    std::array<double, 2> extent(size_t i) const
    {
        const auto c = std::abs(std::cos(bodies[i].angle));
        const auto s = std::abs(std::sin(bodies[i].angle));
        return {c * bounds[i].halfW + s * bounds[i].halfH,
                s * bounds[i].halfW + c * bounds[i].halfH};
    }

    void constrain(size_t i)
    {
        auto& body = bodies[i];
        const auto e = extent(i);
        const auto floorY = double(size().h);
        if (body.y + e[1] >= floorY) {
            body.y = floorY - e[1];
            if (body.vy > 0) {
                const auto impact = body.vy;
                body.vy = impact > 50 ? -impact * 0.45 : 0;
                if (!body.landed) {
                    body.vx += (i % 2 == 0 ? 1 : -1) * 120;
                    body.spin = (i % 2 == 0 ? 1 : -1) * 5.2;
                    body.landed = true;
                } else if (impact > 50) {
                    body.vx *= -0.65;
                    body.spin *= -0.65;
                } else {
                    body.vx *= 0.94;
                    body.spin *= 0.9;
                    if (std::abs(body.vx) < 1) body.vx = 0;
                    if (std::abs(body.spin) < 0.02) body.spin = 0;
                }
            }
        }
        if (body.x < e[0]) {
            body.x = e[0];
            if (body.vx < 0) {
                body.vx *= -0.7;
                body.spin *= -0.7;
            }
        } else if (body.x > size().w - e[0]) {
            body.x = size().w - e[0];
            if (body.vx > 0) {
                body.vx *= -0.7;
                body.spin *= -0.7;
            }
        }
    }

    void step(double dt)
    {
        simulationTime += dt;
        for (size_t i = 0; i < bodies.size(); ++i) {
            auto& b = bodies[i];
            if (!b.active || b.sleeping) continue;
            if (b.landed && std::abs(b.vy) < 100 && std::abs(b.vx) < 40 && std::abs(b.spin) < 0.3) {
                b.restTime += dt;
                if (b.restTime > 0.3) {
                    b.sleeping = true;
                    b.vx = b.vy = b.spin = 0;
                    continue;
                }
            } else b.restTime = 0;
            b.vy += b.gravity * dt;
            b.x += b.vx * dt;
            b.y += b.vy * dt;
            b.angle += b.spin * dt;
        }

        // Resolve overlapping rotated bounding boxes and exchange normal momentum.
        for (int pass = 0; pass < 4; ++pass) {
            // Only compare objects in the same spatial cell as the pile grows.
            const int columns = std::max(1, int(std::ceil(size().w / 100.0)));
            const int rows = std::max(1, int(std::ceil(size().h / 100.0)));
            std::vector<std::vector<size_t>> cells(columns * rows);
            std::vector<std::array<int, 2>> starts(bodies.size());
            for (size_t i = 0; i < bodies.size(); ++i) {
                const auto e = extent(i);
                const auto& b = bodies[i];
                const int x0 = std::clamp(int(std::floor((b.x - e[0]) / 100)), 0, columns - 1);
                const int y0 = std::clamp(int(std::floor((b.y - e[1]) / 100)), 0, rows - 1);
                const int x1 = std::clamp(int(std::floor((b.x + e[0]) / 100)), 0, columns - 1);
                const int y1 = std::clamp(int(std::floor((b.y + e[1]) / 100)), 0, rows - 1);
                starts[i] = {x0, y0};
                for (int y = y0; y <= y1; ++y)
                    for (int x = x0; x <= x1; ++x)
                        cells[y * columns + x].push_back(i);
            }
            for (int cell = 0; cell < columns * rows; ++cell) {
                const auto& members = cells[cell];
                for (size_t first = 0; first < members.size(); ++first) {
                    const auto i = members[first];
                    for (size_t second = first + 1; second < members.size(); ++second) {
                        const auto j = members[second];
                        if (cell % columns != std::max(starts[i][0], starts[j][0]) || cell / columns != std::max(starts[i][1], starts[j][1])) continue;
                        auto& a = bodies[i];
                        auto& b = bodies[j];
                        if (!b.active || (a.sleeping && b.sleeping)) continue;
                        const auto ea = extent(i), eb = extent(j);
                        const auto dx = b.x - a.x, dy = b.y - a.y;
                        const auto overlapX = ea[0] + eb[0] - std::abs(dx);
                        const auto overlapY = ea[1] + eb[1] - std::abs(dy);
                        if (overlapX <= 0 || overlapY <= 0) continue;
                        const auto nx = overlapX < overlapY ? (dx >= 0 ? 1.0 : -1.0) : 0.0;
                        const auto ny = overlapX < overlapY ? 0.0 : (dy >= 0 ? 1.0 : -1.0);
                        const double massA = a.sleeping ? 0.0 : 1.0;
                        const double massB = b.sleeping ? 0.0 : 1.0;
                        const auto correction = (std::min(overlapX, overlapY) + 0.01) / (massA + massB);
                        a.x -= nx * correction * massA;
                        a.y -= ny * correction * massA;
                        b.x += nx * correction * massB;
                        b.y += ny * correction * massB;
                        // Use the middle of the overlapping contact face as the impact point.
                        const auto contactX = 0.5 * (std::max(a.x - ea[0], b.x - eb[0]) + std::min(a.x + ea[0], b.x + eb[0]));
                        const auto contactY = 0.5 * (std::max(a.y - ea[1], b.y - eb[1]) + std::min(a.y + ea[1], b.y + eb[1]));
                        const auto ax = contactX - a.x, ay = contactY - a.y;
                        const auto bx = contactX - b.x, by = contactY - b.y;
                        const auto inertiaA = massA * 3.0 / (bounds[i].halfW * bounds[i].halfW + bounds[i].halfH * bounds[i].halfH);
                        const auto inertiaB = massB * 3.0 / (bounds[j].halfW * bounds[j].halfW + bounds[j].halfH * bounds[j].halfH);
                        const auto normalA = ax * ny - ay * nx;
                        const auto normalB = bx * ny - by * nx;
                        const auto relativeX = b.vx - b.spin * by - a.vx + a.spin * ay;
                        const auto relativeY = b.vy + b.spin * bx - a.vy - a.spin * ax;
                        const auto approach = relativeX * nx + relativeY * ny;
                        if (approach < 0) {
                            const auto denominator = massA + massB + normalA * normalA * inertiaA + normalB * normalB * inertiaB;
                            const auto impulse = -(1.0 + (approach < -50 ? 0.45 : 0.0)) * approach / denominator;
                            a.vx -= impulse * nx * massA;
                            a.vy -= impulse * ny * massA;
                            b.vx += impulse * nx * massB;
                            b.vy += impulse * ny * massB;
                            a.spin -= normalA * impulse * inertiaA;
                            b.spin += normalB * impulse * inertiaB;

                            // Coulomb friction couples sliding motion to rotation at contact.
                            const auto tx = -ny, ty = nx;
                            const auto tangentA = ax * ty - ay * tx;
                            const auto tangentB = bx * ty - by * tx;
                            const auto slip = (b.vx - b.spin * by - a.vx + a.spin * ay) * tx + (b.vy + b.spin * bx - a.vy - a.spin * ax) * ty;
                            const auto frictionMass = massA + massB + tangentA * tangentA * inertiaA + tangentB * tangentB * inertiaB;
                            const auto friction = std::clamp(-slip / frictionMass, -0.4 * impulse, 0.4 * impulse);
                            a.vx -= friction * tx * massA;
                            a.vy -= friction * ty * massA;
                            b.vx += friction * tx * massB;
                            b.vy += friction * ty * massB;
                            a.spin -= tangentA * friction * inertiaA;
                            b.spin += tangentB * friction * inertiaB;
                            a.spin = std::clamp(a.spin, -12.0, 12.0);
                            b.spin = std::clamp(b.spin, -12.0, 12.0);
                            if (ny > 0 && b.landed) a.landed = true;
                            if (ny < 0 && a.landed) b.landed = true;
                            if (std::abs(approach) < 80) {
                                a.spin *= 0.96;
                                b.spin *= 0.96;
                            }
                        }
                    }
                }
            }
            if (cursorActive) cursorCollide(cursorX, cursorY, 0, 0, false);
            for (size_t i = 0; i < bodies.size(); ++i) {
                if (bodies[i].active) constrain(i);
            }
        }
    }

    bool content(tvg::Canvas* canvas, const tvg::toolkit::App::Size& size) override
    {
        if (!tvgexam::verify(tvg::Text::load(EXAMPLE_DIR "/font/NanumGothicCoding.ttf"))) return false;
        auto background = tvg::Shape::gen();
        background->appendRect(0, 0, size.w, size.h);
        background->fill(255, 255, 255);
        canvas->add(background);

        cursorBox = tvg::Shape::gen();
        cursorBox->appendRect(-49, -49, 98, 98);
        cursorBox->fill(45, 115, 235, 40);
        cursorBox->strokeFill(30, 95, 220);
        cursorBox->strokeWidth(2);
        cursorBox->opacity(0);
        if (!tvgexam::verify(canvas->add(cursorBox))) return false;

        return true;
    }

    bool update(tvg::Canvas* canvas, size_t elapsed) override
    {
        tvgexam::Example::update(canvas, elapsed);

        // Small fixed steps prevent fast objects from passing through each other.
        constexpr size_t startDelay = 5000;
        if (elapsed >= startDelay) {
            if (objects.empty()) {
                if (!spawn(canvas)) return false;
                nextSpawn = simulationTime + 0.035;
            }
            accumulator += std::min(double(elapsed - std::max(lastElapsed, startDelay)) / 1000.0, 0.1);
        }
        lastElapsed = elapsed;
        constexpr double dt = 1.0 / 240.0;
        while (accumulator >= dt) {
            step(dt);
            if (!filled && simulationTime >= nextSpawn) {
                if (generationLimit == 0 && targetFilled()) {
                    generationLimit = size_t(std::ceil(std::ceil(std::ceil(bodies.size() * 1.1) * 1.25) * 1.5));
                }
                filled = generationLimit != 0 && bodies.size() >= generationLimit;
                if (!filled && !spawn(canvas)) return false;
                nextSpawn = simulationTime + 0.035;
            }
            accumulator -= dt;
        }

        for (size_t i = 0; i < objects.size(); ++i) {
            const auto& b = bounds[i];
            const auto& body = bodies[i];
            const auto c = std::cos(body.angle), s = std::sin(body.angle);
            tvg::Matrix matrix = {
                float(c), float(-s), float(body.x - c * b.cx + s * b.cy),
                float(s), float(c), float(body.y - s * b.cx - c * b.cy),
                0, 0, 1};
            if (!tvgexam::verify(objects[i]->transform(matrix))) return false;
        }

        cursorBox->opacity(cursorActive ? 255 : 0);
        if (cursorActive) {
            const tvg::Matrix boxTransform = {
                size().w / 1600.0f, 0, float(cursorX),
                0, size().h / 1600.0f, float(cursorY),
                0, 0, 1
            };
            if (!tvgexam::verify(cursorBox->transform(boxTransform))) return false;
        }
        return tvgexam::verify(canvas->update());
    }
};

/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char** argv)
{
    auto params = tvgexam::options(argc, argv, {1600, 1600});
    return tvgexam::run(new UserExample(params), params);
}
