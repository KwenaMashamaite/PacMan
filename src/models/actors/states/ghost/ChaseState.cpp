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

#include "src/models/actors/states/ghost/ChaseState.h"
#include "src/models/actors/states/ghost/ScatterState.h"
#include "src/models/actors/states/ghost/FrightenedState.h"
#include "src/models/actors/Ghost.h"
#include "src/common/PositionTracker.h"
#include "src/utils/Utils.h"
#include <cassert>
#include <stack>

namespace pm {
    ///////////////////////////////////////////////////////////////
    ChaseState::ChaseState(ActorStateFSM* fsm, Ghost* target, GhostGridMover* gridMover) :
        GhostState(fsm, target, gridMover)
    {}

    ///////////////////////////////////////////////////////////////
    void ChaseState::onEntry() {
        assert(ghost_ && "Cannot enter chase state without a ghost");
        assert(ghostMover_ && "Cannot enter chase state without a ghost grid mover");

        ghost_->setState(static_cast<int>(Ghost::State::Chase));
        ghost_->getSprite().getAnimator().startAnimation("going" + utils::convertToString(ghost_->getDirection()));
        initEvents();
        chasePacman();
    }

    ///////////////////////////////////////////////////////////////
    void ChaseState::initEvents() {
        // Make ghost wonder around if it can't find a path to its target tile
        ghostMover_->onPathGenFinish([this](const std::stack<ime::Index>& path) {
            if (path.empty())
                ghostMover_->setRandomMoveEnable(true);
            else if (ghostMover_->isRandomMoveEnabled())
                ghostMover_->setRandomMoveEnable(false);
        });
    }

    ///////////////////////////////////////////////////////////////
    void ChaseState::chasePacman() {
        ime::Index pacmanTile = PositionTracker::getPosition("pacman");
        ime::Vector2i pacmanDir = PositionTracker::getDirection("pacman");

        if (ghost_->getTag() == "blinky")
            ghostMover_->setDestination(pacmanTile);
        else if (ghost_->getTag() == "pinky")
            ghostMover_->setDestination(ime::Index{pacmanTile.row + 4 * pacmanDir.y, pacmanTile.colm + 4 * pacmanDir.x});
        else if (ghost_->getTag() == "inky") {
            ime::Index blinkyTile = PositionTracker::getPosition("blinky");
            // Choose a position two tiles in front of pacman
            ime::Index pacmanTileOffset = ime::Index{pacmanTile.row + 2 * pacmanDir.y, pacmanTile.colm + 2 * pacmanDir.x};

            // Create a vector from chosen position to blinky's tile
            ime::Index pacmanTileOffsetToBlinkyVector = ime::Index{-1 * (pacmanTileOffset.row - blinkyTile.row), -1 * (pacmanTileOffset.colm - blinkyTile.colm)};

            // Flip vector 180 degrees
            ime::Index inkyTargetTile = ime::Index{pacmanTileOffset.row - pacmanTileOffsetToBlinkyVector.row, pacmanTileOffset.colm - pacmanTileOffsetToBlinkyVector.colm};

            // If the target tile lies outside the bounds of the tilemap, pm::GhostMover::setDestination()
            // does nothing. As a result, inky will just sit there and not move. To counteract that, we
            // activate random movement
            if (ghostMover_->getGrid().isIndexValid(inkyTargetTile))
                ghostMover_->setDestination(inkyTargetTile);
            else
                ghostMover_->setRandomMoveEnable(true);

        } else if (ghost_->getTag() == "clyde") {
            const static int CLYDE_SHYNESS_DISTANCE = 8; // Distance in tiles not pixels
            ime::Index clydeTile = ghostMover_->getGrid().getTileOccupiedByChild(ghost_).getIndex();
            if ((std::abs(pacmanTile.row - clydeTile.row) > CLYDE_SHYNESS_DISTANCE) ||
                (std::abs(pacmanTile.colm - clydeTile.colm) > CLYDE_SHYNESS_DISTANCE))
            {
                ghostMover_->setDestination(pacmanTile);
            } else
                ghostMover_->setRandomMoveEnable(true);
        } else {
            assert("Failed to create ghost chase strategy: Invalid tag");
        }

        ghostMover_->startMovement();
    }

    ///////////////////////////////////////////////////////////////
    void ChaseState::handleEvent(GameEvent event, const ime::PropertyContainer &args) {
        if (event == GameEvent::PacManMoved)
            chasePacman();
        else if (event == GameEvent::FrightenedModeBegin && !ghost_->getUserData().getValue<bool>("is_in_ghost_house"))
            fsm_->pop(std::make_unique<FrightenedState>(fsm_, ghost_, ghostMover_, Ghost::State::Chase));
        else if (event == GameEvent::ScatterModeBegin)
            fsm_->pop(std::make_unique<ScatterState>(fsm_, ghost_, ghostMover_));
    }

    ///////////////////////////////////////////////////////////////
    void ChaseState::onExit() {
        ghostMover_->resetDestination();
        ghostMover_->onPathGenFinish(nullptr);
        ghostMover_->setRandomMoveEnable(false);
    }

} // namespace pm
