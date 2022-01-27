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

#include "Ghost.h"
#include "Utils/Utils.h"
#include "AI/ghost/GIdleState.h"
#include "Animations/GhostAnimations.h"
#include <memory>
#include <cassert>

namespace pm {
    ///////////////////////////////////////////////////////////////
    Ghost::Ghost(ime::Scene& scene, Colour colour) :
        ime::GridObject(scene),
        isLockedInHouse_{false}
    {
        setCollisionGroup("ghosts");

        if (colour == Colour::Red) {
            setDirection(ime::Right);
            setTag("blinky");
        } else if (colour == Colour::Pink) {
            setDirection(ime::Down);
            setTag("pinky");
        } else if (colour == Colour::Cyan) {
            setDirection(ime::Up);
            setTag("inky");
        } else if (colour == Colour::Orange) {
            setDirection(ime::Up);
            setTag("clyde");
        }

        initAnimations();
    }

    ///////////////////////////////////////////////////////////////
    void Ghost::initFSM() {
        fsm_.clear();
        fsm_.push(std::make_unique<GIdleState>(&fsm_, this));
        fsm_.start();
    }

    ///////////////////////////////////////////////////////////////
    std::string Ghost::getClassName() const {
        return "Ghost";
    }

    ///////////////////////////////////////////////////////////////
    Ghost::State Ghost::getState() const {
        return static_cast<Ghost::State>(ime::GridObject::getState());
    }

    ///////////////////////////////////////////////////////////////
    void Ghost::playFlashAnimation(bool flash) {
        if (flash)
            getSprite().getAnimator().startAnimation("flash");
        else {
            if (getState() == State::Frightened)
                getSprite().getAnimator().startAnimation("frightened");
            else
                getSprite().getAnimator().startAnimation("going" + utils::convertToString(getDirection()));
        }
    }

    ///////////////////////////////////////////////////////////////
    bool Ghost::isFlashAnimationPlaying() const {
        return getSprite().getAnimator().getActiveAnimation()->getName() == "flash";
    }

    ///////////////////////////////////////////////////////////////
    void Ghost::lockInGhostHouse(bool lock) {
        isLockedInHouse_ = lock;
    }

    ///////////////////////////////////////////////////////////////
    bool Ghost::isLockedInGhostHouse() const {
        return isLockedInHouse_;
    }

    ///////////////////////////////////////////////////////////////
    void Ghost::update(ime::Time deltaTime) {
        assert(fsm_.top() && "A ghost FSM must have at least one state before an updated");
        ime::GridObject::update(deltaTime);
        fsm_.top()->update(deltaTime);
    }

    ///////////////////////////////////////////////////////////////
    void Ghost::handleEvent(GameEvent event, const ime::PropertyContainer &args) {
        assert(fsm_.top() && "A ghost FSM must have at least one state before handling an event");
        fsm_.top()->handleEvent(event, args);
    }

    ///////////////////////////////////////////////////////////////
    void Ghost::initAnimations() {
        GhostAnimations animations;
        animations.createAnimations(getTag());

        int spriteSheetRow = getTag() == "blinky" ? 0 : (getTag() == "pinky" ? 1 : (getTag() == "inky" ? 2 : 3));
        getSprite() = animations.getAll().at(0)->getSpriteSheet().getSprite(ime::Index{spriteSheetRow, 0});
        for (const auto& animation : animations.getAll())
            getSprite().getAnimator().addAnimation(animation);

        getSprite().scale(2.0f, 2.0f);
        resetSpriteOrigin();
        getSprite().getAnimator().startAnimation("going" + utils::convertToString(getDirection()));

        // Automatically switch animations on direction change
        onPropertyChange("direction", [this](const ime::Property& property) {
            // Frightened animation is the same in all directions
            if (getState() == State::Frightened)
                return;

            std::string newAnimation = "going" + utils::convertToString(property.getValue<ime::Vector2i>());

            if (getState() == State::Eaten)
                newAnimation += "Eaten";

            getSprite().getAnimator().startAnimation(newAnimation);
        });
    }

    ///////////////////////////////////////////////////////////////
    Ghost::~Ghost() = default;

} // namespace pm
