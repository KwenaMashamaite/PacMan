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

#ifndef PACMAN_ACTORSTATEFSM_H
#define PACMAN_ACTORSTATEFSM_H

#include "IActorState.h"
#include <stack>
#include <memory>

namespace pm {
    /**
     * @brief Stack-based Finite State Machine (FSM) for IActorState
     */
    class ActorStateFSM {
    public:
        /**
         * @brief Default constructor
         */
        ActorStateFSM();

        /**
         * @brief Start the FSM
         *
         * Note that when started, the FSM will enter the state at the top of
         * the stack if any. In addition, subsequent state pushes will be entered
         * immediately after the push
         *
         * @see push
         */
        void start();

        /**
         * @brief Add a state to the FSM
         * @param state The state to be added
         *
         * The state will become the current active state, after its added
         *
         * @see pop and start
         */
        void push(std::unique_ptr<IActorState> state);

        /**
         * @brief Remove the current active state and optionally push a new one
         * @param state State to be pushed
         *
         * The @a state argument is optional and must only be provided if you
         * intend to pop the current state and immediately push another one.
         * When popping and pushing individually (calling pop() and immediately
         * after calling push()), the previous scene (if any) will be resumed
         * after the pop operation and then paused again after the push operation.
         * The @a state argument intends to prevent this momentary resume and
         * pause action. The @a state will be pushed and entered without resuming
         * and pausing the previous state
         *
         * @warning If the active state calls this function, any attempt
         * to access it afterwards is undefined behavior. In addition,
         * this function must not be called in pm::IActorState::onExit
         *
         * @see push
         */
        void pop(std::unique_ptr<IActorState> state = nullptr);

        /**
         * @brief Get the current active state
         * @return The current active state or a nullptr if the FSM has no states
         */
        IActorState* top();

        /**
         * @brief Get the number of states in the FSM
         * @return The number of states in the FSM
         */
        std::size_t getCount() const;

        /**
         * @brief Remove all states from the FSM
         */
        void clear();

    private:
        std::stack<std::unique_ptr<IActorState>> states_; //!< States container
        bool isStarted_;                                  //!< A flag indicating whether or not the FSM has been started
        bool isExitingState_;                             //!< A flag indicating whether or not the FSM is currently popping a state
    };
}

#endif //PACMAN_ACTORSTATEFSM_H
