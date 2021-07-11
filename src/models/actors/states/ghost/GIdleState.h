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

#ifndef PACMAN_GIDLESTATE_H
#define PACMAN_GIDLESTATE_H

#include "src/models/actors/states/ghost/GhostState.h"

namespace pm {
    /**
     * @brief A ghosts idle state
     *
     * In this state, the ghost cannot remains at its current position
     * and cannot move around the maze
     */
    class GIdleState final : public GhostState {
    public:
        /**
         * @brief Constructor
         * @param fsm The ghost finite state machine
         */
        explicit GIdleState(ActorStateFSM* fsm);

        /**
         * @brief Initialize the state
         */
        void onEntry() override;

        /**
         * @brief Handle a game event
         * @param event The event to be handled
         * @param args Event arguments
         */
        void handleEvent(GameEvent event, const ime::PropertyContainer &args) override;

        /**
         * @brief Exit a state
         *
         * This function will be called by the FSM before the state is
         * destroyed
         */
        void onExit() override;
    };
}

#endif
