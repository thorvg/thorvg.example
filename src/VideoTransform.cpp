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

#include <algorithm>
#include <thorvg_media.h>
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

#define NUM_PER_ROW 3
#define NUM_PER_COL 1

struct UserExample : tvgexam::Example
{
    unique_ptr<tvg::Video> video;
    tvg::Picture* effect = nullptr;
    uint32_t size;

    void sizing(tvg::Picture* picture, uint32_t counter, float offset = 0.0f)
    {
        float w, h;
        picture->size(&w, &h);
        picture->origin(0.5f, 0.5f);
        picture->scale(size * 0.9f / std::max(w, h));
        picture->translate((counter % NUM_PER_ROW) * size + size * 0.5f + offset,
                           (counter / NUM_PER_ROW) * size + size * 0.5f + offset);
    }

    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        auto bg = tvg::Shape::gen();
        bg->appendRect(0, 0, w, h);
        bg->fill(50, 50, 50);
        canvas->add(bg);

        size = std::min(w / NUM_PER_ROW, h / NUM_PER_COL);
        video.reset(tvg::Video::gen());
        auto picture = video->picture();
        if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/video/sample-with-audio.mp4"))) return false;

        auto duplicate = static_cast<tvg::Picture*>(picture->duplicate());
        auto duplicate2 = static_cast<tvg::Picture*>(picture->duplicate());
        effect = static_cast<tvg::Picture*>(picture->duplicate());
        if (!duplicate || !duplicate2 || !effect) return false;

        //video (original)
        sizing(picture, 0);
        canvas->add(picture);

        //video (diagonal duplicates)
        sizing(duplicate, 1, size * -0.06f);
        canvas->add(duplicate);
        sizing(duplicate2, 1, size * 0.06f);
        canvas->add(duplicate2);

        //video (rotation + Gaussian blur)
        sizing(effect, 2);
        auto scene = tvg::Scene::gen();
        scene->add(effect);
        scene->add(tvg::SceneEffect::GaussianBlur, 5.0, 0, 0, 100);
        canvas->add(scene);

        video->mute(true);
        video->loop(true);
        return tvgexam::verify(video->play());
    }

    bool update(tvg::Canvas* canvas, uint32_t elapsed) override
    {
        effect->rotate(tvgexam::progress(elapsed, 6.0f) * 360.0f);
        canvas->update();
        return true;
    }
};


/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char** argv)
{
    return tvgexam::main(new UserExample, argc, argv, false, 900, 300, 4, true);
}
