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

#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        //Add random half-translucent rects with stroking
        for (int i = 0; i < 2000; ++i) {
            auto s = tvg::Shape::gen();
            s->appendRect(rand() % 1600, rand() % 1600, rand() % 100, rand() % 100, rand() % 10, rand() % 10);
            s->fill(rand() % 255, rand() % 255, rand() % 255, 253 + rand() % 3);
            s->strokeFill(255, 255, 255);
            s->strokeWidth(3);
            canvas->add(s);
        }
        return true;
    }

    bool update(tvg::Canvas* canvas, uint32_t elapsed) override
    {
        //change rects' pos and size
        for (auto p : canvas->paints()) {
            auto s = static_cast<tvg::Shape*>(p);
            s->reset();
            s->appendRect(rand() % 1600, rand() % 1600, rand() % 100, rand() % 100, rand() % 10, rand() % 10);
        }
        return canvas->update() == tvg::Result(0);
    }
};

/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    return tvgexam::main(new UserExample, argc, argv, true, 1650, 1650, 4, true);
}