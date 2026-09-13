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

#include <fstream>
#include <memory>
#include <vector>
#include <thorvg_media.h>
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

#define NUM_PER_ROW 2
#define NUM_PER_COL 2

struct UserExample : tvgexam::Example
{
    std::string input = EXAMPLE_DIR "/media/video.mp4";
    std::vector<char> data;
    std::unique_ptr<tvg::Video> videos[2];  //0: file-based, 1: memory-based
    bool playing = true;
    bool paused = false;

    UserExample(const tvgexam::Params& params) : tvgexam::Example(params)
    {
        tvgexam::filepath(params, input);  // -i <filepath>

        std::cout << "Keys:\n"
                "  0: Play or Stop\n"
                "  1: Pause or Resume\n"
                "  2: Volume up\n"
                "  3: Volume down\n";
    }

    void sizing(tvg::Picture* picture, uint32_t counter, const tvg::toolkit::App::Size& size)
    {
        auto cellW = static_cast<float>(size.w) / NUM_PER_ROW;
        auto cellH = static_cast<float>(size.h) / NUM_PER_COL;

        float pw, ph;
        picture->size(&pw, &ph);
        auto scale = (pw / ph > cellW / cellH) ? cellW / pw : cellH / ph;
        picture->origin(0.5f, 0.5f);
        picture->scale(scale);
        picture->translate((counter % NUM_PER_ROW + 0.5f) * cellW, (counter / NUM_PER_ROW + 0.5f) * cellH);
    }

    bool content(tvg::Canvas* canvas, const tvg::toolkit::App::Size& size) override
    {
        // background
        auto bg = tvg::Shape::gen();
        bg->appendRect(0, 0, size.w, size.h);
        bg->fill(0, 0, 0);
        canvas->add(bg);

        //file-source video
        {
            videos[0] = std::unique_ptr<tvg::Video>(tvg::Video::gen());
            auto picture = videos[0]->picture();
            if (!tvgexam::verify(picture->load(input.c_str()))) return false;
            if (!tvgexam::verify(videos[0]->loop(true))) return false;
            sizing(picture, 0, size);
            canvas->add(picture);
        }

        //data-source video
        {
            std::ifstream file(input, std::ios::binary | std::ios::ate);
            if (!file.is_open()) return false;
            auto pos = file.tellg();
            if (pos < 0 || pos > UINT32_MAX) return false;
            auto dataSize = static_cast<uint32_t>(pos);
            data.resize(dataSize);
            file.seekg(0, std::ios::beg);
            if (!file.read(data.data(), dataSize)) return false;

            videos[1] = std::unique_ptr<tvg::Video>(tvg::Video::gen());
            auto picture = videos[1]->picture();
            if (!tvgexam::verify(picture->load(data.data(), dataSize, "mp4"))) return false;
            if (!tvgexam::verify(videos[1]->loop(true))) return false;
            if (!tvgexam::verify(videos[1]->mute(true))) return false;
            sizing(picture, 3, size);
            canvas->add(picture);
        }

        if (!tvgexam::verify(videos[0]->play())) return false;
        if (!tvgexam::verify(videos[1]->play())) return false;

        return true;
    }

    bool keydown(tvg::Canvas* canvas, tvg::toolkit::Key key) override
    {
        tvgexam::Example::keydown(canvas, key);

        auto print = [this]() {
            std::cout << "Videos: " << (playing ? (paused ? "paused" : "playing") : "stopped") << ", file volume: " << videos[0]->volume() << ", memory: muted" << std::endl;
        };

        switch (static_cast<int32_t>(key)) {
            case '0':
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
            case '1':  // pause or resume
                if (!playing) return false;
                if (!tvgexam::verify(paused ? videos[0]->play() : videos[0]->pause())) return false;
                if (!tvgexam::verify(paused ? videos[1]->play() : videos[1]->pause())) return false;
                paused = !paused;
                print();
                return true;
            case '2': {  // volume up
                auto volume = videos[0]->volume();
                if (!tvgexam::verify(videos[0]->volume(volume < 0.9f ? volume + 0.1f : 1.0f))) return false;
                print();
                return true;
            }
            case '3': {  // volume down
                auto volume = videos[0]->volume();
                if (!tvgexam::verify(videos[0]->volume(volume > 0.1f ? volume - 0.1f : 0.0f))) return false;
                print();
                return true;
            }
            default:
                return false;
        }
    }

    bool update(tvg::Canvas* canvas, size_t elapsed) override
    {
        tvgexam::Example::update(canvas, elapsed);
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
    auto params = tvgexam::options(argc, argv, {1024, 1024});
    return tvgexam::run(new UserExample(params), params);
}
