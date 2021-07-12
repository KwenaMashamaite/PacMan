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

#ifndef PACMAN_SCATTERSTATE_H
#define PACMAN_SCATTERSTATE_H

#include "src/models/actors/states/ghost/GhostState.h"
#include <queue>

namespace pm {
    class Ghost;

    /**
     * @brief Defines the behavior of a ghost in scatter mode
     *
     * In this state the ghost moves to a specific corner thereafter follows a
     * cyclic path at the corner. Depending on the duration of the state the
     * ghost may not cycle a corner as the state might timeout while the ghost
     * is en route to corner. This state is triggered between pacman chases in
     * order to give the player a breather
     */
    class ScatterState final : public GhostState {
    public:
        /**
         * @brief Construct state
         * @param fsm The ghost's Finite State Machine
         * @param target The ghost whose behaviour is to be defined by this state
         * @param gridMover The ghost's grid mover
         */
        ScatterState(ActorStateFSM* fsm, Ghost* ghost, GhostGridMover* gridMover);

        /**
         * @brief Initialize the state
         *
         * This function will be called by the FSM before a state is entered
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
         * @brief Initialize the state
         *
         * This function will be called by the FSM before a state is entered
         * for the first time
         */
        void onExit() override;

    private:
        /**
         * @brief Sets the position of the corner the ghost must go to
         */
        void setTargetPosition();

        /**
         * @brief Initialize on destination reached handler
         */
        void initEvents();

    private:
        ime::Index targetCorner_;     //!< The position the ghost must reach first before cycling corner
        std::queue<ime::Index> path_; //!< Cyclic path ghost follows after reaching target position
        int destFoundHandler_;        //!< Handler id for a target destination event
    };
}

#endif
