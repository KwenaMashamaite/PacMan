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

#include "ChaseState.h"
#include "ScatterState.h"
#include "FrightenedState.h"
#include "src/common/ObjectReferenceKeeper.h"
#include "src/utils/Utils.h"
#include <cassert>

namespace pm {
    ///////////////////////////////////////////////////////////////
    ChaseState::ChaseState(ActorStateFSM* fsm, Ghost* target) :
        GhostState(fsm, target),
        adjMoveHandlerID_{-1}
    {}

    ///////////////////////////////////////////////////////////////
    void ChaseState::onEntry() {
        assert(ghost_ && "Cannot enter chase state without a ghost");
        assert(ghost_->getGridMover() && "Cannot enter scatter state without a ghost grid mover");
        auto* gridMover = dynamic_cast<GhostGridMover*>(ghost_->getGridMover());
        assert(gridMover && "Invalid ghost grid mover");

        ghost_->setState(static_cast<int>(Ghost::State::Chase));
        ghost_->getSprite().getAnimator().startAnimation("going" + utils::convertToString(ghost_->getDirection()));

        adjMoveHandlerID_ = gridMover->onAdjacentMoveEnd(std::bind(&ChaseState::chasePacman, this));
        gridMover->setMoveStrategy(GhostGridMover::Strategy::Target);
        gridMover->startMovement();
        chasePacman();
    }

    ///////////////////////////////////////////////////////////////
    void ChaseState::chasePacman() {
        ime::GameObject* pacman = ObjectReferenceKeeper::getActor("pacman");
        ime::Index pacmanTile = pacman->getGridMover()->getCurrentTileIndex();
        ime::Vector2i pacmanDir = pacman->getGridMover()->getDirection();

        if (ghost_->getTag() == "blinky")
            dynamic_cast<GhostGridMover*>(ghost_->getGridMover())->setTargetTile(pacmanTile);
        else if (ghost_->getTag() == "pinky")
            dynamic_cast<GhostGridMover*>(ghost_->getGridMover())->setTargetTile(ime::Index{pacmanTile.row + 4 * pacmanDir.y, pacmanTile.colm + 4 * pacmanDir.x});
        else if (ghost_->getTag() == "inky") {
            ime::Index blinkyTile = ObjectReferenceKeeper::getActor("blinky")->getGridMover()->getCurrentTileIndex();

            // Choose a position two tiles in front of pacman
            ime::Index pacmanTileOffset = ime::Index{pacmanTile.row + 2 * pacmanDir.y, pacmanTile.colm + 2 * pacmanDir.x};

            // Create a vector from chosen position to blinky's tile
            ime::Index pacmanTileOffsetToBlinkyVector = ime::Index{-1 * (pacmanTileOffset.row - blinkyTile.row), -1 * (pacmanTileOffset.colm - blinkyTile.colm)};

            // Flip vector 180 degrees
            ime::Index inkyTargetTile = ime::Index{pacmanTileOffset.row - pacmanTileOffsetToBlinkyVector.row, pacmanTileOffset.colm - pacmanTileOffsetToBlinkyVector.colm};

            static_cast<GhostGridMover*>(ghost_->getGridMover())->setTargetTile(inkyTargetTile);

        } else if (ghost_->getTag() == "clyde") {
            const static int CLYDE_SHYNESS_DISTANCE = 8; // Distance in tiles not pixels
            ime::Index clydeTile = ghost_->getGridMover()->getCurrentTileIndex();
            if ((std::abs(pacmanTile.row - clydeTile.row) > CLYDE_SHYNESS_DISTANCE) ||
                (std::abs(pacmanTile.colm - clydeTile.colm) > CLYDE_SHYNESS_DISTANCE))
            {
                auto* gridMover = dynamic_cast<GhostGridMover*>(ghost_->getGridMover());
                gridMover->setMoveStrategy(GhostGridMover::Strategy::Target);
                gridMover->setTargetTile(pacmanTile);
            } else
                dynamic_cast<GhostGridMover*>(ghost_->getGridMover())->setMoveStrategy(GhostGridMover::Strategy::Random);
        } else {
            assert("Failed to create ghost chase strategy: Invalid tag");
        }
    }

    ///////////////////////////////////////////////////////////////
    void ChaseState::handleEvent(GameEvent event, const ime::PropertyContainer &args) {
        if (event == GameEvent::FrightenedModeBegin /*&& !ghost_->getUserData().getValue<bool>("is_in_ghost_house")*/)
            fsm_->pop(std::make_unique<FrightenedState>(fsm_, ghost_, Ghost::State::Chase));
        else if (event == GameEvent::ScatterModeBegin)
            fsm_->pop(std::make_unique<ScatterState>(fsm_, ghost_));
    }

    ///////////////////////////////////////////////////////////////
    void ChaseState::onExit() {
        ghost_->getGridMover()->unsubscribe(adjMoveHandlerID_);
    }

} // namespace pm
