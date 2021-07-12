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

#include "src/models/actors/states/ghost/FrightenedState.h"
#include "src/models/actors/states/ghost/EatenState.h"
#include "src/models/actors/states/ghost/ScatterState.h"
#include "src/models/actors/states/ghost/ChaseState.h"
#include "src/models/actors/Ghost.h"
#include "src/utils/Utils.h"
#include <cassert>

namespace pm {
    ///////////////////////////////////////////////////////////////
    FrightenedState::FrightenedState(ActorStateFSM* fsm, Ghost* target, GhostGridMover* gridMover, Ghost::State nextState) :
        GhostState(fsm, target, gridMover),
        nextState_{nextState}
    {}

    ///////////////////////////////////////////////////////////////
    void FrightenedState::onEntry() {
        assert(ghost_ && "Cannot enter frightened state without a ghost");
        assert(ghostMover_ && "Cannot enter frightened state without a ghost grid mover");

        ghost_->setState(static_cast<int>(Ghost::State::Frightened));
        ghostMover_->setRandomMoveEnable(true);
        ghost_->getSprite().getAnimator().startAnimation("frightened");
    }

    ///////////////////////////////////////////////////////////////
    void FrightenedState::handleEvent(GameEvent event, const ime::PropertyContainer &args) {
        if (event == GameEvent::FrightenedModeEnd) {
            if (nextState_ == Ghost::State::Scatter)
                fsm_->pop(std::make_unique<ScatterState>(fsm_, ghost_, ghostMover_));
            else if (nextState_ == Ghost::State::Chase)
                fsm_->pop(std::make_unique<ChaseState>(fsm_, ghost_, ghostMover_));
            else {
                assert(false && "Ghost can only transition to scatter or chase state after it was frightened");
            }
        } else if (event == GameEvent::GhostEaten)
            fsm_->pop(std::make_unique<EatenState>(fsm_, ghost_, ghostMover_, nextState_));
    }

    ///////////////////////////////////////////////////////////////
    void FrightenedState::onExit() {
        ghost_->getSprite().getAnimator().startAnimation("going" + utils::convertToString(ghost_->getDirection()));
        ghostMover_->setRandomMoveEnable(false);
    }

} // namespace pm
