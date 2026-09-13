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

#include <memory>
#include <thorvg-1/thorvg_lottie.h>
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

#define NUM_PER_ROW 2
#define NUM_PER_COL 1

struct UserExample : tvgexam::Example
{
    std::unique_ptr<tvg::LottieAnimation> resolver[2];  //picture, text

    using tvgexam::Example::Example;

    void sizing(tvg::Picture* picture, uint32_t counter)
    {
        picture->origin(0.5f, 0.5f);

        //image scaling preserving its aspect ratio
        const auto& size = this->size();
        auto cellSize = size.w / NUM_PER_ROW;
        float w, h;
        picture->size(&w, &h);
        picture->scale((w > h) ? cellSize / w : cellSize / h);
        picture->translate((counter % NUM_PER_ROW) * cellSize + cellSize / 2, (counter / NUM_PER_ROW) * (size.h / NUM_PER_COL) + cellSize / 2);
    }

    bool update(tvg::Canvas* canvas, size_t elapsed) override
    {
        tvgexam::Example::update(canvas, elapsed);
        resolver[0]->frame(resolver[0]->totalFrame() * tvg::toolkit::progress(elapsed, resolver[0]->duration()));
        resolver[1]->frame(resolver[1]->totalFrame() * tvg::toolkit::progress(elapsed, resolver[1]->duration()));

        canvas->update();

        return true;
    }

    bool content(tvg::Canvas* canvas, const tvg::toolkit::App::Size& size) override
    {
        //The default font for fallback in case
        tvg::Text::load(EXAMPLE_DIR"/font/PublicSans-Regular.ttf");

        //Background
        auto bg = tvg::Shape::gen();
        bg->appendRect(0, 0, size.w, size.h);
        bg->fill(75, 75, 75);
        canvas->add(bg);

        //asset resolver (image)
        {
            resolver[0] = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = resolver[0]->picture();

            auto func = [](tvg::Paint* p, const char* src, void* data) {
                if (p->type() != tvg::Type::Picture) return false;
                //The engine may fail to access the source image. This demonstrates how to resolve it with a valid user-provided source.
                auto assetPath = std::string(src).replace(0, sizeof(EXAMPLE_DIR"/lottie/extensions/") - 1, EXAMPLE_DIR"/");
                return tvgexam::verify(static_cast<tvg::Picture*>(p)->load(assetPath.c_str())); //return true if the resolving is successful
            };

            //set a resolver prior to load a resource
            if (!tvgexam::verify(picture->resolver(func, nullptr))) return false;
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/resolver1.json"))) return false;

            sizing(picture, 0);
            canvas->add(picture);
        }

        //asset resolver (font)
        {
            resolver[1] = std::unique_ptr<tvg::LottieAnimation>(tvg::LottieAnimation::gen());
            auto picture = resolver[1]->picture();

            auto func = [](tvg::Paint* p, const char* src, void* data) {
                if (p->type() != tvg::Type::Text) return false;
                //The engine may fail to access the source font. This demonstrates how to resolve it with a valid user-provided font.
                auto assetPath = EXAMPLE_DIR"/" + std::string(src);
                if (!tvgexam::verify(tvg::Text::load(assetPath.c_str()))) return false;
                return tvgexam::verify(static_cast<tvg::Text*>(p)->font("SentyCloud")); //return true if font loading is successful
            };

            if (!tvgexam::verify(picture->resolver(func, nullptr))) return false;
            if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/lottie/extensions/resolver2.json"))) return false;

            sizing(picture, 1);
            canvas->add(picture);
        }

        return true;
    }
};

/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    auto params = tvgexam::options(argc, argv, {1024, 512});
    return tvgexam::run(new UserExample(params), params);
}
