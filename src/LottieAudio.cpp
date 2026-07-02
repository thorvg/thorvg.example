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

#include <thorvg-1/thorvg_lottie.h>
#include <iomanip>
#include <iostream>
#include "Example.h"


/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    unique_ptr<tvg::LottieAnimation> lottie;

    bool playing = false;
    float progress = 0.0f;

    uint32_t elapsed = 0;
    uint32_t audioBaseElapsed = 0;
    float    audioBaseOffset = 0.0f;
    float    audioVolume = 0.0f;

    void onAudio(const tvg::LottieAudioResolver& info)
    {
        if (info.active) {
            if (!playing) {
                playing = true;
                std::cout << "[audio] play (" << (info.embedded ? "embedded" : "external")
                          << ") offset=" << std::fixed << std::setprecision(3) << info.offset
                          << "s volume=" << std::setprecision(0) << info.volume << std::endl;
            }
            audioBaseOffset = info.offset;
            audioBaseElapsed = elapsed;
            audioVolume = info.volume;
        } else if (playing) {
            playing = false;
            std::cout << "\n[audio] stop" << std::endl;
        }
    }

    //draw a text progress of current audio playback position.
    void drawPosition()
    {
        constexpr int WIDTH = 40;

        auto pos = audioBaseOffset + (elapsed - audioBaseElapsed) / 1000.0f;
        int head = static_cast<int>(progress * WIDTH);
        if (head >= WIDTH) head = WIDTH - 1;

        char bar[WIDTH + 1];
        for (int i = 0; i < WIDTH; ++i) {
            bar[i] = (i < head) ? '=' : (i == head) ? '>' : '-';
        }
        bar[WIDTH] = '\0';

        std::cout << "\r[audio] |" << bar << "| "
                  << std::setw(6) << std::fixed << std::setprecision(2) << pos
                  << "s  vol " << std::setw(3) << std::setprecision(0) << audioVolume
                  << std::endl;
    }

    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        //background
        auto* bg = tvg::Shape::gen();
        bg->appendRect(0, 0, w, h);
        bg->fill(30, 30, 35);
        canvas->add(bg);

        //lottie animation
        lottie = unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
        auto* pic = lottie->picture();
        if (!tvgexam::verify(pic->load(EXAMPLE_DIR"/lottie/extensions/audio.json"))) return false;

        float pw, ph;
        pic->size(&pw, &ph);
        auto scale = (pw / ph > float(w) / h) ? float(w) / pw : float(h) / ph;
        pic->scale(scale);
        pic->translate((w - pw * scale) * 0.5f, (h - ph * scale) * 0.5f);
        canvas->add(pic);

        //register the audio resolver
        auto func = [this](const tvg::LottieAudioResolver& info, void*) {
            onAudio(info);
        };
        if (!tvgexam::verify(lottie->resolver(func, nullptr))) return false;

        return true;
    }

    bool update(tvg::Canvas* canvas, uint32_t elapsed) override
    {
        this->elapsed = elapsed;

        progress = tvgexam::progress(elapsed, lottie->duration());

        lottie->frame(lottie->totalFrame() * progress);

        if (playing) drawPosition();

        canvas->update();
        return true;
    }
};


/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    return tvgexam::main(new UserExample, argc, argv, false, 1024, 600, 0);
}
