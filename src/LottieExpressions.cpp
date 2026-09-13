/*
 * Copyright (c) 2024 - 2026 ThorVG project. All rights reserved.

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

#include <memory>
#include <vector>
#include <thorvg-1/thorvg_lottie.h>
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

#define NUM_PER_ROW 6
#define NUM_PER_COL 6

struct UserExample : tvgexam::Example
{
    std::vector<std::unique_ptr<tvg::Animation>> animations;
    int counter = 0;

    using tvgexam::Example::Example;

    void populate(const char* path) override
    {
        if (counter >= NUM_PER_ROW * NUM_PER_COL) return;

        //ignore if not lottie.
        const auto ext = path + std::strlen(path) - 4;
        if (std::strcmp(ext, "json") && std::strcmp(ext, "lot")) return;

        //Animation Controller
        auto animation = tvg::Animation::gen();
        auto picture = animation->picture();
        picture->origin(0.5f, 0.5f);

        if (!tvgexam::verify(picture->load(path))) return;

        //image scaling preserving its aspect ratio
        const auto& size = this->size();
        auto cellSize = size.w / NUM_PER_ROW;
        float w, h;
        picture->size(&w, &h);
        picture->scale((w > h) ? cellSize / w : cellSize / h);
        picture->translate((counter % NUM_PER_ROW) * cellSize + cellSize / 2, (counter / NUM_PER_ROW) * (size.h / NUM_PER_COL) + cellSize / 2);

        animations.push_back(std::unique_ptr<tvg::Animation>(animation));

        std::cout << "Lottie: " << path << std::endl;

        counter++;
    }

    bool update(tvg::Canvas* canvas, size_t elapsed) override
    {
        tvgexam::Example::update(canvas, elapsed);
        for (auto& animation : animations) {
            auto progress = tvg::toolkit::progress(elapsed, animation->duration());
            animation->frame(animation->totalFrame() * progress);
        }

        canvas->update();

        return true;
    }

    bool content(tvg::Canvas* canvas, const tvg::toolkit::App::Size& size) override
    {
        //The default font for fallback in case
        tvg::Text::load(EXAMPLE_DIR"/font/PublicSans-Regular.ttf");

        //Background
        auto shape = tvg::Shape::gen();
        shape->appendRect(0, 0, size.w, size.h);
        shape->fill(75, 75, 75);

        canvas->add(shape);

        this->scandir(EXAMPLE_DIR"/lottie/expressions");

        //Run animation loop
        for (auto& animation : animations) {
            canvas->add(animation->picture());
        }

        return true;
    }
};

/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    if (!tvg::LottieAnimation::expressions()) {
        std::cout << "Lottie expressions are not supported in this build." << std::endl;
        return 0;
    }

    auto params = tvgexam::options(argc, argv, {1024, 1024});
    return tvgexam::run(new UserExample(params), params);
}
