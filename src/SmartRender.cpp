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
#include <vector>
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    struct Particle {
        tvg::Paint* obj;
        float x, y;
        float speed;
        float size;
    };

    const float COUNT = 1200.0f;
    std::vector<Particle> raindrops;
    std::vector<Particle> clouds;

    uint32_t w, h;

    using tvgexam::Example::Example;

    bool content(tvg::Canvas* canvas, const tvg::toolkit::App::Size& size) override
    {
        w = size.w;
        h = size.h;
        std::srand(100);

        auto city = tvg::Picture::gen();
        city->load(EXAMPLE_DIR"/image/particle.jpg");
        canvas->add(city);

        auto cloud1 = tvg::Picture::gen();
        cloud1->load(EXAMPLE_DIR"/image/clouds.png");
        cloud1->opacity(60);
        canvas->add(cloud1);

        float cloudWidth;
        cloud1->size(&cloudWidth, nullptr);
        clouds.push_back({cloud1, 0, 0, 0.25f, cloudWidth});

        auto cloud2 = cloud1->duplicate();
        cloud2->opacity(30);
        cloud2->translate(400, 100);
        canvas->add(cloud2);

        clouds.push_back({cloud2, 400, 100, 0.125f, cloudWidth});

        auto cloud3 = cloud1->duplicate();
        cloud3->opacity(20);
        cloud3->translate(1200, 200);
        canvas->add(cloud3);

        clouds.push_back({cloud3, 1200, 200, 0.075f, cloudWidth});

        auto darkness = tvg::Shape::gen();
        darkness->appendRect(0, 0, w, h);
        darkness->fill(0, 0, 0, 150);
        canvas->add(darkness);

        //rain drops
        auto spacing = w / COUNT;
        raindrops.reserve(COUNT);

        for (int i = 0; i < COUNT; ++i) {
            auto shape = tvg::Shape::gen();
            float x = spacing * i;
            raindrops.push_back({shape, x, float(std::rand()%h), 10 + float(std::rand() % 100) * 0.1f, 0 /* unused */});
            shape->appendRect(0, 0, 1, std::rand() % 15 + spacing);
            shape->fill(255, 255, 255, 55 + std::rand() % 100);
            canvas->add(shape);
        }

        return true;
    }

    bool update(tvg::Canvas* canvas, size_t elapsed) override
    {
        tvgexam::Example::update(canvas, elapsed);
        for (auto& p : raindrops) {
            p.y += p.speed;
            if (p.y > h) {
                p.y -= h;
            }
            p.obj->translate(p.x, p.y);
        }

        for (auto& p : clouds) {
            p.x -= p.speed;
            if (p.x + p.size < 0) {
                p.x = w;
            }
            p.obj->translate(p.x, p.y);
        }

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
    params.threads = 0;  // TODO: Single-threaded rendering is faster for this example... ?
    return tvgexam::run(new UserExample(params), params);
}
