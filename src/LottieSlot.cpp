/*
 * Copyright (c) 2024 - 2026 ThorVG project. All rights reserved.
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
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

#define NUM_PER_ROW 4
#define NUM_PER_COL 4

struct UserExample : tvgexam::Example
{
    vector<unique_ptr<tvg::LottieAnimation>> slots;
    uint32_t w, h;
    uint32_t size;

    void sizing(tvg::Picture* picture, uint32_t counter)
    {
        picture->origin(0.5f, 0.5f);

        //image scaling preserving its aspect ratio
        float w, h;
        picture->size(&w, &h);
        picture->scale((w > h) ? size / w : size / h);
        picture->translate((counter % NUM_PER_ROW) * size + size / 2, (counter / NUM_PER_ROW) * (this->h / NUM_PER_COL) + size / 2);
    }

    bool update(tvg::Canvas* canvas, uint32_t elapsed) override
    {
        for (auto& slot : slots) {
            slot->frame(slot->totalFrame() * tvgexam::progress(elapsed, slot->duration()));
        }

        canvas->update();

        return true;
    }

    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        //The default font for fallback in case
        tvg::Text::load(EXAMPLE_DIR"/font/PublicSans-Regular.ttf");

        //Background
        auto bg = tvg::Shape::gen();
        bg->appendRect(0, 0, w, h);
        bg->fill(75, 75, 75);
        canvas->add(bg);

        this->w = w;
        this->h = h;
        this->size = w / NUM_PER_ROW;

        //slot (default)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot0.json"))) return false;

            sizing(picture, 0);
            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        //slot (gradient: color stops, opacity, start, end, height, angle)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot1.json"))) return false;

            const char* slotJson = R"({
                "gradient_fill":{"p":{"p":2,"k":{"k":[0,0.1,0.1,0.2,1,1,0.1,0.2,0,0,1,1]}}},
                "gradient_opacity":{"p":{"a":1,"k":[{"i":{"x":[0.833],"y":[0.833]},"o":{"x":[0.167],"y":[0.167]},"s":[100],"t":0},{"i":{"x":[0.833],"y":[0.833]},"o":{"x":[0.167],"y":[0.167]},"s":[30],"t":60},{"s":[100],"t":120}]}},
                "gradient_start":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"s":[-80,-40],"t":0},{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"s":[80,40],"t":60},{"s":[-80,-40],"t":120}]}},
                "gradient_end":{"p":{"a":0,"k":[110,0]}},
                "gradient_height":{"p":{"a":1,"k":[{"i":{"x":[0.833],"y":[0.833]},"o":{"x":[0.167],"y":[0.167]},"s":[0],"t":0},{"i":{"x":[0.833],"y":[0.833]},"o":{"x":[0.167],"y":[0.167]},"s":[90],"t":60},{"s":[0],"t":120}]}},
                "gradient_angle":{"p":{"a":1,"k":[{"i":{"x":[0.833],"y":[0.833]},"o":{"x":[0.167],"y":[0.167]},"s":[0],"t":0},{"s":[360],"t":120}]}}
            })";
            auto slotId = slot->gen(slotJson);
            if (!tvgexam::verify(slot->apply(slotId))) return false;

            sizing(picture, 1);
            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        //slot (solid fill)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot2.json"))) return false;

            const char* slotJson = R"({"ball_color":{"p":{"a":1,"k":[{"i":{"x":[0.833],"y":[0.833]},"o":{"x":[0.167],"y":[0.167]},"t":7,"s":[0,0.176,0.867]},{"i":{"x":[0.833],"y":[0.833]},"o":{"x":[0.167],"y":[0.167]},"t":22,"s":[0.867,0,0.533]},{"i":{"x":[0.833],"y":[0.833]},"o":{"x":[0.167],"y":[0.167]},"t":37,"s":[0.867,0,0.533]},{"t":51,"s":[0,0.867,0.255]}]}}})";
            auto slotId = slot->gen(slotJson);
            if (!tvgexam::verify(slot->apply(slotId))) return false;

            sizing(picture, 2);
            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        //slot (image)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot3.json"))) return false;

            const char* slotJson = R"({"path_img":{"p":{"id":"image_0","w":200,"h":300,"u":"images/","p":"logo.png","e":0}}})";
            auto slotId = slot->gen(slotJson);
            if (!tvgexam::verify(slot->apply(slotId))) return false;

            sizing(picture, 3);
            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        //slot (overriden default slot)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot4.json"))) return false;

            const char* slotJson = R"({"bg_color":{"p":{"a":0,"k":[1,0.8196,0.2275]}},"check_color":{"p":{"a":0,"k":[0.0078,0.0078,0.0078]}}})";
            auto slotId = slot->gen(slotJson);
            if (!tvgexam::verify(slot->apply(slotId))) return false;

            sizing(picture, 4);
            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        //slot (duplicate slots with default)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot5.json"))) return false;

            sizing(picture, 5);
            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        //slot (transform: position)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot6.json"))) return false;

            const char* slotJson = R"({"position_id":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"s":[100,100],"t":0},{"s":[200,300],"t":100}]}}})";
            auto slotId = slot->gen(slotJson);
            if (!tvgexam::verify(slot->apply(slotId))) return false;

            sizing(picture, 6);
            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        //slot (transform: scale)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot7.json"))) return false;

            const char* slotJson = R"({"scale_id":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"s":[0,0],"t":0},{"s":[100,100],"t":100}]}}})";
            auto slotId = slot->gen(slotJson);
            if (!tvgexam::verify(slot->apply(slotId))) return false;

            sizing(picture, 7);
            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        //slot (transform: rotation)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot8.json"))) return false;

            const char* slotJson = R"({"rotation_id":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"s":[0],"t":0},{"s":[180],"t":100}]}}})";
            auto slotId = slot->gen(slotJson);
            if (!tvgexam::verify(slot->apply(slotId))) return false;

            sizing(picture, 8);
            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        //slot (transform: opacity)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot9.json"))) return false;

            const char* slotJson = R"({"opacity_id":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"s":[0],"t":0},{"s":[100],"t":100}]}}})";
            auto slotId = slot->gen(slotJson);
            if (!tvgexam::verify(slot->apply(slotId))) return false;

            sizing(picture, 9);

            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        //slot (expression)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot10.json"))) return false;

            const char* slotJson = R"({"rect_rotation":{"p":{"x":"var $bm_rt = time * 360;"}},"rect_scale":{"p":{"x":"var $bm_rt = [];$bm_rt[0] = value[0] + Math.cos(2 * Math.PI * time) * 100;$bm_rt[1] = value[1];"}},"rect_position":{"p":{"x":"var $bm_rt = [];$bm_rt[0] = value[0] + Math.cos(2 * Math.PI * time) * 100;$bm_rt[1] = value[1];"}}})";
            auto slotId = slot->gen(slotJson);
            if (!tvgexam::verify(slot->apply(slotId))) return false;

            sizing(picture, 10);
            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        //slot (text)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot11.json"))) return false;

            const char* slotJson = R"({"text_doc":{"p":{"k":[{"s":{"f":"Ubuntu Light Italic","t":"ThorVG!","j":0,"s":48,"fc":[1,1,1]},"t":0}]}}})";
            auto slotId = slot->gen(slotJson);
            if (!tvgexam::verify(slot->apply(slotId))) return false;

            sizing(picture, 11);
            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        //slot (text range)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot12.json"))) return false;

            const char* slotJson = R"({"texty":{"p":{"a":0,"k":[1,0.5,0]}}})";
            auto slotId = slot->gen(slotJson);
            if (!tvgexam::verify(slot->apply(slotId))) return false;

            sizing(picture, 12);
            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        //slot (bezier path)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot13.json"))) return false;

            const char* slotJson = R"({"bezier_path":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"t":0,"s":[{"c":true,"i":[[0,0],[0,0],[0,0],[0,0]],"o":[[0,0],[0,0],[0,0],[0,0]],"v":[[0,-180],[180,0],[0,180],[-180,0]]}]},{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"t":90,"s":[{"c":true,"i":[[-99.4,0],[0,-99.4],[99.4,0],[0,99.4]],"o":[[99.4,0],[0,99.4],[-99.4,0],[0,-99.4]],"v":[[0,-180],[180,0],[0,180],[-180,0]]}]},{"t":180,"s":[{"c":true,"i":[[0,0],[0,0],[0,0],[0,0]],"o":[[0,0],[0,0],[0,0],[0,0]],"v":[[0,-180],[180,0],[0,180],[-180,0]]}]}]}}})";
            auto slotId = slot->gen(slotJson);
            if (!tvgexam::verify(slot->apply(slotId))) return false;

            sizing(picture, 13);
            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        //slot (time remap)
        {
            auto slot = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = slot->picture();
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/slot14.json"))) return false;

            const char* slotJson = R"({"time_remap":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"t":0,"s":[0]},{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"t":90,"s":[3]},{"t":180,"s":[0]}]}}})";
            auto slotId = slot->gen(slotJson);
            if (!tvgexam::verify(slot->apply(slotId))) return false;

            sizing(picture, 14);
            canvas->add(picture);
            slots.push_back(std::move(slot));
        }

        return true;
    }
};


/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    return tvgexam::main(new UserExample, argc, argv, false, 1024, 1024, 0 /* turn off for expressions */);
}