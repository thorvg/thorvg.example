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

#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

#define CELL 300

using PathOp = tvg::Result (*)(const tvg::Path&, const tvg::Path&, tvg::Path&);

static const PathOp OPS[] = {tvg::Path::add, tvg::Path::subtract, tvg::Path::intersect, tvg::Path::difference};
static const char* NAMES[] = {"Add", "Subtract", "Intersect", "Difference"};
static const uint8_t COLORS[][3] = {{255, 170, 0}, {0, 200, 255}, {120, 220, 90}, {240, 90, 160}};

struct UserExample : tvgexam::Example
{
    tvg::Shape* builder = nullptr;
    tvg::Path* lhs = nullptr;
    tvg::Path* rhs = nullptr;
    tvg::Path* out = nullptr;
    tvg::Shape* results[4] = {};
    tvg::Shape* guides[4] = {};

    ~UserExample()
    {
        delete(lhs);
        delete(rhs);
        delete(out);
        if (builder) tvg::Paint::rel(builder);
    }

    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        if (!tvgexam::verify(tvg::Text::load(EXAMPLE_DIR"/font/PublicSans-Regular.ttf"))) return false;

        const tvg::PathCommand* cmds;
        const tvg::Point* pts;
        uint32_t cmdCnt, ptsCnt;

        //rounded rectangle
        builder = tvg::Shape::gen();
        builder->appendRect(60, 80, 180, 180, 40, 40);
        builder->path(&cmds, &cmdCnt, &pts, &ptsCnt);

        lhs = tvg::Path::gen();
        lhs->add(cmds, cmdCnt, pts, ptsCnt);
        rhs = tvg::Path::gen();
        out = tvg::Path::gen();

        for (int i = 0; i < 4; ++i) {
            auto label = tvg::Text::gen();
            label->font("PublicSans-Regular");
            label->size(28);
            label->text(NAMES[i]);
            label->fill(255, 255, 255);
            label->translate(i * CELL + 20, 10);
            canvas->add(label);

            results[i] = tvg::Shape::gen();
            results[i]->fill(COLORS[i][0], COLORS[i][1], COLORS[i][2]);
            results[i]->translate(i * CELL, 60);
            canvas->add(results[i]);

            guides[i] = tvg::Shape::gen();
            guides[i]->strokeWidth(2);
            guides[i]->strokeFill(255, 255, 255, 100);
            guides[i]->translate(i * CELL, 60);
            canvas->add(guides[i]);
        }

        return true;
    }

    bool update(tvg::Canvas* canvas, uint32_t elapsed) override
    {
        const tvg::PathCommand* cmds;
        const tvg::Point* pts;
        uint32_t cmdCnt, ptsCnt;

        //show animated merged paths
        auto angle = tvgexam::progress(elapsed, 4.0f) * 2.0f * 3.141592f;
        auto cx = 150.0f + cos(angle) * 90.0f;
        auto cy = 170.0f + sin(angle) * 90.0f;

        builder->reset();
        builder->appendCircle(cx, cy, 70, 70);
        builder->path(&cmds, &cmdCnt, &pts, &ptsCnt);

        rhs->reset();
        rhs->add(cmds, cmdCnt, pts, ptsCnt);

        for (int i = 0; i < 4; ++i) {
            if (!tvgexam::verify(OPS[i](*lhs, *rhs, *out), NAMES[i])) return false;

            out->get(cmds, &cmdCnt, pts, &ptsCnt);
            results[i]->reset();
            results[i]->appendPath(cmds, cmdCnt, pts, ptsCnt);

            guides[i]->reset();
            guides[i]->appendRect(60, 80, 180, 180, 40, 40);
            guides[i]->appendCircle(cx, cy, 70, 70);
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
    return tvgexam::main(new UserExample, argc, argv, true, 1200, 400);
}
