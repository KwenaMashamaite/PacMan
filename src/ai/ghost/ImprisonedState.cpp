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

#include "ImprisonedState.h"
#include "src/models/actors/Ghost.h"
#include "src/common/Constants.h"
#include "src/common/ObjectReferenceKeeper.h"
#include "ChaseState.h"
#include "ScatterState.h"
#include <cassert>

namespace pm {
    // Starting point of cyclic ghost house path
    const auto PINKY_GHOST_HOUSE_START_POS = ime::Index{17, 16};
    const auto INKY_GHOST_HOUSE_START_POS = ime::Index{15, 14};
    const auto CLYDE_GHOST_HOUSE_START_POS = ime::Index{15, 18};

    // Ghost house cyclic paths
    const auto PINKY_GHOST_HOUSE_PATH = std::queue<ime::Index>({ime::Index{15, 16}, ime::Index{17, 16}});
    const auto INKY_GHOST_HOUSE_PATH = std::queue<ime::Index>({ime::Index{17, 14}, ime::Index{15, 14}});
    const auto CLYDE_GHOST_HOUSE_PATH = std::queue<ime::Index>({ime::Index{17, 18}, ime::Index{15, 18}});

    ///////////////////////////////////////////////////////////////
    ImprisonedState::ImprisonedState(ActorStateFSM *fsm, Ghost *target, GhostGridMover *gridMover) :
        GhostState(fsm, target, gridMover),
        destFoundHandler_{-1}
    {}

    ///////////////////////////////////////////////////////////////
    void ImprisonedState::onEntry() {
        ghost_->setState(static_cast<int>(Ghost::State::Imprisoned));
        ghostMover_->setReverseDirEnable(true);
        ime::Index position;

        if (ghost_->getTag() == "pinky")
            position = PINKY_GHOST_HOUSE_START_POS;
        else if (ghost_->getTag() == "inky")
            position = INKY_GHOST_HOUSE_START_POS;
        else if (ghost_->getTag() == "clyde")
            position = CLYDE_GHOST_HOUSE_START_POS;
        else {
            assert(false && "Only pinky, inky and clyde can be imprisoned");
        }

        ghostMover_->setDestination(position);

        // Make ghost switch between the same two tiles
        destFoundHandler_ = ghostMover_->onDestinationReached([this](ime::Index dest) {
            if (path_.empty()) {
                if (ghost_->getTag() == "pinky")
                    path_ = PINKY_GHOST_HOUSE_PATH;
                else if (ghost_->getTag() == "inky")
                    path_ = INKY_GHOST_HOUSE_PATH;
                else if (ghost_->getTag() == "clyde")
                    path_ = CLYDE_GHOST_HOUSE_PATH;
                else {
                    assert(false && "Failed to determine ghost house path: Invalid tag");
                }
            }

            ghostMover_->setDestination(path_.front());
            path_.pop();
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
