////////////////////////////////////////////////////////////////////////////////
// Pac-Man clone
//
// Copyright (c) 2021 Kwena Mashamaite (kwena.mashamaite1@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include "GridAnimation.h"

namespace pm {
    ///////////////////////////////////////////////////////////////
    GridAnimation::GridAnimation() :
        spritesheet_{"spritesheet.png", {230, 248}, {1, 1}, {238, 0, 463, 250}}
    {
        animation_ = ime::Animation::create("flash", spritesheet_, ime::seconds(2.0f));
        const int MAX_NUM_OF_FRAMES = 5;
        for (auto i = 0; i <= MAX_NUM_OF_FRAMES; ++i) {
            animation_->addFrame(ime::Index{0, 0});

            if (i != MAX_NUM_OF_FRAMES) // Make the animation finish on the normal grid colour and not on the white grid
                animation_->addFrame({0, 1}); // White grid
        }
    }

    ///////////////////////////////////////////////////////////////
    ime::Animation::Ptr GridAnimation::get() {
        return animation_;
    }

} // namespace pm
