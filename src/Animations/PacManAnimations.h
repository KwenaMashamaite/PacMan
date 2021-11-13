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

#ifndef PACMAN_PACMANANIMATIONS_H
#define PACMAN_PACMANANIMATIONS_H

#include <IME/core/animation/Animation.h>
#include <vector>

namespace pm {
    /**
     * @brief Constructs all the pacman Animations
     */
    class PacManAnimations {
    public:
        /**
         * @brief Constructor
         */
        PacManAnimations();

        /**
         * @brief Create the pacman Animations
         */
        void create();

        /**
         * @brief Get all of the created Animations
         * @return A list of all the pacman Animations
         */
        const std::vector<ime::Animation::Ptr>& getAll() const;

    private:
        /**
         * @brief Create a PathFinders animation animation
         * @param name Name of the animation
         * @param index Index of the frame in the spritesheet
         */
        void createAnimation(const std::string& name, ime::Index index);

    private:
        std::vector<ime::Animation::Ptr> animations_;
        ime::SpriteSheet movementSpritesheet_;
        ime::SpriteSheet deathSpritesheet_;
    };
}

#endif
