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

#include "src/animations/PacManAnimations.h"

namespace pm {
    ///////////////////////////////////////////////////////////////
    PacManAnimations::PacManAnimations() :
        movementSpritesheet_{"spritesheet.png", {16, 16}, {1, 1}, {0, 0, 52, 69}},
        deathSpritesheet_{"spritesheet.png", {16, 16}, {1, 1}, {0, 0, 239, 18}}
    {}

    ///////////////////////////////////////////////////////////////
    void PacManAnimations::create() {
        // Movement animations
        createAnimation("goingLeft", {0, 0});
        createAnimation("goingUp", {1, 0});
        createAnimation("goingRight", {2, 0});
        createAnimation("goingDown", {3, 0});

        // Death animation
        auto deathAnimation = ime::Animation::create("dying", deathSpritesheet_, ime::seconds(2));
        deathAnimation->addFrames({0, 0}, 14);
        animations_.push_back(std::move(deathAnimation));
    }

    ///////////////////////////////////////////////////////////////
    const std::vector<ime::Animation::Ptr> &PacManAnimations::getAll() const {
        return animations_;
    }

    ///////////////////////////////////////////////////////////////
    void PacManAnimations::createAnimation(const std::string &name, ime::Index index) {
        auto movementAnimDuration = ime::milliseconds(150);
        ime::Animation::Ptr animation = ime::Animation::create(name, movementSpritesheet_, movementAnimDuration);
        animation->addFrames(index, 3);
        animation->showTargetOnStart(false);
        animation->setRepeatCount(-1);
        animations_.push_back(std::move(animation));
    }

} // namespace pm
