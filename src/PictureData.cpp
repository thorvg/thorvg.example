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

#include "Example.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct UserExample : tvgexam::Example
{
    tvg::Picture* picture1;
    tvg::Picture* picture2;
    uint8_t buffer[800*800*4];

    void generate()
    {
        // generate the content image
        for (int i = 0; i < 800*800*4; i+=4) {
            buffer[i + 0] = 255;
            buffer[i + 1] = rand() % 255;
            buffer[i + 2] = rand() % 255;
            buffer[i + 3] = rand() % 255;
        }
    }

    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        generate();

        // picture 1
        picture1 = tvg::Picture::gen();
        picture1->scale(0.5f);

        // load the picture raw image data
        if (!tvgexam::verify(picture1->load((uint32_t*)buffer, 800, 800, tvg::ColorSpace::ABGR8888))) return false;

        canvas->add(picture1);

        // picture 2
        picture2 = tvg::Picture::gen();
        picture2->translate(w/2, h/2);
        picture2->scale(0.5f);

        // load the picture raw image data
        if (!tvgexam::verify(picture2->load((uint32_t*)buffer, 800, 800, tvg::ColorSpace::ABGR8888))) return false;

        canvas->add(picture2);

        return true;
    }

    bool update(tvg::Canvas* canvas, uint32_t elapsed) override
    {
        generate();

        // only reload the picture1 image
        if (!tvgexam::verify(picture1->load((uint32_t*)buffer, 800, 800, tvg::ColorSpace::ABGR8888))) return false;

        canvas->update();

        return true;
    }
};


/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    return tvgexam::main(new UserExample, argc, argv, true, 800, 800);
}
