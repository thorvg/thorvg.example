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

    tvg::Shape* shape1 = nullptr;
    tvg::Shape* shape2 = nullptr;
    tvg::Shape* shape3 = nullptr;
    float angle = 0.0f;
    tvg::Fill::ColorStop colorStops[2] = {
        {0, 255, 255, 255, 255},
        {1, 0, 0, 0, 255}};
    tvg::Fill::ColorStop colorStops2[4] = {
        {0, 0, 255, 255, 255},
        {1.0f / 3.0f, 255, 0, 255, 255},
        {2.0f / 3.0f, 255, 255, 0, 255},
        {1, 0, 255, 255, 255}};
    tvg::Fill::ColorStop colorStops3[4] = {
        {0, 0, 127, 0, 127},
        {0.25, 0, 170, 170, 170},
        {0.5, 200, 0, 200, 200},
        {1, 255, 255, 255, 255}};

    bool content(tvg::Canvas* canvas, const tvg::toolkit::App::Size& size) override
    {
        //Prepare Round Rectangle
        shape1 = tvg::Shape::gen();
        shape1->appendRect(0, 0, 400, 400);    //x, y, w, h

        //ConicGradient
        auto fill = tvg::ConicGradient::gen();
        tvgexam::verify(fill->conic(200, 200, angle));   //cx, cy, angle

        //Gradient Color Stops
        tvgexam::verify(fill->colorStops(colorStops, 2));

        tvgexam::verify(shape1->fill(fill));
        canvas->add(shape1);

        //Prepare Circle
        shape2 = tvg::Shape::gen();
        shape2->appendCircle(400, 400, 200, 200);    //cx, cy, radiusW, radiusH

        //ConicGradient
        auto fill2 = tvg::ConicGradient::gen();
        tvgexam::verify(fill2->conic(400, 400, angle));   //cx, cy, angle

        //Gradient Color Stops
        tvgexam::verify(fill2->colorStops(colorStops2, 4));

        tvgexam::verify(shape2->fill(fill2));
        canvas->add(shape2);

        //Prepare Ellipse
        shape3 = tvg::Shape::gen();
        shape3->appendCircle(600, 600, 150, 100);    //cx, cy, radiusW, radiusH

        //ConicGradient
        auto fill3 = tvg::ConicGradient::gen();
        tvgexam::verify(fill3->conic(600, 600, angle));   //cx, cy, angle

        //Gradient Color Stops
        tvgexam::verify(fill3->colorStops(colorStops3, 4));

        tvgexam::verify(shape3->fill(fill3));
        canvas->add(shape3);

        return true;
    }

    bool keydown(tvg::Canvas* canvas, tvg::toolkit::Key key) override
    {
        tvgexam::Example::keydown(canvas, key);
        if (key != tvg::toolkit::Key::Space) return false;

        angle = std::fmod(angle + 1.0f, 360.0f);
        auto fill = tvg::ConicGradient::gen();
        tvgexam::verify(fill->conic(200, 200, angle));
        tvgexam::verify(fill->colorStops(colorStops, 2));
        tvgexam::verify(shape1->fill(fill));

        auto fill2 = tvg::ConicGradient::gen();
        tvgexam::verify(fill2->conic(400, 400, angle));
        tvgexam::verify(fill2->colorStops(colorStops2, 4));
        tvgexam::verify(shape2->fill(fill2));

        auto fill3 = tvg::ConicGradient::gen();
        tvgexam::verify(fill3->conic(600, 600, angle));
        tvgexam::verify(fill3->colorStops(colorStops3, 4));
        tvgexam::verify(shape3->fill(fill3));

        std::cout << "ConicGradient angle: " << angle << std::endl;

        canvas->update();
        return true;
    }
};

/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char** argv)
{
    auto params = tvgexam::options(argc, argv, {800, 800});
    return tvgexam::run(new UserExample(params), params);
}
