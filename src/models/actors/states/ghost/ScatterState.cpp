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

#include "src/models/actors/states/ghost/ScatterState.h"
#include "src/models/actors/states/ghost/ChaseState.h"
#include "src/models/actors/states/ghost/FrightenedState.h"
#include "src/models/actors/Ghost.h"
#include "src/common/Constants.h"
#include <cassert>
#include <queue>

namespace pm {
    namespace {
        // Starting point of cyclic corner path
        const auto TOP_LEFT_CORNER = ime::Index{3, 5};
        const auto TOP_RIGHT_CORNER = ime::Index{3, 19};
        const auto BOTTOM_LEFT_CORNER = ime::Index{23, 7};
        const auto BOTTOM_RIGHT_CORNER = ime::Index{23, 17};
        const auto UNKNOWN_CORNER = ime::Index{-1, -1};

        // Cyclic paths at specific corners
        const auto TOP_LEFT_CORNER_PATH = std::queue<ime::Index>({ime::Index{7, 5}, ime::Index{3, 5}});
        const auto TOP_RIGHT_CORNER_PATH = std::queue<ime::Index>({ime::Index{7, 19}, ime::Index{3, 19}});
        const auto BOTTOM_LEFT_CORNER_PATH = std::queue<ime::Index>({ime::Index{27, 7}, ime::Index{23, 7}});
        const auto BOTTOM_RIGHT_CORNER_PATH = std::queue<ime::Index>({ime::Index{27, 17}, ime::Index{23, 17}});

        // Starting point of cyclic ghost house path
        const auto PINKY_GHOST_HOUSE_START_POS = ime::Index{Constants::PINKY_SPAWN_TILE.row - 1, Constants::PINKY_SPAWN_TILE.colm};
        const auto INKY_GHOST_HOUSE_START_POS = ime::Index{Constants::INKY_SPAWN_TILE.row - 1, Constants::INKY_SPAWN_TILE.colm};
        const auto CLYDE_GHOST_HOUSE_START_POS = ime::Index{Constants::CLYDE_SPAWN_TILE.colm - 1, Constants::CLYDE_SPAWN_TILE.colm};

        // Ghost house cyclic paths
        const auto PINKY_GHOST_HOUSE_PATH = std::queue<ime::Index>({Constants::PINKY_SPAWN_TILE, PINKY_GHOST_HOUSE_START_POS});
        const auto INKY_GHOST_HOUSE_PATH = std::queue<ime::Index>({Constants::INKY_SPAWN_TILE, INKY_GHOST_HOUSE_START_POS});
        const auto CLYDE_GHOST_HOUSE_PATH = std::queue<ime::Index>({Constants::CLYDE_SPAWN_TILE, CLYDE_GHOST_HOUSE_START_POS});
    } // namespace anonymous

    ///////////////////////////////////////////////////////////////
    ScatterState::ScatterState(ActorStateFSM* fsm, Ghost* ghost, GhostGridMover* gridMover) :
        GhostState(fsm, ghost, gridMover),
        targetCorner_{UNKNOWN_CORNER},
        destFoundHandler_{-1},
        valueChangeHandler_{-1},
        isLockedInGhostHouse_{false}
    {}

    ///////////////////////////////////////////////////////////////
    void ScatterState::onEntry() {
        assert(ghost_ && "Cannot enter scatter state without a ghost");
        assert(ghostMover_ && "Cannot enter scatter state without a ghost grid mover");

        ghost_->setState(static_cast<int>(Ghost::State::Scatter));
        isLockedInGhostHouse_ = ghost_->getUserData().getValue<bool>("is_locked_in_ghost_house");
        initEvents();
        setTargetPosition();
        ghostMover_->startMovement();
    }

    ///////////////////////////////////////////////////////////////
    void ScatterState::setTargetPosition() {
        if (ghost_->getTag() == "blinky")
            targetCorner_ = TOP_RIGHT_CORNER;
        else if (ghost_->getTag() == "pinky") {
            if (isLockedInGhostHouse_)
                targetCorner_ = PINKY_GHOST_HOUSE_START_POS;
            else
                targetCorner_ = TOP_LEFT_CORNER;
        } else if (ghost_->getTag() == "inky") {
            if (isLockedInGhostHouse_)
                targetCorner_ = INKY_GHOST_HOUSE_START_POS;
            else
                targetCorner_ = BOTTOM_RIGHT_CORNER;
        } else if (ghost_->getTag() == "clyde") {
            if (isLockedInGhostHouse_)
                targetCorner_ = CLYDE_GHOST_HOUSE_START_POS;
            else
                targetCorner_ = BOTTOM_LEFT_CORNER;
        } else {
            assert(false && "Cannot generate cyclic path: Unknown ghost tag");
        }

        ghostMover_->setDestination(targetCorner_);
    }

