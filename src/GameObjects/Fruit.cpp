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

#include "Fruit.h"

namespace pm {
    ///////////////////////////////////////////////////////////////
    Fruit::Fruit(ime::Scene& scene, Type type) :
        ime::GameObject(scene)
    {
        setCollisionGroup("fruits");

        std::string tag;
        ime::Index texturePos;

        switch (type) {
            case Type::Cherry:
                tag = "cherry";
                texturePos = {0, 0};
                break;
            case Type::Strawberry:
                tag = "strawberry";
                texturePos = {0, 1};
                break;
            case Type::Peach:
                tag = "peach";
                texturePos = {0, 2};
                break;
            case Type::Apple:
                tag = "apple";
                texturePos = {0, 3};
                break;
            case Type::Melon:
                tag = "melon";
                texturePos = {0, 4};
                break;
            case Type::Galaxian:
                tag = "galaxian";
                texturePos = {0, 5};
                break;
            case Type::Bell:
                tag = "bell";
                texturePos = {0, 6};
                break;
            case Type::Key:
                tag = "key";
                texturePos = {0, 7};
                break;
        }

        setTag(tag);

        static const ime::SpriteSheet spritesheet_{"spritesheet.png", ime::Vector2u{16, 16}, ime::Vector2u{1, 1}, {51, 51, 137, 18}};
        getSprite() = spritesheet_.getSprite(texturePos);
        resetSpriteOrigin();
        getSprite().scale(2.0f, 2.0f);
    }

    ///////////////////////////////////////////////////////////////
    std::string Fruit::getClassName() const {
        return "Fruit";
    }

} // namespace pm
