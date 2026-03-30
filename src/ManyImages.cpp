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

#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

static constexpr int CANVAS_SIZE = 1650;
static constexpr int PIC_COUNT = 500;

struct UserExample : tvgexam::Example
{
    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        for (int i = 0; i < PIC_COUNT; ++i) {
            auto p = tvg::Picture::gen();
            p->load(EXAMPLE_DIR"/image/test.png");
            p->origin(0.5f, 0.5f);
            p->translate(rand() % CANVAS_SIZE, rand() % CANVAS_SIZE);
            p->scale(float(10 + rand() % 90) * 0.005f);
            p->rotate(float(rand() % 360));
            canvas->add(p);
        }
        return true;
    }

    bool update(tvg::Canvas* canvas, uint32_t elapsed) override
    {
        for (auto i : canvas->paints()) {
            auto p = static_cast<tvg::Picture*>(i);
            p->translate(rand() % 1650, rand() % 1650);
            p->scale(float(10 + rand() % 90) * 0.005f);
            p->rotate(float(rand() % 360));
        }
        return canvas->update() == tvg::Result(0);
    }
};

/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    return tvgexam::main(new UserExample, argc, argv, true, CANVAS_SIZE, CANVAS_SIZE, 4, true);
}