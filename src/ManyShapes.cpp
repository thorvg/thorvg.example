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

struct UserExample : tvgexam::Example
{
    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        constexpr uint32_t columns = 40;
        constexpr uint32_t rows = 30;
        const auto cellW = static_cast<float>(w) / columns;
        const auto cellH = static_cast<float>(h) / rows;
        const auto padding = 4.0f;
        const auto shapeW = cellW - 2.0f * padding;
        const auto shapeH = cellH - 2.0f * padding;
        const auto arm = shapeW * 0.38f;

        for (uint32_t column = 0; column < columns; ++column) {
            for (uint32_t row = 0; row < rows; ++row) {
                const auto x = column * cellW + padding;
                const auto y = row * cellH + padding;
                auto shape = tvg::Shape::gen();

                shape->moveTo(x, y);
                shape->lineTo(x + shapeW, y);
                shape->lineTo(x + shapeW, y + arm);
                shape->lineTo(x + arm, y + arm);
                shape->lineTo(x + arm, y + shapeH);
                shape->lineTo(x, y + shapeH);
                shape->close();
                shape->fill(48 + (column * 29 + row * 7) % 192,
                            48 + (column * 11 + row * 31) % 192,
                            48 + (column * 23 + row * 17) % 192);
                shape->fillRule(tvg::FillRule::NonZero);
                canvas->add(shape);
            }
        }
        return true;
    }

    bool update(tvg::Canvas* canvas, uint32_t elapsed) override
    {
        uint32_t index = 0;
        for (auto paint : canvas->paints()) {
            const auto column = index / 30;
            const auto row = index % 30;
            auto phase = (elapsed / 4 + column * 11 + row * 7) % 510;
            if (phase > 255) phase = 510 - phase;
            paint->opacity(static_cast<uint8_t>(96 + phase * 159 / 255));
            ++index;
        }
        return canvas->update() == tvg::Result(0);
    }
};


/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    return tvgexam::main(new UserExample, argc, argv, true, 1280, 960, 0, true);
}
