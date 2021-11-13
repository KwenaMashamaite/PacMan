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

#include "ScatterState.h"
#include "ChaseState.h"
#include "FrightenedState.h"
#include "Utils/Utils.h"
#include "Common/Constants.h"
#include <cassert>

namespace pm {
    ///////////////////////////////////////////////////////////////
    ScatterState::ScatterState(ActorStateFSM* fsm, Ghost* ghost) :
        GhostState(fsm, ghost)
    {}

    ///////////////////////////////////////////////////////////////
    void ScatterState::onEntry() {
        assert(ghost_->getGridMover() && "Cannot enter scatter state without a ghost grid mover");
        auto* gridMover = dynamic_cast<GhostGridMover*>(ghost_->getGridMover());
        assert(gridMover && "Invalid ghost grid mover");

        ghost_->setState(static_cast<int>(Ghost::State::Scatter));
        ghost_->getSprite().getAnimator().startAnimation("going" + utils::convertToString(ghost_->getDirection()));

        gridMover->setMoveStrategy(GhostGridMover::Strategy::Target);

        if (ghost_->getTag() == "blinky")
            gridMover->setTargetTile(Constants::BLINKY_SCATTER_TARGET_TILE);
        else if (ghost_->getTag() == "pinky")
            gridMover->setTargetTile(Constants::PINKY_SCATTER_TARGET_TILE);
        else if (ghost_->getTag() == "inky")
            gridMover->setTargetTile(Constants::INKY_SCATTER_TARGET_TILE);
        else if (ghost_->getTag() == "clyde")
            gridMover->setTargetTile(Constants::CLYDE_SCATTER_TARGET_TILE);
        else {
            assert(false && "Failed to initialize ScatterState, unknown ghost tag");
        }

        gridMover->startMovement();
    }

    ///////////////////////////////////////////////////////////////
    void ScatterState::handleEvent(GameEvent event, const ime::PropertyContainer &args) {
        if (event == GameEvent::FrightenedModeBegin)
            fsm_->pop(std::make_unique<FrightenedState>(fsm_, ghost_, Ghost::State::Scatter));
        else if (event == GameEvent::ChaseModeBegin)
            fsm_->pop(std::make_unique<ChaseState>(fsm_, ghost_));
    }

} // namespace pm
