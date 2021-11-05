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

#include "src/models/actors/controllers/PacManGridMover.h"
#include "src/models/actors/PacMan.h"
#include "src/common/PositionTracker.h"
#include "src/common/Constants.h"
#include <cassert>

namespace pm {
    ///////////////////////////////////////////////////////////////
    PacManGridMover::PacManGridMover(ime::TileMap &grid, PacMan* pacman) :
        ime::KeyboardGridMover(grid, pacman),
        pacmanStateChangeId_{-1},
        pendingDirection_{ime::Unknown}
    {
        assert(pacman && "Cannot create pacman's grid mover with a nullptr");
        setTag("pacmanGridMover");
    }

    ///////////////////////////////////////////////////////////////
    void PacManGridMover::init() {
        setKeys(ime::TriggerKeys{ime::Key::A, ime::Key::D, ime::Key::W, ime::Key::S});
        setMaxLinearSpeed({Constants::PACMAN_SPEED, Constants::PACMAN_SPEED});

        auto* pacman = static_cast<PacMan*>(getTarget());

        if (pacman->getState() == PacMan::State::Idle)
            setMovementRestriction(ime::GridMover::MoveRestriction::All);

        PositionTracker::updatePosition(pacman->getTag(), getCurrentTileIndex());
        PositionTracker::updateDirection(pacman->getTag(), pacman->getDirection());

        // Update the position when pacman moves to another tile
        onAdjacentMoveBegin([this, pacman](ime::Index index) {
            PositionTracker::updatePosition(pacman->getTag(), index);
            PositionTracker::updateDirection(pacman->getTag(), getDirection());
        });

        // Update the position and direction when pacmans grid position is changed manually
        onTargetTileReset([this, pacman](ime::Index index) {
            PositionTracker::updatePosition(pacman->getTag(), index);
            PositionTracker::updateDirection(pacman->getTag(), getDirection());
        });

        // Keep pacman moving until he collides with a wall
        onAdjacentMoveEnd([this, pacman](ime::Index) {
            if (pendingDirection_ != ime::Unknown && !isBlockedInDirection(pendingDirection_).first) {
                requestDirectionChange(pendingDirection_);
                pendingDirection_ = ime::Unknown;
            } else
                requestDirectionChange(pacman->getDirection());
        });

        // Prevent pacmam from moving down when above the ghost house gate
        onAdjacentMoveBegin([this](ime::Index index) {
            if (index == Constants::BLINKY_SPAWN_TILE)
                setMovementRestriction(ime::GridMover::MoveRestriction::Horizontal);
            else
                setMovementRestriction(ime::GridMover::MoveRestriction::None);
        });

        // Prevent pacman from turning into a direction that causes a collision with an obstacle
        onInput([this](ime::Keyboard::Key key) {
            ime::Direction newDir;

            if (key == getTriggerKeys().rightKey)
                newDir = ime::Right;
            else if (key == getTriggerKeys().leftKey)
                newDir = ime::Left;
            else if (key == getTriggerKeys().upKey)
                newDir = ime::Up;
            else
                newDir = ime::Down;

            if (!isBlockedInDirection(newDir).first && !isTargetMoving()) {
                pendingDirection_ = ime::Unknown;
                return true;
            } else
                pendingDirection_ = newDir;

            return false;
        });

        // Move or stop pacman depending on his current state
        pacmanStateChangeId_ = pacman->onPropertyChange("state", [this, pacman](const ime::Property& property) {
            switch (static_cast<PacMan::State>(property.getValue<int>())) {
                case PacMan::State::Idle:
                    setMovementRestriction(ime::GridMover::MoveRestriction::All);
                    break;
                case PacMan::State::Moving:
                    setMovementRestriction(ime::GridMover::MoveRestriction::NonDiagonal);

                    // In case his speed was reset while his movement was restricted
                    setMaxLinearSpeed({Constants::PACMAN_SPEED, Constants::PACMAN_SPEED});
                    requestDirectionChange(pacman->getDirection());
                    break;
                case PacMan::State::Dying:
                    setMovementRestriction(ime::GridMover::MoveRestriction::All);
                    break;
            }
        });

        // Update pacmans direction when the grid mover turns him
        onPropertyChange("direction", [pacman](const ime::Property& property) {
            pacman->setDirection(property.getValue<ime::Direction>());
        });
    }

    ///////////////////////////////////////////////////////////////
    PacManGridMover::~PacManGridMover() {
        if (getTarget())
            getTarget()->unsubscribe("state", pacmanStateChangeId_);
    }
}
