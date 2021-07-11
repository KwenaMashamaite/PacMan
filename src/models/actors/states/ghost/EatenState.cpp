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

#include "src/models/actors/states/ghost/EatenState.h"
#include "src/models/actors/states/ghost/ChaseState.h"
#include "src/models/actors/states/ghost/ScatterState.h"
#include "src/models/actors/Ghost.h"
#include "src/common/Constants.h"
#include "src/utils/Utils.h"
#include <cassert>

namespace pm {
    ///////////////////////////////////////////////////////////////
    EatenState::EatenState(ActorStateFSM* fsm, Ghost* target, GhostGridMover* gridMover, Ghost::State nextState) :
        GhostState(fsm),
        destFoundHandler_{-1},
        adjMoveEndHandler_{-1},
        nextState_{nextState}
    {
        setTarget(target);
        setGridMover(gridMover);

        assert((nextState == Ghost::State::Scatter || nextState == Ghost::State::Chase) && "Invalid regeneration transition state");
    }

    ///////////////////////////////////////////////////////////////
    void EatenState::onEntry() {
        assert(ghost_ && "Cannot enter Eaten state without a ghost");
        assert(ghostMover_ && "Cannot enter Eaten state without a ghost grid mover");

        ghost_->setState(static_cast<int>(Ghost::State::Eaten));

        if (ghostMover_->isTargetMoving()) {
            adjMoveEndHandler_ = ghostMover_->onAdjacentMoveEnd([this] (ime::Index) {
                ghostMover_->unsubscribe(adjMoveEndHandler_);
                ghostMover_->setDestination(Constants::EATEN_GHOST_RESPAWN_TILE);
            });
        } else
            ghostMover_->setDestination(Constants::EATEN_GHOST_RESPAWN_TILE);

        ghostMover_->startMovement();

        destFoundHandler_ = ghostMover_->onDestinationReached([this](ime::Index) {
            fsm_->pop();
        });
    }

    ///////////////////////////////////////////////////////////////
    void EatenState::handleEvent(GameEvent event, const ime::PropertyContainer &args) {
        if (event == GameEvent::ChaseModeEnd)
            nextState_ = Ghost::State::Scatter;
        else if (event == GameEvent::ScatterModeEnd)
            nextState_ = Ghost::State::Chase;
    }

    ///////////////////////////////////////////////////////////////
    void EatenState::onExit() {
        ghostMover_->unsubscribe(destFoundHandler_);

        if (nextState_ == Ghost::State::Chase)
            fsm_->push(std::make_unique<ChaseState>(fsm_, ghost_, ghostMover_));
        else
            fsm_->push(std::make_unique<ScatterState>(fsm_, ghost_, ghostMover_));
    }

} // namespace pm
