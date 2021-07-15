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

#include <IME/core/physics/grid/TargetGridMover.h>
#include <vector>

namespace pm {
    class Ghost;

    /**
     * @brief Controls the movement of a ghost in th grid
     */
    class GhostGridMover : public ime::TargetGridMover {
    public:
        /**
         * @brief Constructor
         * @param tileMap The grid the target is in
         * @param ghost Ghost to be moved in the tilemap
         */
        GhostGridMover(ime::TileMap& tileMap, Ghost* ghost);

        /**
         * @brief Enable or disable random movement
         * @param random True to enable or false to disable
         *
         * By default, random movement is disabled
         */
        void setRandomMoveEnable(bool enable);

        /**
         * @brief Check if random movement is enabled or not
         * @return True if enabled, otherwise false
         *
         * @see setRandomMoveEnable
         */
        bool isRandomMoveEnabled() const;

        /**
         * @brief Enable or disable direction reversing
         * @param reverse True to enable or false to disable
         *
         * When enabled, the ghost cannot go back to its previous tile.
         * In other words the ghost can only move in three directions if
         * diagonal movement is disabled or seven directions if diagonal
         * movement is enabled
         *
         * Note that this the ghost will reverse directions if it is stuck
         * in a dead-end
         *
         * By default, reversing direction is disabled
         *
         * @see setRandomMoveEnable
         */
        void setReverseDirEnable(bool reverse);

        /**
         * @brief Check if reversing direction is allowed or not
         * @return True if reversing direction is allowed, otherwise false
         *
         * @see setReverseDirEnable
         */
        bool isReverseDirEnabled() const;

        /**
         * @brief Destructor
         */
        ~GhostGridMover() override;

    private:
        /**
         * @brief Generate a random destination for the target
         */
        void generateRandomDestination();

        /**
         * @brief Check whether or not the actors path is blocked by an obstacle
         *        in the current direction
         * @param targetTileIndex The index of the tile the actor wants to occupy
         * @return True if actor is blocked by an obstacle, otherwise false
         */
        bool isBlocked(const ime::Index& targetTileIndex) const;

    private:
        int moveFinishId_;                              //!< Destination reached event handler id
        bool reverseDirectionNow_;                      //!< A flag indicating whether or not the ghost must reverse direction before moving
        bool canReverseDirection_;                      //!< A flag indicating whether or not the ghost can reverse directions when moving
        bool isRandomMove_;                             //!< A flag indicating whether or not the ghost is moving randomly
        std::vector<ime::Direction> directionAttempts_; //!< Stores directions to be attempted by randomly moving ghost
    };
}

#endif //PACMAN_GHOSTGRIDMOVER_H
