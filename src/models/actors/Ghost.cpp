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

#include "src/models/actors/Ghost.h"
#include "src/utils/Utils.h"
#include "src/animations/GhostAnimations.h"

namespace pm {
    ///////////////////////////////////////////////////////////////
    Ghost::Ghost(ime::Scene& scene, Colour colour) :
        ime::GameObject(scene),
        direction_{ime::Left}
    {
        setCollisionGroup("ghosts");

        int spriteSheetRow;
        if (colour == Colour::Red) {
            setTag("blinky");
            spriteSheetRow = 0;
        } else if (colour == Colour::Pink) {
            setTag("pinky");
            spriteSheetRow = 1;
        } else if (colour == Colour::Cyan) {
            setTag("inky");
            spriteSheetRow = 2;
        } else if (colour == Colour::Orange) {
            setTag("clyde");
            spriteSheetRow = 3;
        } else {
            assert(false && "Unknown ghost colour");
        }

        // Init default animation
        GhostAnimations animations{};
        animations.createAnimations(getTag());
        getSprite() = animations.getAll().at(0)->getSpriteSheet().getSprite(ime::Index{spriteSheetRow, 0});
        for (const auto& animation : animations.getAll())
            getSprite().getAnimator().addAnimation(animation);

        getSprite().scale(2.0f, 2.0f);
        getSprite().getAnimator().startAnimation("going" + utils::convertToString(direction_));
    }

    ///////////////////////////////////////////////////////////////
    std::string Ghost::getClassName() const {
        return "Ghost";
    }

    ///////////////////////////////////////////////////////////////
    Ghost::State Ghost::getState() const {
        return static_cast<Ghost::State>(ime::GameObject::getState());
    }

    ///////////////////////////////////////////////////////////////
    void Ghost::setDirection(ime::Vector2i dir) {
        if (direction_ != dir) {
            // Frightened animation is the same in all directions
            if (getState() == State::Frightened)
                return;

            std::string newAnimation = "going" + utils::convertToString(direction_);

            if (getState() == State::Eaten)
                newAnimation += "Eaten";

            getSprite().getAnimator().startAnimation(newAnimation);
        }
    }

    ///////////////////////////////////////////////////////////////
    ime::Vector2i Ghost::getDirection() const {
        return direction_;
    }

    ///////////////////////////////////////////////////////////////
    Ghost::~Ghost() = default;

} // namespace pm
