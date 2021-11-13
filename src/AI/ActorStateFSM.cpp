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

#include "ActorStateFSM.h"
#include <cassert>

namespace pm {
    ///////////////////////////////////////////////////////////////
    ActorStateFSM::ActorStateFSM() :
        isStarted_{false},
        isExitingState_{false}
    {}

    ///////////////////////////////////////////////////////////////
    void ActorStateFSM::start() {
        if (!isStarted_) {
            assert(!states_.empty() && "Cannot start FSM without a state, add at least one state to the FSM");
            isStarted_ = true;
            states_.top()->onEntry();
        }
    }

    ///////////////////////////////////////////////////////////////
    void ActorStateFSM::push(std::unique_ptr<IActorState> state) {
        assert(state && "Cannot push a nullptr to the FSM");

        // Ignore IActorState::onPause() when ActorStateFSM::push() is called
        // inside IActorState::onExit() because it was called when the state
        // that is currently being exited was pushed to the FSM
        if (!states_.empty() && !isExitingState_)
            states_.top()->onPause();

        states_.push(std::move(state));

        if (isStarted_)
            states_.top()->onEntry();
    }

    ///////////////////////////////////////////////////////////////
    void ActorStateFSM::pop(std::unique_ptr<IActorState> state) {
        if (states_.empty())
            return;

        if (isExitingState_) {
            assert(false && "ActorStateFSM::onExit() must not call ActorStateFSM::pop()");
        }

        std::unique_ptr poppedState = std::move(states_.top());
        states_.pop();

        auto sizeBeforeOnExit = states_.size();

        isExitingState_ = true; // Prevent the state from performing a pop operation when it is being popped
        poppedState->onExit();

        if (state)
            push(std::move(state));

        isExitingState_ = false;

        if (states_.size() > sizeBeforeOnExit) // onExit(), pushed a new state, don't resume previous state
            return;

        if (!states_.empty())
            states_.top()->onResume();
    }

    ///////////////////////////////////////////////////////////////
    IActorState *ActorStateFSM::top() {
        return states_.empty() ? nullptr : states_.top().get();
    }

    ///////////////////////////////////////////////////////////////
    std::size_t ActorStateFSM::getCount() const {
        return states_.size();
    }

    ///////////////////////////////////////////////////////////////
    void ActorStateFSM::clear() {
        auto emptyStack = std::stack<std::unique_ptr<IActorState>>{};
        states_.swap(emptyStack);
    }

} // namespace pm
