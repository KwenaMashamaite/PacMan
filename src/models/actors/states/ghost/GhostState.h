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

#ifndef PACMAN_GHOSTSTATE_H
#define PACMAN_GHOSTSTATE_H

#include "src/models/actors/states/IActorState.h"
#include "src/models/actors/states/ActorStateFSM.h"
#include "src/models/actors/controllers/GhostGridMover.h"
#include <IME/core/time/Timer.h>

namespace pm {
    class Ghost;

    /**
     * @brief Intermediate abstract base class for a ghost state
     *
     * This class implements functions that are common to all ghost states.
     * Its main purpose is to avoid code duplication across states
     */
    class GhostState : public IActorState {
    public:
        /**
         * @brief Constructor
         * @param fsm The ghost's finite state machine
         * @param ghost The ghost the ghost for the state
         * @param gridMover The ghosts grid mover
         */
        explicit GhostState(ActorStateFSM* fsm, Ghost* ghost = nullptr, GhostGridMover* gridMover = nullptr);

        /**
         * @brief Set the ghost for this state
         * @param ghost The ghost the ghost for the state
         *
         * @warning @a ghost must not be a nullptr
         */
        void setTarget(Ghost* ghost);

        /**
         * @brief Set the ghosts grid mover
         * @param gridMover The ghosts grid mover
         *
         * @warning @a gridMover must not be a nullptr
         */
        void setGridMover(GhostGridMover* gridMover);

        /**
         * @brief Update the state
         * @param deltaTime Time passed since last update
         */
        void update(ime::Time deltaTime) override {}

        /**
         * @brief Handle an event
         * @param event The event to be handled
         * @param args Event arguments
         */
        void handleEvent(GameEvent event, const ime::PropertyContainer &args) override {}

        /**
         * @brief Pause the state
         */
        void onPause() override {}

        /**
         * @brief Resume the state
         */
        void onResume() override {}

        /**
         * @brief Exit the state
         */
        void onExit() override {}

    protected:
        ActorStateFSM* fsm_;         //!< Ghost state controller
        Ghost* ghost_;               //!< Ghost whose behavior is to be defined by the state
        GhostGridMover* ghostMover_; //!< Ghost movement controller
    };
}

#endif //PACMAN_GHOSTSTATE_H
