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

#include "PacMan.h"
#include "Animations/PacManAnimations.h"
#include "Utils/Utils.h"
#include "Common/Constants.h"

namespace pm {
    ///////////////////////////////////////////////////////////////
    PacMan::PacMan(ime::Scene& scene) :
        ime::GameObject(scene),
        livesCount_{Constants::PLAYER_LiVES},
        state_{State::Unknown}
    {
        setTag("pacman");
        setCollisionGroup("pacman");
        initAnimations();

        getTransform().scale(2.0f, 2.0f);
        setDirection(ime::Left);
        setState(State::Idle);
    }

    ///////////////////////////////////////////////////////////////
    void PacMan::setLivesCount(int numOfLives) {
        livesCount_ = numOfLives;
        emitChange(ime::Property{"livesCount", livesCount_});
    }

    ///////////////////////////////////////////////////////////////
    int PacMan::getLivesCount() const {
        return livesCount_;
    }

    ///////////////////////////////////////////////////////////////
    void PacMan::switchAnimation(ime::Direction dir) {
        if (state_ != State::Dying) {
            getSprite().getAnimator().startAnimation("going" + utils::convertToString(dir));
            resetSpriteOrigin();
        }
    }

    ///////////////////////////////////////////////////////////////
    void PacMan::setState(PacMan::State state) {
        if (state_ != state) {
            state_ = state;
            ime::GameObject::setState(static_cast<int>(state));

            if (state_ == State::Dying)
                getSprite().getAnimator().startAnimation("dying");
            else
                switchAnimation(direction_);
        }
    }

    ///////////////////////////////////////////////////////////////
    PacMan::State PacMan::getState() const {
        return static_cast<State>(ime::GameObject::getState());
    }

    ///////////////////////////////////////////////////////////////
    void PacMan::setDirection(const ime::Vector2i &direction) {
        if (direction_ != direction) {
            direction_ = direction;
            switchAnimation(direction_);
        }
    }

    ///////////////////////////////////////////////////////////////
    ime::Vector2i PacMan::getDirection() const {
        return direction_;
    }

    ///////////////////////////////////////////////////////////////
    void PacMan::initAnimations() {
        auto animations = PacManAnimations();
        animations.create();
        getSprite() = animations.getAll().at(0)->getSpriteSheet().getSprite(ime::Index{0, 0});

        for (const auto& animation : animations.getAll())
            getSprite().getAnimator().addAnimation(animation);
    }

    ///////////////////////////////////////////////////////////////
    std::string PacMan::getClassName() const {
        return "PacMan";
    }

} // namespace pm
