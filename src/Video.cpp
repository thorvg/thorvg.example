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

#define NUM_PER_ROW 2
#define NUM_PER_COL 2

struct UserExample : tvgexam::Example
{
    std::string input = EXAMPLE_DIR"/media/video.mp4";
    vector<char> data;
    unique_ptr<tvg::Video> videos[2];  //0: file-based, 1: memory-based
    uint32_t w, h;
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

    void sizing(tvg::Picture* picture, uint32_t counter)
    {
        auto cellW = static_cast<float>(this->w) / NUM_PER_ROW;
        auto cellH = static_cast<float>(this->h) / NUM_PER_COL;

        float pw, ph;
        picture->size(&pw, &ph);
        auto scale = (pw / ph > cellW / cellH) ? cellW / pw : cellH / ph;
        picture->origin(0.5f, 0.5f);
        picture->scale(scale);
        picture->translate((counter % NUM_PER_ROW + 0.5f) * cellW, (counter / NUM_PER_ROW + 0.5f) * cellH);
    }

    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        // background
        auto bg = tvg::Shape::gen();
        bg->appendRect(0, 0, w, h);
        bg->fill(0, 0, 0);
        canvas->add(bg);

        this->w = w;
        this->h = h;

        //file-source video
        {
            videos[0] = unique_ptr<tvg::Video>(tvg::Video::gen());
            auto picture = videos[0]->picture();
            if (!tvgexam::verify(picture->load(input.c_str()))) return false;
            if (!tvgexam::verify(videos[0]->loop(true))) return false;
            sizing(picture, 0);
            canvas->add(picture);
        }

        //data-source video
        {
            ifstream file(input, ios::binary | ios::ate);
            if (!file.is_open()) return false;
            auto pos = file.tellg();
            if (pos < 0 || pos > UINT32_MAX) return false;
            auto size = static_cast<uint32_t>(pos);
            data.resize(size);
            file.seekg(0, ios::beg);
            if (!file.read(data.data(), size)) return false;

            videos[1] = unique_ptr<tvg::Video>(tvg::Video::gen());
            auto picture = videos[1]->picture();
            if (!tvgexam::verify(picture->load(data.data(), size, "mp4"))) return false;
            if (!tvgexam::verify(videos[1]->loop(true))) return false;
            if (!tvgexam::verify(videos[1]->mute(true))) return false;
            sizing(picture, 3);
            canvas->add(picture);
        }

        if (!tvgexam::verify(videos[0]->play())) return false;
        if (!tvgexam::verify(videos[1]->play())) return false;

        return true;
    }

    bool keydown(tvg::Canvas* canvas, int32_t key) override
    {
        auto print = [this]() {
            cout << "Videos: " << (playing ? (paused ? "paused" : "playing") : "stopped") << ", file volume: " << videos[0]->volume() << ", memory: muted" << endl;
        };

        switch (key) {
            case SDLK_0:
                if (playing) {  // play or stop
                    if (!tvgexam::verify(videos[0]->stop())) return false;
                    if (!tvgexam::verify(videos[1]->stop())) return false;
                    playing = false;
                    paused = false;
                } else {
                    if (!tvgexam::verify(videos[0]->play())) return false;
                    if (!tvgexam::verify(videos[1]->play())) return false;
                    playing = true;
                }
                print();
                return true;
            case SDLK_1:    // pause or resume
                if (!playing) return false;
                if (!tvgexam::verify(paused ? videos[0]->play() : videos[0]->pause())) return false;
                if (!tvgexam::verify(paused ? videos[1]->play() : videos[1]->pause())) return false;
                paused = !paused;
                print();
                return true;
            case SDLK_2: {  // volume up
                auto volume = videos[0]->volume();
                if (!tvgexam::verify(videos[0]->volume(volume < 0.9f ? volume + 0.1f : 1.0f))) return false;
                print();
                return true;
            }
            case SDLK_3: {  // volume down
                auto volume = videos[0]->volume();
                if (!tvgexam::verify(videos[0]->volume(volume > 0.1f ? volume - 0.1f : 0.0f))) return false;
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