    ///////////////////////////////////////////////////////////////
    void ScatterState::clearPath() {
        if (!path_.empty()) {
            auto emptyQueue = std::queue<ime::Index>{};
            path_.swap(emptyQueue);
        }
    }

    ///////////////////////////////////////////////////////////////
    void ScatterState::initEvents() {
        // Make ghost cycle corner after it finds its target corner
        destFoundHandler_ = ghostMover_->onDestinationReached([this](ime::Index) {
            if (path_.empty()) {
                if (isLockedInGhostHouse_) {
                    if (ghost_->getTag() == "pinky")
                        path_ = PINKY_GHOST_HOUSE_PATH;
                    else if (ghost_->getTag() == "inky")
                        path_ = INKY_GHOST_HOUSE_PATH;
                    else if (ghost_->getTag() == "clyde")
                        path_ = CLYDE_GHOST_HOUSE_PATH;
                    else if (ghost_->getTag() == "blinky")
                        assert(false && "Failed to determine ghost house path: The red ghost \"blinky\" does not cycle the ghost house");
                    else
                        assert(false && "Failed to determine ghost house path:  invalid ghost tag");
                } else if (targetCorner_ == TOP_LEFT_CORNER)
                    path_ = TOP_LEFT_CORNER_PATH;
                else if (targetCorner_ == TOP_RIGHT_CORNER)
                    path_ = TOP_RIGHT_CORNER_PATH;
                else if (targetCorner_ == BOTTOM_LEFT_CORNER)
                    path_ = BOTTOM_LEFT_CORNER_PATH;
                else if (targetCorner_ == BOTTOM_RIGHT_CORNER)
                    path_ = BOTTOM_RIGHT_CORNER_PATH;
                else {
                    assert(false && "Failed to determine cyclic corner path: Invalid grid corner");
                }
            }

            ghostMover_->setDestination(path_.front());
            path_.pop();
        });

        // Automatically update ScatterState when the ghost's locked status changes.
        valueChangeHandler_ = ghost_->getUserData().onValueChange("is_locked_in_ghost_house", [this](ime::Property* const property) {
            auto value = property->getValue<bool>();
            if (isLockedInGhostHouse_ == value)
                return;

            isLockedInGhostHouse_ = value;
            clearPath();
            setTargetPosition();
        });
    }

    ///////////////////////////////////////////////////////////////
    void ScatterState::handleEvent(GameEvent event, const ime::PropertyContainer &args) {
        if (event == GameEvent::FrightenedModeBegin)
            fsm_->push(std::make_unique<FrightenedState>(fsm_, ghost_, ghostMover_, Ghost::State::Scatter));
        else if (event == GameEvent::ChaseModeBegin)
            fsm_->pop();
    }

    ///////////////////////////////////////////////////////////////
    void ScatterState::onPause() {
        ghostMover_->unsubscribe(destFoundHandler_);
        ghost_->getUserData().unsubscribe("is_locked_in_ghost_house", valueChangeHandler_);
        destFoundHandler_ = -1;
        valueChangeHandler_ = -1;

        ghostMover_->clearPath();
        targetCorner_ = UNKNOWN_CORNER;
        clearPath();
    }

    ///////////////////////////////////////////////////////////////
    void ScatterState::onResume() {
        ghost_->setState(static_cast<int>(Ghost::State::Scatter));
        initEvents();
        setTargetPosition();
    }

    ///////////////////////////////////////////////////////////////
    void ScatterState::onExit() {
        ghostMover_->unsubscribe(destFoundHandler_);
        ghost_->getUserData().unsubscribe("is_locked_in_ghost_house", valueChangeHandler_);
        ghostMover_->clearPath();

        std::unique_ptr<GhostState> nextState;

        // When scatter state timer expires, the state is automatically removed
        // from the FSM, however, if the ghost is still locked inside the ghost
        // house when the timer expires, we must remain in scatter mode instead
        // of initiating a pacman chase
        if (isLockedInGhostHouse_)
            nextState = std::make_unique<ScatterState>(fsm_, ghost_, ghostMover_);
        else
            nextState = std::make_unique<ChaseState>(fsm_, ghost_, ghostMover_);

        nextState->setTarget(ghost_);
        nextState->setGridMover(ghostMover_);
        fsm_->push(std::move(nextState));
    }

} // namespace pm
