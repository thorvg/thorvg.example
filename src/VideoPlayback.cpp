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

struct UserExample : tvgexam::Example
{
    unique_ptr<tvg::Video> video;
    bool playing = true;
    bool looping = true;

    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        if (!tvgexam::verify(tvg::Text::load(EXAMPLE_DIR"/font/PublicSans-Regular.ttf"))) return false;

        auto bg = tvg::Shape::gen();
        bg->appendRect(0, 0, w, h);
        bg->fill(0, 0, 0);
        canvas->add(bg);

        video.reset(tvg::Video::gen());
        auto picture = video->picture();
        if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/video/sample-with-audio.mp4"))) return false;

        float pw, ph;
        picture->size(&pw, &ph);
        picture->origin(0.5f, 0.5f);
        picture->scale(std::min(w / pw, h / ph));
        picture->translate(w * 0.5f, h * 0.5f);
        canvas->add(picture);

        auto shortcut = tvg::Text::gen();
        shortcut->font("PublicSans-Regular");
        shortcut->size(16.0f);
        shortcut->text("Shortcuts\nSpacebar: Play/Pause  S: Stop  M: Mute  L: Loop\nLeft/Right: Seek  Up/Down: Volume");
        shortcut->fill(255, 255, 255);
        shortcut->translate(20.0f, 20.0f);
        canvas->add(shortcut);

        video->loop(looping);
        return tvgexam::verify(video->play());
    }

    bool update(tvg::Canvas* canvas, uint32_t) override
    {
        canvas->update();
        return true;
    }

    bool keydown(tvg::Canvas*, int32_t key) override
    {
        if (key == ' ') {
            auto result = playing ? video->pause() : video->play();
            if (result == tvg::Result::Success) playing = !playing;
        } else if (key == 's') {
            if (video->stop() == tvg::Result::Success) playing = false;
        } else if (key == 'm') {
            video->mute(!video->muted());
        } else if (key == 'l') {
            looping = !looping;
            video->loop(looping);
        } else if (key == SDLK_LEFT) {
            video->seek(std::max(0.0f, video->time() - 1.0f));
        } else if (key == SDLK_RIGHT) {
            video->seek(std::min(video->duration(), video->time() + 1.0f));
        } else if (key == SDLK_UP) {
            video->volume(std::min(1.0f, video->volume() + 0.1f));
        } else if (key == SDLK_DOWN) {
            video->volume(std::max(0.0f, video->volume() - 0.1f));
        } else {
            return false;
        }
        return true;
    }
};


/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char** argv)
{
    return tvgexam::main(new UserExample, argc, argv, false, 800, 600, 4, true);
}
