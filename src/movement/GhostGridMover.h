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

#ifndef PACMAN_GHOSTGRIDMOVER_H
#define PACMAN_GHOSTGRIDMOVER_H

#include <IME/core/physics/grid/GridMover.h>
#include <vector>

namespace pm {
    class Ghost;

    /**
     * @brief Controls the movement of a ghost in the grid
     */
    class GhostGridMover : public ime::GridMover {
    public:
        /**
         * @brief The movement strategy
         */
        enum class Strategy {
            Random, //!< The Ghost chooses directions at random
            Target  //!< The Ghost targets a specific tile
        };

        /**
         * @brief Constructor
         * @param tileMap The grid the target is in
         * @param ghost Ghost to be moved in the tilemap
         */
        GhostGridMover(ime::TileMap& tileMap, Ghost* ghost);

        /**
         * @brief Set the movement strategy
         * @param strategy The new movement strategy
         *
         * By default the movement strategy is pm::GhostGridMover::Strategy::Random
         */
        void setMoveStrategy(Strategy strategy);

        /**
         * @brief Set the target tile
         * @param index The index of the target tile
         *
         * By default the target tile is the one at index{0, 0}
         */
        void setTargetTile(ime::Index index);

        /**
         * @brief Start the movement
         */
        void startMovement();

    private:
        /**
         * @brief Move the ghost
         */
        void move();

        /**
         * @brief Filter forbidden directions from a list of possible directions
         * @param reverseGhostDir The reverse of the ghosts current direction
         */
        void initPossibleDirections(const ime::Direction& reverseGhostDir);

        /**
         * @brief Get the direction to move in
         * @return The next direction based on the movement strategy
         */
        ime::Direction getNextDirection();

    private:
        bool movementStarted_;                           //!< Flags if movement has been initiated or not
        Strategy moveStrategy_;                          //!< The current movement strategy of the ghost
        const ime::Tile* targetTile_;                    //!< The target tile to move to when move strategy is target
        std::vector<ime::Direction> possibleDirections_; //!< Stores directions to be attempted by randomly moving ghost
    };
}

#endif //PACMAN_GHOSTGRIDMOVER_H
