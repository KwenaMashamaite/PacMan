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

#ifndef PACMAN_CONSTANTS_H
#define PACMAN_CONSTANTS_H

#include <IME/core/tilemap/Index.h>

namespace pm {
    /**
     * @brief Stores data that doesn't change throughout the game
     */
    struct Constants {
        // 1. Non-gameplay constants
        static constexpr auto MAX_NUM_LEVEL_RESTARTS = 2;  //!< The number of times the player can restart a level before being forced to start from level 1

        // 2. Grid positions
        static inline const auto READY_TEXT_POSITION = ime::Index{17, 12}; //!< The position of the text shown during gameplay delay
        static inline auto PACMAN_SPAWN_TILE = ime::Index{22, 12};         //!< Pacman's spawn position when a level starts or restarts
        static inline auto BLINKY_SPAWN_TILE = ime::Index{12, 12};         //!< Blinky's spawn position when a level starts or restarts
        static inline auto PINKY_SPAWN_TILE = ime::Index{15, 12};          //!< Pinky's spawn position when a level starts or restarts
        static inline auto INKY_SPAWN_TILE = ime::Index{15, 11};           //!< Inky's spawn position when a level starts or restarts
        static inline auto CLYDE_SPAWN_TILE = ime::Index{15, 13};          //!< Clyde's spawn position when a level starts or restarts
        static inline auto EATEN_GHOST_RESPAWN_TILE = ime::Index{15, 12};  //!< The tile a ghost targets after it is eaten (Once it reaches this tile, it gets revived)

        // 3. Player
        static constexpr auto PLAYER_LiVES = 3;     // The initial number of player lives
        static constexpr auto PACMAN_SPEED = 100;   // The pacmans speed when moving in the grid

        // 4. Ghost
        static constexpr auto ENERGIZE_MODE_DURATION = 7.0f;           //!< Initial duration of energizer effects on ghosts
        static constexpr auto SCATTER_MODE_DURATION = 7.0f;            //!< The initial duration of a ghosts scatter mode
        static constexpr auto CHASE_MODE_DURATION = 7.0f;              //!< The initial duration of a ghosts chase mode
        static constexpr auto PINKY_HOUSE_ARREST_DURATION = 7.0f;      //!< Time spent by pinky in the ghost house before entering the maze
        static constexpr auto INKY_HOUSE_ARREST_DURATION = 14.0f;      //!< Time spent by inky in the ghost house before entering the maze
        static constexpr auto CLYDE_HOUSE_ARREST_DURATION = 21.0f;     //!< Time spent by clyde in the ghost house before entering the maze

        // 5. Misc
        static constexpr auto GRID_TILE_SIZE = 20;      // The size of each grid cell
        static constexpr auto LEVEL_START_DELAY = 2.0f; //!< The time (in seconds) the player must wait before they can control pacman

        /**
         * @brief Points awarded to the player when pacman eats another actor
         */
        struct Points {
            static constexpr int DOT = 10;         //!< Points awarded to the player when they eat a dot
            static constexpr int ENERGIZER = 50;   //!< Points awarded to the player when they eat an energizer
        }; // struct Points

    }; // struct Constants
} // namespace pm

#endif
