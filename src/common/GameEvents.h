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

#ifndef PACMAN_GAMEEVENTS_H
#define PACMAN_GAMEEVENTS_H

namespace pm {
    /**
     * @brief Events that take place during gameplay
     */
    enum class GameEvent {
        EnergizeModeBegin, //!< Occurs when pacman eats an energizer pellet
        EnergizeModeEnd,   //!< Occurs when the energizer runs out
        PacManDied,        //!< Occurs when pacman collides with a ghost and is not in super mode
        PacManMoved,       //!< Occurs when pacman moves from one tile to another
        GhostEaten,        //!< Occurs when a ghost is eaten by pacman
        GhostRespawned,    //!< Occurs when an eaten ghost is revived
        LevelStarted,      //!< Fired when a level is started for the first time or when it is restarted
    };
}

#endif //PACMAN_GAMEEVENTS_H
