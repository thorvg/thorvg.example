/*
 * Copyright (c) 2026 ThorVG project. All rights reserved.

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

#include <cmath>
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    using tvgexam::Example::Example;

    tvg::Shape* circles[3];
    tvg::Point distance = {0.0f, 0.0f};

    UserExample(const tvgexam::Params& params) : tvgexam::Example(params, true) {}

    bool content(tvg::Canvas* canvas, const tvg::toolkit::App::Size& size) override
    {
        auto radius = float(size.w < size.h ? size.w : size.h) * 0.1f;
        distance = {float(size.w) - 2.0f * radius, float(size.h) - 2.0f * radius};

        // horizontal
        {
            circles[0] = tvg::Shape::gen();
            circles[0]->appendCircle(radius, radius, radius, radius);
            circles[0]->fill(255, 255, 0);

            // motion blur effect scene
            auto scene = tvg::Scene::gen();
            // distance, angle, border, quality
            tvgexam::verify(scene->add(tvg::SceneEffect::MotionBlur, double(distance.x) * 0.1, 0.0, 0, 100));
            scene->add(circles[0]);
            canvas->add(scene);
        }

        // vertical
        {
            circles[1] = tvg::Shape::gen();
            circles[1]->appendCircle(float(size.w) * 0.5f, radius, radius, radius);
            circles[1]->fill(0, 255, 255);

            // motion blur effect scene
            auto scene = tvg::Scene::gen();
            scene->add(tvg::SceneEffect::MotionBlur, double(distance.y) * 0.1, 90.0, 0, 100);
            scene->add(circles[1]);
            canvas->add(scene);
        }

        // diagonal
        {
            circles[2] = tvg::Shape::gen();
            circles[2]->appendCircle(float(size.w) - radius, float(size.h) - radius, radius, radius);
            circles[2]->fill(255, 0, 255);

            // motion blur effect scene
            auto scene = tvg::Scene::gen();
            auto dist = std::hypot(distance.x, distance.y) * 0.1f;
            auto angle = std::atan2(distance.x, distance.y) * 180.0f / M_PI;
            tvgexam::verify(scene->add(tvg::SceneEffect::MotionBlur, dist, angle, 0, 100));

            scene->add(circles[2]);
            canvas->add(scene);
        }

        return true;
    }

    bool update(tvg::Canvas* canvas, size_t elapsed) override
    {
        tvgexam::Example::update(canvas, elapsed);

        auto progress = tvg::toolkit::progress(elapsed, 1.0f, true);

        circles[0]->translate(distance.x * progress, 0.0f);
        circles[1]->translate(0.0f, distance.y * progress);
        circles[2]->translate(-distance.x * progress, -distance.y * progress);

        canvas->update();

        return true;
    }
};

/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    auto params = tvgexam::options(argc, argv, {800, 800});
    return tvgexam::run(new UserExample(params), params);
}
