/*
 * Copyright (c) 2023 - 2026 ThorVG project. All rights reserved.

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
#include <cmath>
#include <vector>
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

#define NUM_PER_ROW 10
#define NUM_PER_COL 10

struct UserExample : tvgexam::Example
{
    using tvgexam::Example::Example;

    struct Image
    {
        tvg::Picture* picture;
        float x, y;
    };
    std::vector<Image> images;

    bool update(tvg::Canvas* canvas, size_t elapsed) override
    {
        tvgexam::Example::update(canvas, elapsed);
        const auto offset = 10.0f * std::sin((elapsed % 2000) * (2.0f * 3.14159265f / 2000.0f));
        for (auto& image : images) {
            image.picture->translate(image.x + offset, image.y);
        }
        canvas->update();
        return true;
    }

    bool content(tvg::Canvas* canvas, const tvg::toolkit::App::Size& size) override
    {
        //Background
        auto shape = tvg::Shape::gen();
        shape->appendRect(0, 0, size.w, size.h);
        shape->fill(75, 75, 75);
        canvas->add(shape);

        const auto cellWidth = size.w / float(NUM_PER_ROW);
        const auto cellHeight = size.h / float(NUM_PER_COL);

        for (int i = 0; i < NUM_PER_ROW * NUM_PER_COL; ++i) {
            auto picture = tvg::Picture::gen();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR "/image/test.jpg"))) return false;

            float w, h;
            picture->size(&w, &h);
            picture->origin(0.5f, 0.5f);
            picture->scale(std::min(cellWidth / w, cellHeight / h));
            const auto x = (i % NUM_PER_ROW + 0.5f) * cellWidth;
            const auto y = (i / NUM_PER_ROW + 0.5f) * cellHeight;
            picture->translate(x, y);
            canvas->add(picture);
            images.push_back({picture, x, y});
        }

        return true;
    }
};

/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    auto params = tvgexam::options(argc, argv, {1280, 1280});
    return tvgexam::run(new UserExample(params), params);
}
