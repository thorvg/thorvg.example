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

#include <memory>
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    std::unique_ptr<tvg::Animation> animation;
    std::string input = EXAMPLE_DIR "/lottie/sample.json";

    UserExample(const tvgexam::Params& params) : tvgexam::Example(params)
    {
        tvgexam::filepath(params, input);   // -i <filepath>
    }

    bool content(tvg::Canvas* canvas, const tvg::toolkit::App::Size& size) override
    {
        //The default font for fallback in case
        tvg::Text::load(EXAMPLE_DIR"/font/PublicSans-Regular.ttf");

        //Animation Controller
        animation = std::unique_ptr<tvg::Animation>(tvg::Animation::gen());
        auto picture = animation->picture();
        picture->origin(0.5f, 0.5f);  //center origin

        //Background
        auto shape = tvg::Shape::gen();
        shape->appendRect(0, 0, size.w, size.h);
        shape->fill(50, 50, 50);

        canvas->add(shape);

        if (!tvgexam::verify(picture->load(input.c_str()))) return false;

        //image scaling preserving its aspect ratio
        float w2, h2;
        picture->size(&w2, &h2);
        auto scale = (w2 > h2) ? size.w / w2 : size.h / h2;
        picture->scale(scale);
        picture->translate(float(size.w) * 0.5f, float(size.h) * 0.5f);

        canvas->add(picture);

        return true;
    }

    bool update(tvg::Canvas* canvas, size_t elapsed) override
    {
        tvgexam::Example::update(canvas, elapsed);
        auto progress = tvg::toolkit::progress(elapsed, animation->duration());

        //Update animation frame only when it's changed
        if (animation->frame(animation->totalFrame() * progress) == tvg::Result::Success) {
            canvas->update();
            return true;
        }

        return false;
    }
};

/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    auto params = tvgexam::options(argc, argv, {1024, 1024});
    return tvgexam::run(new UserExample(params), params);
}