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

#ifndef PACMAN_IACTORSTATE_H
#define PACMAN_IACTORSTATE_H

#include "src/common/GameEvents.h"
#include <IME/core/time/Time.h>
#include <IME/common/PropertyContainer.h>

namespace pm {
    /**
     * @brief Interface for actor states
     */
    class IActorState {
    public:
        /**
         * @brief Initialize the state
         *
         * This function is called when the state is entered
         */
        virtual void onEntry() = 0;

        /**
         * @brief Pause the state
         */
        virtual void onPause() = 0;

        /**
         * @brief Resume the state
         */
        virtual void onResume() = 0;

        /**
         * @brief Exit a state
         *
         * This function is called when the state is exited
         */
        virtual void onExit() = 0;

        /**
         * @brief update the state
         * @param deltaTime Time passed since last state update
         */
        virtual void update(ime::Time deltaTime) = 0;

        /**
         * @brief Handle a game event
         * @param event The event to be handled
         * @param args Arguments associated with the event
         */
        virtual void handleEvent(GameEvent event, const ime::PropertyContainer& args) = 0;

        /**
         * @brief Destructor
         */
        virtual ~IActorState() = default;
    };
}

#endif
