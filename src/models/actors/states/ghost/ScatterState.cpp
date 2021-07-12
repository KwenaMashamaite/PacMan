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
    } // namespace anonymous

    ///////////////////////////////////////////////////////////////
    ScatterState::ScatterState(ActorStateFSM* fsm, Ghost* ghost, GhostGridMover* gridMover) :
        GhostState(fsm, ghost, gridMover),
        targetCorner_{UNKNOWN_CORNER},
        destFoundHandler_{-1}
    {}

    ///////////////////////////////////////////////////////////////
    void ScatterState::onEntry() {
        assert(ghost_ && "Cannot enter scatter state without a ghost");
        assert(ghostMover_ && "Cannot enter scatter state without a ghost grid mover");

        ghost_->setState(static_cast<int>(Ghost::State::Scatter));
        initEvents();
        setTargetPosition();
        ghostMover_->startMovement();
    }

    ///////////////////////////////////////////////////////////////
    void ScatterState::setTargetPosition() {
        if (ghost_->getTag() == "blinky")
            targetCorner_ = TOP_RIGHT_CORNER;
        else if (ghost_->getTag() == "pinky")
                targetCorner_ = TOP_LEFT_CORNER;
        else if (ghost_->getTag() == "inky")
                targetCorner_ = BOTTOM_RIGHT_CORNER;
        else if (ghost_->getTag() == "clyde")
                targetCorner_ = BOTTOM_LEFT_CORNER;
        else {
            assert(false && "Cannot generate cyclic path: Unknown ghost tag");
        }

        ghostMover_->setDestination(targetCorner_);
    }

    ///////////////////////////////////////////////////////////////
    void ScatterState::initEvents() {
        // Make ghost cycle corner after it finds its target corner
        destFoundHandler_ = ghostMover_->onDestinationReached([this](ime::Index) {
            if (path_.empty()) {
                if (targetCorner_ == TOP_LEFT_CORNER)
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
    }

    ///////////////////////////////////////////////////////////////
    void ScatterState::handleEvent(GameEvent event, const ime::PropertyContainer &args) {
        if (event == GameEvent::FrightenedModeBegin)
            fsm_->pop(std::make_unique<FrightenedState>(fsm_, ghost_, ghostMover_, Ghost::State::Scatter));
        else if (event == GameEvent::ChaseModeBegin)
            fsm_->pop(std::make_unique<ChaseState>(fsm_, ghost_, ghostMover_));
    }

    ///////////////////////////////////////////////////////////////
    void ScatterState::onExit() {
        ghostMover_->unsubscribe(destFoundHandler_);
        ghostMover_->clearPath();

        // ime::TargetGridMover::clearPath() does not reset the destination and
        // there is no other way to reset it --> IME v2.1.0. Since the tile that
        // is targeted in scatter mode is static, when the we transition to scatter
        // state from another state that did not change the destination, the
        // target will not move since the destination we are trying to set is
        // already set. A workaround is to set the destination to an unreachable
        // tile. This will cause the destination to change without generating a path
        ghostMover_->setDestination(ime::Index{0, 0});
    }

} // namespace pm
