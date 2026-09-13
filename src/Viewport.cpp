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

#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    static constexpr uint32_t VPORT_SIZE = 300;
    tvg::Picture* picture = nullptr;

    UserExample(const tvgexam::Params& params) : tvgexam::Example(params, true) {}

    bool content(tvg::Canvas* canvas, const tvg::toolkit::App::Size& size) override
    {
        //set viewport before canvas become dirty.
        if (!tvgexam::verify(canvas->viewport(0, 0, VPORT_SIZE, VPORT_SIZE))) return false;

        auto mask = tvg::Shape::gen();
        mask->appendCircle(size.w / 2, size.h / 2, size.w / 2, size.h / 2);
        mask->fill(255, 255, 255);
        //Use the opacity for a half-translucent mask.
        mask->opacity(125);

        picture = tvg::Picture::gen();
        if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/svg/tiger.svg"))) return false;
        picture->size(size.w, size.h);
        picture->mask(mask, tvg::MaskMethod::Alpha);
        canvas->add(picture);

        return true;
    }

    bool update(tvg::Canvas* canvas, size_t elapsed) override
    {
        tvgexam::Example::update(canvas, elapsed);
        auto progress = tvg::toolkit::progress(elapsed, 2.0f, true);  // play time 2 sec.

        auto& size = this->size();

        if (!tvgexam::verify(canvas->viewport((size.w - VPORT_SIZE) * progress, (size.h - VPORT_SIZE) * progress, VPORT_SIZE, VPORT_SIZE))) return false;

        canvas->update();

        return true;
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