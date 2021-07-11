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

#ifndef PACMAN_CHASESTATE_H
#define PACMAN_CHASESTATE_H

#include "src/models/actors/states/ghost/GhostState.h"

namespace pm {
    /**
     * @brief Defines the behavior of a ghost when it is chasing pacman
     */
    class ChaseState final : public GhostState {
    public:
        /**
         * @brief Construct state
         * @param fsm The ghost's Finite State Machine
         * @param target The ghost whose behaviour is to be defined by this state
         * @param gridMover The ghost's grid mover
         */
        ChaseState(ActorStateFSM* fsm, Ghost* target, GhostGridMover* gridMover);

        /**
         * @brief Initialize the state
         *
         * This function will be called by the FSM when a state is entered
         * for the first time
         */
        void onEntry() override;

        /**
         * @brief Handle a game event
         * @param event The event to be handled
         * @param args Arguments associated with the event
         */
        void handleEvent(GameEvent event, const ime::PropertyContainer &args) override;

        /**
         * @brief Pause the state
         */
        void onPause() override;

        /**
         * @brief Resume the state
         */
        void onResume() override;

        /**
         * @brief Exit a state
         *
         * This function will be called by the FSM before the state is
         * destroyed
         */
        void onExit() override;

    private:
        /**
         * @brief Init state events
         */
        void initEvents();

        /**
         * @brief Defines the chase strategy for each ghost
         */
        void chasePacman();
    };
}

#endif
