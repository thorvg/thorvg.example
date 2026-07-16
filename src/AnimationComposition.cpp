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

#include "Example.h"

namespace
{

constexpr uint32_t ReproSize = 1013;
constexpr float BrokenFrame = 142.0f;
constexpr auto TargetName = "Dark Red Solid 1 Comp 1";
constexpr auto Asset = EXAMPLE_DIR"/lottie/calculator-broken.json";

tvg::Scene* findTarget(tvg::Picture* picture)
{
    tvg::Scene* target = nullptr;
    auto accessor = unique_ptr<tvg::Accessor>(tvg::Accessor::gen());
    auto callback = [](const tvg::Paint* paint, void* data) {
        if (paint->type() != tvg::Type::Scene ||
            paint->id != tvg::Accessor::id(TargetName)) {
            return true;
        }
        *static_cast<tvg::Scene**>(data) =
            static_cast<tvg::Scene*>(const_cast<tvg::Paint*>(paint));
        return false;
    };
    accessor->set(picture, callback, &target);
    return target;
}

tvg::Shape* dummy(float x)
{
    auto shape = tvg::Shape::gen();
    shape->appendRect(x, 0, 16, 16);
    return shape;
}

bool movePaint(tvg::Scene* from, tvg::Scene* to, tvg::Paint* paint)
{
    paint->ref();
    auto removed = from->remove(paint);
    auto added = removed == tvg::Result::Success ? to->add(paint) : removed;
    paint->unref();
    return removed == tvg::Result::Success && added == tvg::Result::Success;
}

} // namespace


/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    bool renderBrokenFrame(vector<uint32_t>& pixels)
    {
        pixels.resize(ReproSize * ReproSize);
        auto animation = unique_ptr<tvg::Animation>(tvg::Animation::gen());
        auto canvas = unique_ptr<tvg::SwCanvas>(
            tvg::SwCanvas::gen(tvg::EngineOption::None));
        if (!animation || !canvas) return false;
        auto picture = animation->picture();
        if (!tvgexam::verify(picture->load(Asset), "Failed to load the Lottie") ||
            !tvgexam::verify(animation->frame(BrokenFrame), "Failed to set the frame") ||
            !tvgexam::verify(picture->size(float(ReproSize), float(ReproSize)),
                             "Failed to resize the Lottie") ||
            !tvgexam::verify(canvas->target(pixels.data(), ReproSize, ReproSize,
                                            ReproSize, tvg::ColorSpace::ARGB8888S),
                             "Failed to create the target")) {
            return false;
        }

        auto target = findTarget(picture);
        if (!target) return false;

        auto broken = static_cast<tvg::Scene*>(target->duplicate());
        if (!broken) return false;
        target->visible(false);

        auto staging = tvg::Scene::gen();
        vector<tvg::Paint*> children(broken->paints().begin(), broken->paints().end());
        for (auto child : children) {
            if (!movePaint(broken, staging, child)) return false;
        }

        vector<tvg::Paint*> dummies = {dummy(0), dummy(32)};
        for (auto paint : dummies) broken->add(paint);

        auto transform = broken->transform();
        auto offscreen = transform;
        offscreen.e13 += 3000.0f;
        offscreen.e23 += 3000.0f;
        broken->transform(offscreen);

        canvas->add(picture);
        canvas->add(staging);
        canvas->add(broken);
        if (!tvgexam::verify(canvas->update(), "Failed to prepare the frame")) return false;

        //Cache the off-screen bounds, then replace the prepared children without
        //another update. Frame 142 now reproduces the stale composition target.
        broken->intersects(0, 0, ReproSize, ReproSize, false);
        broken->transform(transform);
        for (auto child : children) {
            if (!movePaint(staging, broken, child)) return false;
        }
        for (auto paint : dummies) paint->ref();
        auto removed = true;
        for (auto paint : dummies) {
            removed &= broken->remove(paint) == tvg::Result::Success;
        }

        auto success = removed &&
                       tvgexam::verify(canvas->draw(true), "Failed to draw the frame") &&
                       tvgexam::verify(canvas->sync(), "Failed to sync the frame");
        for (auto paint : dummies) paint->unref();
        return success;
    }

    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        vector<uint32_t> pixels;
        if (!renderBrokenFrame(pixels)) return false;

        //The SDL canvas uses ARGB8888, which does not enter the failing target
        //path. Present the ARGB8888S regression result as an immutable snapshot.
        auto snapshot = tvg::Picture::gen();
        if (!tvgexam::verify(snapshot->load(pixels.data(), ReproSize, ReproSize,
                                            tvg::ColorSpace::ARGB8888S, true),
                             "Failed to load the broken frame") ||
            !tvgexam::verify(snapshot->size(float(w), float(h)),
                             "Failed to resize the broken frame")) {
            return false;
        }
        canvas->add(snapshot);
        return true;
    }
};


/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char** argv)
{
    return tvgexam::main(new UserExample, argc, argv, true, ReproSize, ReproSize);
}
