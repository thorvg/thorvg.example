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

#include <thorvg_media.h>
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    std::string input = EXAMPLE_DIR"/media/video.mp4";
    unique_ptr<tvg::Video> video;
    bool playing = true;
    bool paused = false;

    UserExample()
    {
        cout << "Keys:\n"
                "  0: Play or Stop\n"
                "  1: Pause or Resume\n"
                "  2: Volume up\n"
                "  3: Volume down\n";
    }

    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        // background
        auto bg = tvg::Shape::gen();
        bg->appendRect(0, 0, w, h);
        bg->fill(0, 0, 0);
        canvas->add(bg);

        // video player
        video = unique_ptr<tvg::Video>(tvg::Video::gen());
        video->loop(true);

        auto picture = video->picture();
        picture->origin(0.5f, 0.5f);

        if (!tvgexam::verify(picture->load(input.c_str()))) return false;

        // video scaling preserving its aspect ratio
        float w2, h2;
        picture->size(&w2, &h2);
        auto scale = (w2 / h2 > float(w) / float(h)) ? float(w) / w2 : float(h) / h2;
        picture->scale(scale);
        picture->translate(float(w) * 0.5f, float(h) * 0.5f);

        canvas->add(picture);

        // play the video
        if (!tvgexam::verify(video->play())) return false;

        return true;
    }

    bool keydown(tvg::Canvas* canvas, int32_t key) override
    {
        auto print = [this]() {
            cout << "Video: " << (playing ? (paused ? "paused" : "playing") : "stopped") << ", volume: " << video->volume() << endl;
        };

        switch (key) {
            case SDLK_0:
                if (playing) {  // play or stop
                    if (!tvgexam::verify(video->stop())) return false;
                    playing = false;
                    paused = false;
                } else {
                    if (!tvgexam::verify(video->play())) return false;
                    playing = true;
                }
                print();
                return true;
            case SDLK_1:    // pause or resume
                if (!playing) return false;
                if (!tvgexam::verify(paused ? video->play() : video->pause())) return false;
                paused = !paused;
                print();
                return true;
            case SDLK_2: {  // volume up
                auto volume = video->volume();
                if (!tvgexam::verify(video->volume(volume < 0.9f ? volume + 0.1f : 1.0f))) return false;
                print();
                return true;
            }
            case SDLK_3: {  // volume down
                auto volume = video->volume();
                if (!tvgexam::verify(video->volume(volume > 0.1f ? volume - 0.1f : 0.0f))) return false;
                print();
                return true;
            }
            default:
                return false;
        }
    }

    bool update(tvg::Canvas* canvas, uint32_t elapsed) override
    {
        // Update the canvas alongside the video playback (only while frames advance).
        if (!playing || paused) return false;

        canvas->update();
        return true;
    }
};


/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    auto example = new UserExample;

    tvgexam::input(argc, argv, example->input);

    return tvgexam::main(example, argc, argv, false, 1024, 1024, 4, true);
}
