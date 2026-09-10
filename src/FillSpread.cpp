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
    void add(tvg::Canvas* canvas, tvg::Shape* shape, float x, float y, float r)
    {
        auto rotated = shape->duplicate();
        //Rotate around the square center and move down to the second row.
        rotated->transform({0, -1, x + y + 2.0f * r, 1,  0, y - x + 2.0f * r + 20.0f, 0,  0, 1});
        canvas->add(shape);
        canvas->add(rotated);
    }

    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        const int colorCnt = 4;
        tvg::Fill::ColorStop colorStops[colorCnt];
        colorStops[0] = {0.0f, 127, 39, 255, 255};
        colorStops[1] = {0.33f, 159, 112, 253, 255};
        colorStops[2] = {0.66f, 253, 191, 96, 255};
        colorStops[3] = {1.0f, 255, 137, 17, 255};

        //Radial grad
        {
            float x1, y1 = 80.0f, r = 120.0f;

            //Pad
            x1 = 20.0f;
            auto shape1 = tvg::Shape::gen();
            shape1->appendRect(x1, y1, 2.0f * r, 2.0f * r);

            auto fill1 = tvg::RadialGradient::gen();
            fill1->radial(x1 + r, y1 + r, 40.0f, x1 + r, y1 + r, 0.0f);
            fill1->colorStops(colorStops, colorCnt);
            fill1->spread(tvg::FillSpread::Pad);
            shape1->fill(fill1);

            add(canvas, shape1, x1, y1, r);

            //Reflect
            x1 = 280.0f;
            auto shape2 = tvg::Shape::gen();
            shape2->appendRect(x1, y1, 2.0f * r, 2.0f * r);

            auto fill2 = tvg::RadialGradient::gen();
            fill2->radial(x1 + r, y1 + r, 40.0f, x1 + r, y1 + r, 0.0f);
            fill2->colorStops(colorStops, colorCnt);
            fill2->spread(tvg::FillSpread::Reflect);
            shape2->fill(fill2);

            add(canvas, shape2, x1, y1, r);

            //Repeat
            x1 = 540.0f;
            auto shape3 = tvg::Shape::gen();
            shape3->appendRect(x1, y1, 2.0f * r, 2.0f * r);

            auto fill3 = tvg::RadialGradient::gen();
            fill3->radial(x1 + r, y1 + r, 40.0f, x1 + r, y1 + r, 0.0f);
            fill3->colorStops(colorStops, colorCnt);
            fill3->spread(tvg::FillSpread::Repeat);
            shape3->fill(fill3);

            add(canvas, shape3, x1, y1, r);
        }

        //Linear grad
        {
            float x1, y1 = 680.0f, r = 120.0f;

            //Pad
            x1 = 20.0f;
            auto shape1 = tvg::Shape::gen();
            shape1->appendRect(x1, y1, 2.0f * r, 2.0f * r);

            auto fill1 = tvg::LinearGradient::gen();
            fill1->linear(x1, y1, x1 + 50.0f, y1 + 50.0f);
            fill1->colorStops(colorStops, colorCnt);
            fill1->spread(tvg::FillSpread::Pad);
            shape1->fill(fill1);

            add(canvas, shape1, x1, y1, r);

            //Reflect
            x1 = 280.0f;
            auto shape2 = tvg::Shape::gen();
            shape2->appendRect(x1, y1, 2.0f * r, 2.0f * r);

            auto fill2 = tvg::LinearGradient::gen();
            fill2->linear(x1, y1, x1 + 50.0f, y1 + 50.0f);
            fill2->colorStops(colorStops, colorCnt);
            fill2->spread(tvg::FillSpread::Reflect);
            shape2->fill(fill2);

            add(canvas, shape2, x1, y1, r);

            //Repeat
            x1 = 540.0f;
            auto shape3 = tvg::Shape::gen();
            shape3->appendRect(x1, y1, 2.0f * r, 2.0f * r);

            auto fill3 = tvg::LinearGradient::gen();
            fill3->linear(x1, y1, x1 + 50.0f, y1 + 50.0f);
            fill3->colorStops(colorStops, colorCnt);
            fill3->spread(tvg::FillSpread::Repeat);
            shape3->fill(fill3);

            add(canvas, shape3, x1, y1, r);

            return true;
        }
    }
};


/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    return tvgexam::main(new UserExample, argc, argv, false, 800, 1260);
}
