/*
 * Copyright (c) 2025 - 2026 ThorVG project. All rights reserved.

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

#include <cstdio>
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    UserExample(const tvgexam::Params& params) : tvgexam::Example(params, true) {}

    void label(tvg::Canvas* canvas, const char* value, float x, float y, float size = 12)
    {
        auto text = tvg::Text::gen();
        text->font("LiberationSans-Regular");
        text->size(size);
        text->text(value);
        text->translate(x, y);
        text->fill(200, 200, 200);
        canvas->add(text);
    }

    void sample(tvg::Canvas* canvas, const char* value, tvg::TextWrap mode, float x, float y, float width, float height)
    {
        auto guide = tvg::Shape::gen();
        guide->appendRect(x, y, width, height);
        guide->strokeFill(100, 100, 100);
        guide->strokeWidth(1);
        canvas->add(guide);

        auto text = tvg::Text::gen();
        text->font("LiberationSans-Regular");
        text->size(30);
        text->text(value);
        text->layout(width, height);
        text->wrap(mode);
        text->translate(x, y);
        text->fill(255, 255, 255);
        canvas->add(text);

        char status[32];
        auto lines = text->lines();
        std::snprintf(status, sizeof(status), "%u %s", lines, lines == 1 ? "line" : "lines");
        label(canvas, status, x + width + 18, y + height - 20, 10);
    }

    bool content(tvg::Canvas* canvas, const tvg::toolkit::App::Size& size) override
    {
        if (!tvgexam::verify(tvg::Text::load(EXAMPLE_DIR "/font/LiberationSans-Regular.ttf"))) return false;

        label(canvas, "Kerning across line breaks (Liberation Sans)", 25, 15, 18);
        label(canvas, "Reference: AV", 25, 60);
        sample(canvas, "AV", tvg::TextWrap::None, 180, 50, 90, 55);
        label(canvas, "Same font size and box in every grid cell. Narrow boxes fit A, but not AV.", 340, 55);
        label(canvas, "Labels: \\n = line break, [sp] = space. Gray edges mark each line's origin.", 340, 80);

        const char* names[] = {"None", "Character", "Word", "Smart", "Ellipsis"};
        const char* labels[] = {"A\\nV", "AV", "A\\nAV", "A[sp]\\nAV", "A[sp]\\nV"};
        const char* values[] = {"A\nV", "AV", "A\nAV", "A \nAV", "A \nV"};
        tvg::TextWrap modes[] = {tvg::TextWrap::None, tvg::TextWrap::Character, tvg::TextWrap::Word, tvg::TextWrap::Smart, tvg::TextWrap::Ellipsis};

        for (auto col = 0; col < 5; ++col) {
            auto x = 180.0f + col * 180;
            label(canvas, names[col], x, 125, 14);
            for (auto row = 0; row < 5; ++row) {
                auto y = 160.0f + row * 175;
                if (col == 0) label(canvas, labels[row], 25, y + 10, 14);
                sample(canvas, values[row], modes[col], x, y, 42, 150);
            }
        }

        //edge cases: each cell sets its own wrap mode and box width
        struct {
            const char* name;
            const char* text;
            const char* value;
            tvg::TextWrap mode;
            float width;
            const char* path;
            const char* fix;
        } cases[] = {
            {"Character, 20px", "AV", "AV", tvg::TextWrap::Character, 20, "wrapChar: force pushing", "Reset left glyph after feedLine"},
            {"Word, 42px", "A[sp]AV", "A AV", tvg::TextWrap::Word, 42, "wrapWord: word shift at first glyph", "Remove preceding-line kerning"},
            {"Word, 90px", "AV[sp]AV[sp]AV", "AV AV AV", tvg::TextWrap::Word, 90, "wrapWord: word shift inside word", "Remove preceding-line kerning"},
            {"Smart, 42px", "A[sp]AV", "A AV", tvg::TextWrap::Smart, 42, "wrapWord: word shift, then fallback", "Smart fallback: wrap by character"},
            {"Ellipsis, 30px", "A\\nW", "A\nW", tvg::TextWrap::Ellipsis, 30, "wrapEllipsis: ellipsis > revert", "Reset capture at explicit newline"},
        };

        label(canvas, "Edge cases", 1110, 125, 14);
        for (auto row = 0; row < 5; ++row) {
            auto y = 160.0f + row * 175;
            auto& c = cases[row];
            sample(canvas, c.value, c.mode, 1110, y, c.width, 150);
            label(canvas, c.name, 1110 + c.width + 18, y + 8);
            label(canvas, c.text, 1110 + c.width + 18, y + 26);
            label(canvas, c.path, 1110 + c.width + 18, y + 56, 10);
            label(canvas, c.fix, 1110 + c.width + 18, y + 76, 10);
        }
        return true;
    }
};

/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    auto params = tvgexam::options(argc, argv, {1510, 1040});
    return tvgexam::run(new UserExample(params), params);
}
