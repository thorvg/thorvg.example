/*
 * Copyright (c) 2026 ThorVG project. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <thorvg-1/thorvg_lottie.h>
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    unique_ptr<tvg::LottieAnimation> lottie;

    tvg::Shape* indicator = nullptr;
    ma_engine maEngine;
    ma_sound  maSound;
    bool      maEngineReady = false;
    bool      maSoundReady  = false;
    bool      audioActive   = false;


    ~UserExample()
    {
        if (maSoundReady)  ma_sound_uninit(&maSound);
        if (maEngineReady) ma_engine_uninit(&maEngine);
    }


    // Audio callback — fires on the update() calling thread
    void onAudio(const tvg::AudioInfo& info)
    {
        audioActive = info.active;

        if (info.active) {
            cout << "Audio: PLAY  id=" << info.id << "  offset=" << info.offset << "s  vol=" << info.volume;
            if (info.mimeType) cout << "  [" << info.mimeType << "]";
            if (info.path)     cout << "  " << info.path;
            cout << endl;

            if (!maSoundReady && info.path) {
                auto flags = MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION;
                if (ma_sound_init_from_file(&maEngine, info.path, flags,
                                            nullptr, nullptr, &maSound) == MA_SUCCESS) {
                    maSoundReady = true;
                    cout << "Audio: Sound loaded OK" << endl;
                } else {
                    cerr << "Audio: Failed to load: " << info.path << endl;
                }
            }

            if (maSoundReady) {
                auto sampleRate = ma_engine_get_sample_rate(&maEngine);
                ma_sound_seek_to_pcm_frame(&maSound,
                    static_cast<ma_uint64>(info.offset * sampleRate));
                ma_sound_set_volume(&maSound, info.volume);
                ma_sound_start(&maSound);
            }

        } else {
            cout << "Audio: STOP  id=" << info.id << endl;
            if (maSoundReady) ma_sound_stop(&maSound);
        }
    }


    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        if (ma_engine_init(nullptr, &maEngine) != MA_SUCCESS) {
            cerr << "LottieAudio: miniaudio engine init failed" << endl;
            return false;
        }
        maEngineReady = true;

        // Background
        auto* bg = tvg::Shape::gen();
        bg->appendRect(0, 0, w, h);
        bg->fill(30, 30, 35);
        canvas->add(bg);

        // Lottie animation
        lottie = unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
        auto* pic = lottie->picture();

        if (!tvgexam::verify(pic->load(EXAMPLE_DIR"/lottie/bell_audio.json"))) return false;

        float pw, ph;
        pic->size(&pw, &ph);
        float scale = (w < h ? w : h) * 0.8f / (pw > ph ? pw : ph);
        pic->scale(scale);
        pic->translate((w - pw * scale) * 0.5f, (h - ph * scale) * 0.5f);

        canvas->add(pic);

        // Audio status indicator
        auto* dot = tvg::Shape::gen();
        dot->appendCircle(w - 32.0f, 32.0f, 16.0f, 16.0f);
        dot->fill(80, 80, 80);     // gray = inactive
        indicator = dot;
        canvas->add(dot);

        // Register the audio callback
        lottie->audioCallback([this](const tvg::AudioInfo& info) {
            onAudio(info);
        });

        return true;
    }


    bool update(tvg::Canvas* canvas, uint32_t elapsed) override
    {
        auto progress = tvgexam::progress(elapsed, lottie->duration());
        lottie->frame(lottie->totalFrame() * progress);

        if (indicator) {
            if (audioActive) indicator->fill(60, 220, 90);   // green  = playing
            else             indicator->fill(80, 80,  80);   // gray   = silent
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
    return tvgexam::main(new UserExample, argc, argv, false, 600, 600, 0);
}
