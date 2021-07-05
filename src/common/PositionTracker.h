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

#ifndef PACMAN_POSITIONTRACKER_H
#define PACMAN_POSITIONTRACKER_H

#include <IME/core/tilemap/Index.h>
#include <IME/common/Vector2.h>
#include <string>
#include <unordered_map>
#include <utility>

namespace pm {
    /**
     * @brief Tracks the position and direction of an actor in the grid using
     *        its tag
     */
    class PositionTracker {
    public:
        /**
         * @brief Update the position of the actor
         * @param tag The tag of the actor whose position is to be updated
         * @param index The new position of the actor
         *
         * Note that this function will add the @a tag if it does not exist
         *
         * @see updateDirection
         */
        static void updatePosition(const std::string& tag, const ime::Index& index);

        /**
         * @brief Update the direction of the actor
         * @param tag The tag of the actor whose direction is to be updated
         * @param dir The new direction of the actor
         *
         * Note that this function will add the @a tag if it does not exist
         *
         * @see updatePosition
         */
        static void updateDirection(const std::string& tag, const ime::Vector2i& dir);

        /**
         * @brief Get the position of an actor
         * @param tag The tag of the actor
         * @return The position of the actor
         *
         * @warning The @a tag must exist before this function is called,
         * otherwise undefined behavior
         *
         * @see updatePosition and updateDirection
         */
        static ime::Index getPosition(const std::string& tag);

        /**
         * @brief Get the direction of an actor
         * @param tag The tag of the actor
         * @return The direction of the actor
         *
         * @warning The @a tag must exist before this function is called,
         * otherwise undefined behavior
         *
         * @see updatePosition and updateDirection
         */
        static ime::Vector2i getDirection(const std::string& tag);

    private:
        inline static std::unordered_map<std::string, std::pair<ime::Index, ime::Vector2i>> positions_{};
    };
}

#endif //PACMAN_POSITIONTRACKER_H
