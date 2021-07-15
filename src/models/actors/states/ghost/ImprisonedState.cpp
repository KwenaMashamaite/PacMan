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

#include "src/models/actors/states/ghost/ImprisonedState.h"
#include "src/models/actors/Ghost.h"
#include "src/common/Constants.h"
#include "src/common/PositionTracker.h"
#include "src/models/actors/states/ghost/ChaseState.h"
#include "src/models/actors/states/ghost/ScatterState.h"
#include <cassert>

namespace pm {
    // Starting point of cyclic ghost house path
    const auto PINKY_GHOST_HOUSE_START_POS = ime::Index{Constants::PINKY_SPAWN_TILE.row - 1, Constants::PINKY_SPAWN_TILE.colm};
    const auto INKY_GHOST_HOUSE_START_POS = ime::Index{Constants::INKY_SPAWN_TILE.row - 1, Constants::INKY_SPAWN_TILE.colm};
    const auto CLYDE_GHOST_HOUSE_START_POS = ime::Index{Constants::CLYDE_SPAWN_TILE.colm - 1, Constants::CLYDE_SPAWN_TILE.colm};

    // Ghost house cyclic paths
    const auto PINKY_GHOST_HOUSE_PATH = std::queue<ime::Index>({Constants::PINKY_SPAWN_TILE, PINKY_GHOST_HOUSE_START_POS});
    const auto INKY_GHOST_HOUSE_PATH = std::queue<ime::Index>({Constants::INKY_SPAWN_TILE, INKY_GHOST_HOUSE_START_POS});
    const auto CLYDE_GHOST_HOUSE_PATH = std::queue<ime::Index>({Constants::CLYDE_SPAWN_TILE, CLYDE_GHOST_HOUSE_START_POS});

    ///////////////////////////////////////////////////////////////
    ImprisonedState::ImprisonedState(ActorStateFSM *fsm, Ghost *target, GhostGridMover *gridMover) :
        GhostState(fsm, target, gridMover),
        destFoundHandler_{-1}
    {}

    ///////////////////////////////////////////////////////////////
    void ImprisonedState::onEntry() {
        ghost_->setState(static_cast<int>(Ghost::State::Imprisoned));
        ghostMover_->setReverseDirEnable(true);
        ime::Index position = PositionTracker::getPosition(ghost_->getTag());
        ghostMover_->setDestination(ime::Index{position.row - 1, position.colm});

        // Make ghost switch between the same two tiles
        destFoundHandler_ = ghostMover_->onDestinationReached([this](ime::Index dest) {
            ime::Vector2i dir = ghost_->getDirection();
            ghostMover_->setDestination(ime::Index{dest.row + (-1) * dir.y, dest.colm});
        });
    }

    ///////////////////////////////////////////////////////////////
    void ImprisonedState::handleEvent(GameEvent event, const ime::PropertyContainer &args) {
        if (event == GameEvent::LevelStarted)
            ghostMover_->startMovement();
        else if (event == GameEvent::GhostFreed) {
            auto newState = args.getValue<Ghost::State>("nextState");

            auto switchState = [this, newState] {
                if (newState == Ghost::State::Scatter)
                    fsm_->pop(std::make_unique<ScatterState>(fsm_, ghost_, ghostMover_));
                else if (newState == Ghost::State::Chase)
                    fsm_->pop(std::make_unique<ChaseState>(fsm_, ghost_, ghostMover_));
                else {
                    assert(false && "Ghost can only transition to chase or scatter state after being released from the ghost house");
                }
            };

            ghostMover_->unsubscribe(destFoundHandler_);

            if (ghostMover_->isTargetMoving()) {
                destFoundHandler_ = ghostMover_->onDestinationReached([this, switchState](ime::Index) {
                    ghostMover_->unsubscribe(destFoundHandler_);
                    ghostMover_->setDestination(ime::Index{Constants::BLINKY_SPAWN_TILE.row + 1, Constants::BLINKY_SPAWN_TILE.colm});
                    destFoundHandler_ = ghostMover_->onDestinationReached([switchState](ime::Index) {
                        switchState();
                    });
                });
            } else {
                ghostMover_->setDestination(ime::Index{Constants::BLINKY_SPAWN_TILE.row + 1, Constants::BLINKY_SPAWN_TILE.colm});
                destFoundHandler_ = ghostMover_->onDestinationReached([switchState](ime::Index) {
                    switchState();
                });
            }
        }
    }

    ///////////////////////////////////////////////////////////////
    void ImprisonedState::onExit() {
        ghostMover_->unsubscribe(destFoundHandler_);
        ghostMover_->resetDestination();
        ghostMover_->setReverseDirEnable(false);
        ghost_->getUserData().setValue("is_locked_in_ghost_house", false);
    }
}
