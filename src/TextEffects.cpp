/*
 * Copyright (c) 2026 the ThorVG project. All rights reserved.

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

    bool content(tvg::Canvas* canvas, const tvg::toolkit::App::Size& size) override
    {
        // Load fonts
        return tvgexam::verify(tvg::Text::load(EXAMPLE_DIR"/font/PublicSans-Regular.ttf"));
    }

    bool update(tvg::Canvas* canvas, size_t elapsed) override
    {
        tvgexam::Example::update(canvas, elapsed);
        if (!tvgexam::verify(canvas->remove())) return false;

        const char* waveText = "WAVE EFFECT";
        auto time = elapsed * 0.001f * 3.0f;

        auto shape = tvg::Shape::gen();
        shape->appendRect(0, 0, 600, 600);
        shape->fill(50, 50, 50);
        canvas->add(shape);

        // Rainbow effect on title
        auto r = std::sin(time) * 127 + 128;
        auto g = std::sin(time + 2) * 127 + 128;
        auto b = std::sin(time + 4) * 127 + 128;

        auto animTitle = tvg::Text::gen();
        animTitle->font("PublicSans-Regular");
        animTitle->size(48);
        animTitle->text("ThorVG");
        animTitle->fill(uint8_t(r), uint8_t(g), uint8_t(b));
        animTitle->outline(2, 0, 0, 0);

        float x, y, w, h;
        animTitle->bounds(&x, &y, &w, &h);
        animTitle->translate(300 - w * 0.5f, 150 - h * 0.5f);
        canvas->add(animTitle);

        // Pulsing subtitle
        auto scale = 1 + std::sin(time * 2) * 0.1f;
        auto animSubtitle = tvg::Text::gen();
        animSubtitle->font("PublicSans-Regular");
        animSubtitle->size(24 * scale);
        animSubtitle->text("High-Performance Vector Graphics");
        animSubtitle->fill(100, 200, 255);

        animSubtitle->bounds(&x, &y, &w, &h);
        animSubtitle->translate(300 - w * 0.5f, 220 - h * 0.5f);
        canvas->add(animSubtitle);

        // Rotating text
        auto rotatingText = tvg::Text::gen();
        rotatingText->font("PublicSans-Regular");
        rotatingText->size(32);
        rotatingText->text("Animated!");
        rotatingText->fill(255, 200, 100);

        rotatingText->bounds(&x, &y, &w, &h);
        auto cx = 300;
        auto cy = 300;

        constexpr auto PI = 3.141592f;
        auto degree = time * 20;
        auto radian = degree / 180.0f * PI;
        auto cosVal = std::cos(radian);
        auto sinVal = std::sin(radian);

        auto textCenterX = cx - w * 0.5f;
        auto textCenterY = cy - h * 0.5f;

        auto tx = cx + (textCenterX - cx) * cosVal - (textCenterY - cy) * sinVal;
        auto ty = cy + (textCenterX - cx) * sinVal + (textCenterY - cy) * cosVal;

        tvg::Matrix m = {cosVal, -sinVal, tx, sinVal, cosVal, ty, 0, 0, 1};
        rotatingText->transform(m);
        canvas->add(rotatingText);

        // Wave effect text
        size_t waveLen = std::strlen(waveText);
        for (size_t i = 0; i < waveLen; i++) {
            auto yOffset = std::sin(time * 2 + i * 0.5f) * 20;
            auto charColor = std::sin(time + i * 0.3f) * 127 + 128;

            auto charText = tvg::Text::gen();
            charText->font("PublicSans-Regular");
            charText->size(28);
            char buf[2] = {waveText[i], '\0'};
            charText->text(buf);
            charText->fill(uint8_t(charColor), 150, uint8_t(255 - charColor));

            charText->bounds(&x, &y, &w, &h);
            charText->translate(150 + i * 30 - w * 0.5f, 400 + yOffset - h * 0.5f);
            canvas->add(charText);
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
    auto params = tvgexam::options(argc, argv, {600, 600});
    return tvgexam::run(new UserExample(params), params);
}
