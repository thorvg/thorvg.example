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
    static constexpr size_t MAX_LINE = 16;

    tvg::TextMetrics textMetric;
    tvg::Scene* group;
    tvg::Text* text;
    std::string data = "Type here: ";
    tvg::Shape* cursor;
    tvg::Point pos;                //cursor position
    float endPos[MAX_LINE];        //each line end position
    uint32_t curLine = 1;          //default line number
    uint32_t lastElapsed = 0;
    bool toggle = true;            //blinking toggle
    bool updated = false;

    ~UserExample()
    {
        text->unref();
        cursor->unref();
    }

    /* This example demonstrates very basic alphabet text typing. */
    bool keydown(tvg::Canvas* canvas, int32_t key) override
    {
        auto input = static_cast<char>(key);
        if (Example::lshift && input >= 'a' && input <= 'z') input -= 32;  // capital character

        tvg::GlyphMetrics glyphMetric;
        auto pos = this->pos;

        // backspace
        if (input == '\b') {
            if (data.empty()) return false;
            auto last = data.back();
            if (tvgexam::verify(text->metrics(&last, glyphMetric))) {
                data.resize(data.size() - 1);    // revert the character
                pos.x -= glyphMetric.advance;    // cursor x position
            }
        // feed line
        } else if (input == '\r') {
            data += '\n';
            endPos[curLine - 1] = pos.x;
        // append a character
        } else {
            if (tvgexam::verify(text->metrics(&input, glyphMetric))) {
                data += input;                    // append the character
                pos.x += glyphMetric.advance;     // cursor x position
            }
        }

        text->text(data.c_str());

        // get the line count
        auto newLine = text->lines();

        // limit the line number, revert the condition
        if (newLine > MAX_LINE) {
            data.resize(data.size() - 1);
            text->text(data.c_str());
            pos.x -= glyphMetric.advance;
            return false;
        }

        // line has been changed
        if (curLine != newLine) {
            // retreat
            if (curLine > newLine) {
                pos.x = endPos[newLine - 1];
            // advance
            } else {
                endPos[curLine - 1] = this->pos.x;
                pos.x = 0.0f;
            }
            curLine = newLine;
        }

        // cursor y position
        pos.y = textMetric.ascent;
        if (newLine > 1) pos.y += (newLine - 1) * textMetric.advance;

        cursor->translate(pos.x, pos.y);

        this->pos = pos;
        updated = true;

        return true;
    }

    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        if (!tvgexam::verify(tvg::Text::load(EXAMPLE_DIR"/font/NOTO-SANS-KR.ttf"))) return false;

        // guide line
        float border = 150.0f;
        float dashPattern[2] = {10.0f, 10.0f};
        auto lines = tvg::Shape::gen();
        lines->strokeFill(100, 100, 100);
        lines->strokeWidth(1);
        lines->strokeDash(dashPattern, 2);
        lines->moveTo(border, border);
        lines->lineTo(w - border, border);
        lines->lineTo(w - border, h - border);
        lines->lineTo(border, h - border);
        lines->close();
        canvas->add(lines);

        // text group
        group = tvg::Scene::gen();
        group->translate(border, border);
        canvas->add(group);

        // text body
        text = tvg::Text::gen();
        text->ref();
        text->font("NOTO-SANS-KR");
        text->size(16.0f);
        text->align(0.0f, 0.0f);
        text->layout(w - border * 2.0f, h - border * 2.0f);
        text->wrap(tvg::TextWrap::Character);
        text->text(data.c_str());
        text->fill(255, 255, 255);
        group->add(text);

        // text cursor
        tvg::GlyphMetrics glyphMetric;

        // figure out the cursor x position
        auto p = text->text();
        while (*p != '\0') {
            if (tvgexam::verify(text->metrics(p, glyphMetric))) {
                pos.x += glyphMetric.advance;
            }
            p++;
        }

        // figure out the cursor y position
        if (!tvgexam::verify(text->metrics(textMetric))) return false;
        pos.y = textMetric.ascent;

        // cursor visual
        cursor = tvg::Shape::gen();
        cursor->ref();
        cursor->appendRect(0, 0, 15, 3);
        cursor->fill(255, 255, 255);
        cursor->translate(pos.x, pos.y);
        group->add(cursor);

        return true;
    }

    bool update(tvg::Canvas* canvas, uint32_t elapsed) override
    {
        auto updated = this->updated;
        this->updated = false;

        //blinking cursor effect
        if (elapsed - lastElapsed > 500) {
            toggle = !toggle;

            if (toggle) cursor->opacity(255);
            else cursor->opacity(0);

            lastElapsed = elapsed;
            updated = true;
        }

        if (updated) canvas->update();

        return updated;
    }

};


/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    return tvgexam::main(new UserExample, argc, argv, true, 800, 800);
}