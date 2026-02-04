/*
 * Copyright (c) 2020 - 2026 ThorVG project. All rights reserved.

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

#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        //Image
        ifstream file(EXAMPLE_DIR"/image/rawimage_200x300.raw", ios::binary);
        if (!file.is_open()) return false;
        auto data = (uint32_t*) malloc(sizeof(uint32_t) * (200 * 300));
        file.read(reinterpret_cast<char*>(data), sizeof (uint32_t) * 200 * 300);
        file.close();

        //Masking
        {
            //Solid Rectangle
            auto shape = tvg::Shape::gen();
            shape->appendRect(0, 0, 300.0f, 300.0f);
            shape->fill(0, 0, 255);

            //Mask
            auto mask = tvg::Shape::gen();
            mask->appendCircle(150.0f, 150.0f, 93.75f, 93.75f);
            mask->fill(255, 255, 255);

            //Nested Mask
            auto nMask = tvg::Shape::gen();
            nMask->appendCircle(165.0f, 165.0f, 93.75f, 93.75f);
            nMask->fill(255, 255, 255);

            mask->mask(nMask, tvg::MaskMethod::Alpha);
            shape->mask(mask, tvg::MaskMethod::Alpha);
            canvas->add(shape);

            //SVG
            auto svg = tvg::Picture::gen();
            if (!tvgexam::verify(svg->load(EXAMPLE_DIR"/svg/cartman.svg"))) return false;
            svg->opacity(100);
            svg->scale(2.25f);
            svg->translate(37.5f, 300.0f);

            //Mask2
            auto mask2 = tvg::Shape::gen();
            mask2->appendCircle(112.5f, 375.0f, 56.25f, 56.25f);
            mask2->appendRect(112.5f, 375.0f, 150.0f, 150.0f, 22.5f, 22.5f);
            mask2->fill(255, 255, 255);
            svg->mask(mask2, tvg::MaskMethod::Alpha);
            canvas->add(svg);

            //Star
            auto star = tvg::Shape::gen();
            star->fill(80, 80, 80);
            star->moveTo(449.25f, 25.5f);
            star->lineTo(489.75f, 107.25f);
            star->lineTo(580.5f, 120.0f);
            star->lineTo(515.25f, 183.0f);
            star->lineTo(530.25f, 273.75f);
            star->lineTo(449.25f, 231.75f);
            star->lineTo(372.75f, 273.75f);
            star->lineTo(384.0f, 183.75f);
            star->lineTo(319.5f, 120.75f);
            star->lineTo(409.5f, 107.25f);
            star->close();
            star->strokeWidth(22.5f);
            star->strokeJoin(tvg::StrokeJoin::Miter);
            star->strokeFill(255, 255, 255);

            //Mask3
            auto mask3 = tvg::Shape::gen();
            mask3->appendCircle(450.0f, 150.0f, 93.75f, 93.75f);
            mask3->fill(255, 255, 255);
            mask3->opacity(200);
            star->mask(mask3, tvg::MaskMethod::Alpha);
            canvas->add(star);

            auto image = tvg::Picture::gen();
            if (!tvgexam::verify(image->load(data, 200, 300, tvg::ColorSpace::ARGB8888, true))) return false;
            image->translate(375.0f, 300.0f);
            image->scale(0.75f);

            //Mask4
            auto mask4 = tvg::Shape::gen();
            mask4->moveTo(449.25f, 288.0f);
            mask4->lineTo(489.75f, 369.75f);
            mask4->lineTo(580.5f, 382.5f);
            mask4->lineTo(515.25f, 445.5f);
            mask4->lineTo(530.25f, 536.25f);
            mask4->lineTo(449.25f, 494.25f);
            mask4->lineTo(372.75f, 536.25f);
            mask4->lineTo(384.0f, 446.25f);
            mask4->lineTo(319.5f, 383.25f);
            mask4->lineTo(409.5f, 369.75f);
            mask4->close();
            mask4->fill(255, 255, 255);
            mask4->opacity(70);
            image->mask(mask4, tvg::MaskMethod::Alpha);
            canvas->add(image);
        }

        //Inverse Masking
        {
            //Solid Rectangle
            auto shape = tvg::Shape::gen();
            shape->appendRect(600.0f, 0.0f, 300.0f, 300.0f);
            shape->fill(0, 0, 255);

            //Mask
            auto mask = tvg::Shape::gen();
            mask->appendCircle(750.0f, 150.0f, 93.75f, 93.75f);
            mask->fill(255, 255, 255);

            //Nested Mask
            auto nMask = tvg::Shape::gen();
            nMask->appendCircle(765.0f, 165.0f, 93.75f, 93.75f);
            nMask->fill(255, 255, 255);

            mask->mask(nMask, tvg::MaskMethod::InvAlpha);
            shape->mask(mask, tvg::MaskMethod::InvAlpha);
            canvas->add(shape);

            //SVG
            auto svg = tvg::Picture::gen();
            if (!tvgexam::verify(svg->load(EXAMPLE_DIR"/svg/cartman.svg"))) return false;
            svg->opacity(100);
            svg->scale(2.25f); 
            svg->translate(637.5f, 300.0f);

            //Mask2
            auto mask2 = tvg::Shape::gen();
            mask2->appendCircle(712.5f, 375.0f, 56.25f, 56.25f);
            mask2->appendRect(712.5f, 375.0f, 150.0f, 150.0f, 22.5f, 22.5f);
            mask2->fill(255, 255, 255);
            svg->mask(mask2, tvg::MaskMethod::InvAlpha);
            canvas->add(svg);

            //Star
            auto star = tvg::Shape::gen();
            star->fill(80, 80, 80);
            star->moveTo(1049.25f, 25.5f);
            star->lineTo(1089.75f, 107.25f);
            star->lineTo(1180.5f, 120.0f);
            star->lineTo(1115.25f, 183.0f);
            star->lineTo(1130.25f, 273.75f);
            star->lineTo(1049.25f, 231.75f);
            star->lineTo(972.75f, 273.75f);
            star->lineTo(984.0f, 183.75f);
            star->lineTo(919.5f, 120.75f);
            star->lineTo(1009.5f, 107.25f);
            star->close();
            star->strokeWidth(7.5f);
            star->strokeFill(255, 255, 255);

            //Mask3
            auto mask3 = tvg::Shape::gen();
            mask3->appendCircle(1050.0f, 150.0f, 93.75f, 93.75f);
            mask3->fill(255, 255, 255);
            star->mask(mask3, tvg::MaskMethod::InvAlpha);
            canvas->add(star);

            auto image = tvg::Picture::gen();
            if (!tvgexam::verify(image->load(data, 200, 300, tvg::ColorSpace::ABGR8888, true))) return false;
            image->scale(0.75f);
            image->translate(975.0f, 300.0f);

            //Mask4
            auto mask4 = tvg::Shape::gen();
            mask4->moveTo(1049.25f, 288.0f);
            mask4->lineTo(1089.75f, 369.75f);
            mask4->lineTo(1180.5f, 382.5f);
            mask4->lineTo(1115.25f, 445.5f);
            mask4->lineTo(1130.25f, 536.25f);
            mask4->lineTo(1049.25f, 494.25f);
            mask4->lineTo(972.75f, 536.25f);
            mask4->lineTo(984.0f, 446.25f);
            mask4->lineTo(919.5f, 383.25f);
            mask4->lineTo(1009.5f, 369.75f);
            mask4->close();
            mask4->fill(255, 255, 255);
            mask4->opacity(70);
            image->mask(mask4, tvg::MaskMethod::InvAlpha);
            canvas->add(image);
        }

        //Luma Masking
        {
            auto shape = tvg::Shape::gen();
            shape->appendRect(0.0f, 525.0f, 300.0f, 300.0f);
            shape->fill(255, 0, 0);

            auto mask = tvg::Shape::gen();
            mask->appendCircle(150.0f, 675.0f, 93.75f, 93.75f);
            mask->fill(255, 100, 255);

            auto nMask = tvg::Shape::gen();
            nMask->appendCircle(165.0f, 690.0f, 93.75f, 93.75f);
            nMask->fill(255, 200, 255);

            mask->mask(nMask, tvg::MaskMethod::Luma);
            shape->mask(mask, tvg::MaskMethod::Luma);
            canvas->add(shape);

            auto svg = tvg::Picture::gen();
            if (!tvgexam::verify(svg->load(EXAMPLE_DIR"/svg/cartman.svg"))) return false;
            svg->opacity(100);
            svg->scale(2.25f);
            svg->translate(37.5f, 825.0f);

            auto mask2 = tvg::Shape::gen();
            mask2->appendCircle(112.5f, 900.0f, 56.25f, 56.25f);
            mask2->appendRect(112.5f, 900.0f, 150.0f, 150.0f, 22.5f, 22.5f);
            mask2->fill(255, 255, 255);
            svg->mask(mask2, tvg::MaskMethod::Luma);
            canvas->add(svg);

            auto star = tvg::Shape::gen();
            star->fill(80, 80, 80);
            star->moveTo(449.25f, 540.0f);
            star->lineTo(489.75f, 632.25f);
            star->lineTo(580.5f, 645.0f);
            star->lineTo(515.25f, 708.0f);
            star->lineTo(530.25f, 798.75f);
            star->lineTo(449.25f, 756.75f);
            star->lineTo(372.75f, 798.75f);
            star->lineTo(384.0f, 708.75f);
            star->lineTo(319.5f, 645.75f);
            star->lineTo(409.5f, 632.25f);
            star->close();
            star->strokeWidth(7.5f);
            star->strokeFill(255, 255, 255);

            auto mask3 = tvg::Shape::gen();
            mask3->appendCircle(450.0f, 675.0f, 93.75f, 93.75f);
            mask3->fill(0, 255, 255);
            star->mask(mask3, tvg::MaskMethod::Luma);
            canvas->add(star);

            auto image = tvg::Picture::gen();
            if (!tvgexam::verify(image->load(data, 200, 300, tvg::ColorSpace::ARGB8888, true))) return false;
            image->translate(375.0f, 825.0f);
            image->scale(0.75f);

            auto mask4 = tvg::Scene::gen();
            auto mask4_rect = tvg::Shape::gen();
            mask4_rect->appendRect(375.0f, 825.0f, 150.0f, 225.0f);
            mask4_rect->fill(255, 255, 255);
            auto mask4_circle = tvg::Shape::gen();
            mask4_circle->appendCircle(450.0f, 937.5f, 93.75f, 93.75f);
            mask4_circle->fill(128, 0, 128);
            mask4->add(mask4_rect);
            mask4->add(mask4_circle);
            image->mask(mask4, tvg::MaskMethod::Luma);
            canvas->add(image);
        }

        //Inverse Luma Masking
        {
            auto shape = tvg::Shape::gen();
            shape->appendRect(600.0f, 525.0f, 300.0f, 300.0f);
            shape->fill(255, 0, 0);

            auto mask = tvg::Shape::gen();
            mask->appendCircle(750.0f, 675.0f, 93.75f, 93.75f);
            mask->fill(255, 100, 255);

            auto nMask = tvg::Shape::gen();
            nMask->appendCircle(765.0f, 690.0f, 93.75f, 93.75f);
            nMask->fill(255, 200, 255);

            mask->mask(nMask, tvg::MaskMethod::InvLuma);
            shape->mask(mask, tvg::MaskMethod::InvLuma);
            canvas->add(shape);

            auto svg = tvg::Picture::gen();
            if (!tvgexam::verify(svg->load(EXAMPLE_DIR"/svg/cartman.svg"))) return false;
            svg->opacity(100);
            svg->scale(2.25f);
            svg->translate(637.5f, 825.0f);

            auto mask2 = tvg::Shape::gen();
            mask2->appendCircle(712.5f, 900.0f, 56.25f, 56.25f);
            mask2->appendRect(712.5f, 900.0f, 150.0f, 150.0f, 22.5f, 22.5f);
            mask2->fill(255, 255, 255);
            svg->mask(mask2, tvg::MaskMethod::InvLuma);
            canvas->add(svg);

            auto star = tvg::Shape::gen();
            star->fill(80, 80, 80);
            star->moveTo(1049.25f, 540.0f);
            star->lineTo(1089.75f, 632.25f);
            star->lineTo(1180.5f, 645.0f);
            star->lineTo(1115.25f, 708.0f);
            star->lineTo(1130.25f, 798.75f);
            star->lineTo(1049.25f, 756.75f);
            star->lineTo(972.75f, 798.75f);
            star->lineTo(984.0f, 708.75f);
            star->lineTo(919.5f, 645.75f);
            star->lineTo(1009.5f, 632.25f);
            star->close();
            star->strokeWidth(7.5f);
            star->strokeFill(255, 255, 255);

            auto mask3 = tvg::Shape::gen();
            mask3->appendCircle(1050.0f, 675.0f, 93.75f, 93.75f);
            mask3->fill(0, 255, 255);
            star->mask(mask3, tvg::MaskMethod::InvLuma);
            canvas->add(star);

            auto image = tvg::Picture::gen();
            if (!tvgexam::verify(image->load(data, 200, 300, tvg::ColorSpace::ARGB8888, true))) return false;
            image->translate(975.0f, 825.0f);
            image->scale(0.75f);

            auto mask4 = tvg::Scene::gen();
            auto mask4_rect = tvg::Shape::gen();
            mask4_rect->appendRect(975.0f, 825.0f, 150.0f, 225.0f);
            mask4_rect->fill(255, 255, 255);
            auto mask4_circle = tvg::Shape::gen();
            mask4_circle->appendCircle(1050.0f, 937.5f, 93.75f, 93.75f);
            mask4_circle->fill(128, 0, 128);
            mask4->add(mask4_rect);
            mask4->add(mask4_circle);
            image->mask(mask4, tvg::MaskMethod::InvLuma);
            canvas->add(image);
        }

        free(data);

        return true;
    }
};


/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    return tvgexam::main(new UserExample, argc, argv, false, 1200, 1100);
}