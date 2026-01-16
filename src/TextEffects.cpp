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
    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        // Load fonts
        return tvgexam::verify(tvg::Text::load(EXAMPLE_DIR"/font/Arial.ttf"));
    }

    bool update(tvg::Canvas* canvas, uint32_t elapsed) override
    {
        if (!tvgexam::verify(canvas->remove())) return false;

        const char* waveText = "WAVE EFFECT";
        float time = elapsed * 0.001f * 3.0f;

        auto shape = tvg::Shape::gen();
        shape->appendRect(0, 0, 600, 600);
        shape->fill(50, 50, 50);
        canvas->add(shape);

        // Rainbow effect on title
        float r = std::sin(time) * 127 + 128;
        float g = std::sin(time + 2) * 127 + 128;
        float b = std::sin(time + 4) * 127 + 128;

        auto animTitle = tvg::Text::gen();
        animTitle->font("Arial");
        animTitle->size(48);
        animTitle->text("ThorVG");
        animTitle->fill(uint8_t(r), uint8_t(g), uint8_t(b));
        animTitle->outline(2, 0, 0, 0);

        float x, y, w, h;
        animTitle->bounds(&x, &y, &w, &h);
        animTitle->translate(300 - w * 0.5f, 150 - h * 0.5f);
        canvas->add(animTitle);

        // Pulsing subtitle
        float scale = 1 + std::sin(time * 2) * 0.1f;
        auto animSubtitle = tvg::Text::gen();
        animSubtitle->font("Arial");
        animSubtitle->size(24 * scale);
        animSubtitle->text("High-Performance Vector Graphics");
        animSubtitle->fill(100, 200, 255);

        animSubtitle->bounds(&x, &y, &w, &h);
        animSubtitle->translate(300 - w * 0.5f, 220 - h * 0.5f);
        canvas->add(animSubtitle);

        // Rotating text
        auto rotatingText = tvg::Text::gen();
        rotatingText->font("Arial");
        rotatingText->size(32);
        rotatingText->text("Animated!");
        rotatingText->fill(255, 200, 100);

        rotatingText->bounds(&x, &y, &w, &h);
        float cx = 300;
        float cy = 300;

        constexpr auto PI = 3.141592f;
        float degree = time * 20;
        float radian = degree / 180.0f * PI;
        float cosVal = std::cos(radian);
        float sinVal = std::sin(radian);

        float textCenterX = cx - w * 0.5f;
        float textCenterY = cy - h * 0.5f;

        float tx = cx + (textCenterX - cx) * cosVal - (textCenterY - cy) * sinVal;
        float ty = cy + (textCenterX - cx) * sinVal + (textCenterY - cy) * cosVal;

        tvg::Matrix m = {cosVal, -sinVal, tx, sinVal, cosVal, ty, 0, 0, 1};
        rotatingText->transform(m);
        canvas->add(rotatingText);

        // Wave effect text
        size_t waveLen = strlen(waveText);
        for (size_t i = 0; i < waveLen; i++) {
            float yOffset = std::sin(time * 2 + i * 0.5f) * 20;
            float charColor = std::sin(time + i * 0.3f) * 127 + 128;

            auto charText = tvg::Text::gen();
            charText->font("Arial");
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
    return tvgexam::main(new UserExample, argc, argv, false, 600, 600);
}
