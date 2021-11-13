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

#include "Pellet.h"
#include "Animations/PelletAnimations.h"

namespace pm {
    ///////////////////////////////////////////////////////////////
    Pellet::Pellet(ime::Scene& scene, Type type) :
        ime::GameObject(scene),
        type_{type}
    {
        setCollisionGroup(type == Type::Energizer ? "energizers" : "dots");

        ime::Sprite& sprite = getSprite();
        if (type == Type::Energizer) {
            ime::Animation::Ptr blinkAnimation = PelletAnimations().get();
            sprite = blinkAnimation->getSpriteSheet().getSprite(ime::Index{0, 0});
            sprite.getAnimator().addAnimation(std::move(blinkAnimation));
            sprite.getAnimator().startAnimation("blink");
        } else {
            sprite.setTexture("spritesheet.png");
            sprite.setTextureRect(ime::UIntRect{137, 210, 16, 16});
        }

        getSprite().scale(2.0f, 2.0f);
        resetSpriteOrigin(); // Set sprite origin as the centre
    }

    ///////////////////////////////////////////////////////////////
    std::string Pellet::getClassName() const {
        return "Pellet";
    }

    ///////////////////////////////////////////////////////////////
    Pellet::Type Pellet::getPelletType() const {
        return type_;
    }

} // namespace pm
