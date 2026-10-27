/*
 * Copyright (c) 2023 - 2026 ThorVG project. All rights reserved.

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
#include <memory>
#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    std::unique_ptr<tvg::Animation> animation;
    std::string input = EXAMPLE_DIR "/lottie/sample.json";
    tvg::Scene* scene = nullptr;
    tvg::SwCanvas* swCanvas = nullptr;
    bool cpuEngine;
    tvg::Shape* toggle = nullptr;
    tvg::Text* toggleLabel = nullptr;
    float toggleX = 0.0f;
    static constexpr float ToggleY = 16.0f;
    static constexpr float ToggleWidth = 144.0f;
    static constexpr float ToggleHeight = 44.0f;
    bool effectsEnabled = true;
    bool togglePressed = false;
    tvg::Shape* slider = nullptr;
    tvg::Text* sliderLabel = nullptr;
    static constexpr float SliderY = 108.0f;
    int pixelSize = 4;
    bool sliderPressed = false;

    UserExample(const tvgexam::Params& params) : tvgexam::Example(params)
    {
        cpuEngine = params.engine == tvg::toolkit::RenderEngine::CPU;
        tvgexam::filepath(params, input);   // -i <filepath>
    }

    bool content(tvg::Canvas* canvas, const tvg::toolkit::App::Size& size) override
    {
        if (cpuEngine) swCanvas = static_cast<tvg::SwCanvas*>(canvas);

        //The default font for fallback in case
        tvg::Text::load(EXAMPLE_DIR"/font/PublicSans-Regular.ttf");

        //Animation Controller
        animation = std::unique_ptr<tvg::Animation>(tvg::Animation::gen());
        auto picture = animation->picture();
        picture->origin(0.5f, 0.5f);  //center origin

        //Background
        auto shape = tvg::Shape::gen();
        shape->appendRect(0, 0, size.w, size.h);
        shape->fill(50, 50, 50);

        canvas->add(shape);

        if (!tvgexam::verify(picture->load(input.c_str()))) return false;

        //image scaling preserving its aspect ratio
        float w2, h2;
        picture->size(&w2, &h2);
        auto scale = (w2 > h2) ? size.w / w2 : size.h / h2;
        picture->scale(scale);
        picture->translate(float(size.w) * 0.5f, float(size.h) * 0.5f);

        //Pixellation uses square blocks measured in output pixels (CPU renderer).
        scene = tvg::Scene::gen();
        scene->add(picture);
        if (!applyEffects()) return false;
        canvas->add(scene);

        //Keep the toggle above and outside the effected scene.
        toggleX = float(size.w) - ToggleWidth - 16.0f;
        toggle = tvg::Shape::gen();
        toggle->appendRect(toggleX, ToggleY, ToggleWidth, ToggleHeight, 10, 10);
        canvas->add(toggle);

        toggleLabel = tvg::Text::gen();
        toggleLabel->font("PublicSans-Regular");
        toggleLabel->size(16);
        toggleLabel->fill(0, 0, 0);
        toggleLabel->layout(ToggleWidth, ToggleHeight);
        toggleLabel->align(0.5f, 0.5f);
        toggleLabel->translate(toggleX, ToggleY);
        canvas->add(toggleLabel);
        updateToggle();

        sliderLabel = tvg::Text::gen();
        sliderLabel->font("PublicSans-Regular");
        sliderLabel->size(16);
        sliderLabel->fill(0, 0, 0);
        sliderLabel->translate(toggleX, 76.0f);
        canvas->add(sliderLabel);

        slider = tvg::Shape::gen();
        canvas->add(slider);
        updateSlider();

        return true;
    }

    bool applyEffects()
    {
        if (!tvgexam::verify(scene->add(tvg::SceneEffect::Clear))) return false;
        if (!effectsEnabled) return true;
        if (!tvgexam::verify(scene->add(tvg::SceneEffect::Pixellation, pixelSize))) return false;
        //BIOS Mode 13h palette; expand 6-bit DAC channels to 8-bit RGB.
        static constexpr uint32_t vga256[] = {
            0x000000, 0x0000aa, 0x00aa00, 0x00aaaa, 0xaa0000, 0xaa00aa, 0xaa5500, 0xaaaaaa,
            0x555555, 0x5555ff, 0x55ff55, 0x55ffff, 0xff5555, 0xff55ff, 0xffff55, 0xffffff,
            0x000000, 0x141414, 0x202020, 0x2c2c2c, 0x383838, 0x454545, 0x515151, 0x616161,
            0x717171, 0x828282, 0x929292, 0xa2a2a2, 0xb6b6b6, 0xcbcbcb, 0xe3e3e3, 0xffffff,
            0x0000ff, 0x4100ff, 0x7d00ff, 0xbe00ff, 0xff00ff, 0xff00be, 0xff007d, 0xff0041,
            0xff0000, 0xff4100, 0xff7d00, 0xffbe00, 0xffff00, 0xbeff00, 0x7dff00, 0x41ff00,
            0x00ff00, 0x00ff41, 0x00ff7d, 0x00ffbe, 0x00ffff, 0x00beff, 0x007dff, 0x0041ff,
            0x7d7dff, 0x9e7dff, 0xbe7dff, 0xdf7dff, 0xff7dff, 0xff7ddf, 0xff7dbe, 0xff7d9e,
            0xff7d7d, 0xff9e7d, 0xffbe7d, 0xffdf7d, 0xffff7d, 0xdfff7d, 0xbeff7d, 0x9eff7d,
            0x7dff7d, 0x7dff9e, 0x7dffbe, 0x7dffdf, 0x7dffff, 0x7ddfff, 0x7dbeff, 0x7d9eff,
            0xb6b6ff, 0xc7b6ff, 0xdbb6ff, 0xebb6ff, 0xffb6ff, 0xffb6eb, 0xffb6db, 0xffb6c7,
            0xffb6b6, 0xffc7b6, 0xffdbb6, 0xffebb6, 0xffffb6, 0xebffb6, 0xdbffb6, 0xc7ffb6,
            0xb6ffb6, 0xb6ffc7, 0xb6ffdb, 0xb6ffeb, 0xb6ffff, 0xb6ebff, 0xb6dbff, 0xb6c7ff,
            0x000071, 0x1c0071, 0x380071, 0x550071, 0x710071, 0x710055, 0x710038, 0x71001c,
            0x710000, 0x711c00, 0x713800, 0x715500, 0x717100, 0x557100, 0x387100, 0x1c7100,
            0x007100, 0x00711c, 0x007138, 0x007155, 0x007171, 0x005571, 0x003871, 0x001c71,
            0x383871, 0x453871, 0x553871, 0x613871, 0x713871, 0x713861, 0x713855, 0x713845,
            0x713838, 0x714538, 0x715538, 0x716138, 0x717138, 0x617138, 0x557138, 0x457138,
            0x387138, 0x387145, 0x387155, 0x387161, 0x387171, 0x386171, 0x385571, 0x384571,
            0x515171, 0x595171, 0x615171, 0x695171, 0x715171, 0x715169, 0x715161, 0x715159,
            0x715151, 0x715951, 0x716151, 0x716951, 0x717151, 0x697151, 0x617151, 0x597151,
            0x517151, 0x517159, 0x517161, 0x517169, 0x517171, 0x516971, 0x516171, 0x515971,
            0x000041, 0x100041, 0x200041, 0x300041, 0x410041, 0x410030, 0x410020, 0x410010,
            0x410000, 0x411000, 0x412000, 0x413000, 0x414100, 0x304100, 0x204100, 0x104100,
            0x004100, 0x004110, 0x004120, 0x004130, 0x004141, 0x003041, 0x002041, 0x001041,
            0x202041, 0x282041, 0x302041, 0x382041, 0x412041, 0x412038, 0x412030, 0x412028,
            0x412020, 0x412820, 0x413020, 0x413820, 0x414120, 0x384120, 0x304120, 0x284120,
            0x204120, 0x204128, 0x204130, 0x204138, 0x204141, 0x203841, 0x203041, 0x202841,
            0x2c2c41, 0x302c41, 0x342c41, 0x3c2c41, 0x412c41, 0x412c3c, 0x412c34, 0x412c30,
            0x412c2c, 0x41302c, 0x41342c, 0x413c2c, 0x41412c, 0x3c412c, 0x34412c, 0x30412c,
            0x2c412c, 0x2c4130, 0x2c4134, 0x2c413c, 0x2c4141, 0x2c3c41, 0x2c3441, 0x2c3041,
            0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000
        };
        return tvgexam::verify(scene->add(tvg::SceneEffect::Palette, vga256, 256, 0.1));
    }

    void updateToggle()
    {
        if (effectsEnabled) toggle->fill(35, 125, 90);
        else toggle->fill(85, 85, 85);
        toggleLabel->text(effectsEnabled ? "Retro: On" : "Retro: Off");
    }

    void updateSlider()
    {
        sliderLabel->text(("Pixel size: " + std::to_string(pixelSize)).c_str());
        slider->reset();
        slider->appendRect(toggleX, SliderY - 3.0f, ToggleWidth, 6.0f, 3, 3);
        slider->appendCircle(toggleX + ToggleWidth * (pixelSize - 1) / 15.0f, SliderY, 9, 9);
        slider->fill(90, 190, 150);
    }

    bool adjustPixelSize(tvg::Canvas* canvas, float x)
    {
        auto ratio = std::max(0.0f, std::min(1.0f, (x - toggleX) / ToggleWidth));
        auto value = 1 + int(ratio * 15.0f + 0.5f);
        if (value == pixelSize) return false;
        if (!tvgexam::verify(canvas->sync())) return false;
        pixelSize = value;
        if (!applyEffects()) return false;
        updateSlider();
        canvas->update();
        return true;
    }

    bool motion(tvg::Canvas* canvas, float x, float y) override
    {
        return sliderPressed && adjustPixelSize(canvas, x);
    }

    bool hitToggle(float x, float y) const
    {
        return x >= toggleX && x < toggleX + ToggleWidth &&
               y >= ToggleY && y < ToggleY + ToggleHeight;
    }

    bool clickdown(tvg::Canvas* canvas, float x, float y, uint8_t button) override
    {
        if (button == 1) {
            togglePressed = hitToggle(x, y);
            sliderPressed = x >= toggleX - 9.0f && x <= toggleX + ToggleWidth + 9.0f &&
                            y >= SliderY - 12.0f && y <= SliderY + 12.0f;
            if (sliderPressed) return adjustPixelSize(canvas, x);
        }
        return false;
    }

    bool clickup(tvg::Canvas* canvas, float x, float y, uint8_t button) override
    {
        if (button != 1) return false;
        if (sliderPressed) {
            sliderPressed = false;
            return adjustPixelSize(canvas, x);
        }
        auto clicked = togglePressed && hitToggle(x, y);
        togglePressed = false;
        if (!clicked) return false;

        if (!tvgexam::verify(canvas->sync())) return false;
        auto enabled = !effectsEnabled;
        if (swCanvas && !tvgexam::verify(swCanvas->antialias(!enabled))) return false;
        effectsEnabled = enabled;
        if (!applyEffects()) return false;
        updateToggle();
        canvas->update();
        return true;
    }

    bool update(tvg::Canvas* canvas, size_t elapsed) override
    {
        tvgexam::Example::update(canvas, elapsed);
        auto progress = tvg::toolkit::progress(elapsed, animation->duration());

        //Update animation frame only when it's changed
        if (animation->frame(animation->totalFrame() * progress) == tvg::Result::Success) {
            canvas->update();
            return true;
        }

        return false;
    }
};

/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    auto params = tvgexam::options(argc, argv, {1024, 1024});
    return tvgexam::run(new UserExample(params), params, tvg::EngineOption::Aliased);
}