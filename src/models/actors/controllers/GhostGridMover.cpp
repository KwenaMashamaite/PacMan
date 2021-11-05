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

#include "src/models/actors/controllers/GhostGridMover.h"
#include "src/models/actors/controllers/ForwardDirectionalBFS.h"
#include "src/models/actors/Ghost.h"
#include "src/common/PositionTracker.h"
#include <IME/core/event/EventDispatcher.h>
#include <cassert>
#include <algorithm>
#include <random>

namespace pm {
    ///////////////////////////////////////////////////////////////
    GhostGridMover::GhostGridMover(ime::TileMap& tileMap, Ghost* ghost) :
        ime::TargetGridMover(tileMap, ghost),
        moveFinishId_{-1},
        stateChangeId_{-1},
        reverseDirectionNow_{false},
        canReverseDirection_{false},
        isRandomMove_{false}
    {
        assert(ghost && "spm::GhostGridMover target must not be a nullptr");

        // Update the ghosts direction when the grid mover turns it
        onPropertyChange("direction", [ghost](const ime::Property& property) {
            ghost->setDirection(property.getValue<ime::Direction>());
        });

        // Reverse direction when state changes
        stateChangeId_ = ghost->onPropertyChange("state", [this](const ime::Property& state) {
            auto ghostState = static_cast<Ghost::State>(state.getValue<int>());
            if (ghostState == Ghost::State::Idle || ghostState == Ghost::State::Imprisoned || ghostState == Ghost::State::Eaten)
                return;
            else
                reverseDirectionNow_ = true;
        });

        // Make the ghost position accessible to interested parties
        PositionTracker::updatePosition(ghost->getTag(), getGrid().getTileOccupiedByChild(ghost).getIndex());
        PositionTracker::updateDirection(ghost->getTag(), ghost->getDirection());

        // Update the position when ghost moves to another tile
        onAdjacentMoveBegin([this, ghost](ime::Index index) {
            PositionTracker::updatePosition(ghost->getTag(), index);
            PositionTracker::updateDirection(ghost->getTag(), getDirection());
        });

        // Update the position and direction when the ghosts grid position is changed manually
        onTargetTileReset([this, ghost](ime::Index index) {
            PositionTracker::updatePosition(ghost->getTag(), index);
            PositionTracker::updateDirection(ghost->getTag(), getDirection());
        });

        // Prevent ghost from switching directions when chasing in chase mode
        static auto GlobalEmitter = ime::EventDispatcher::instance();
        int blockId = GlobalEmitter->onEvent("blockPath" + ghost->getTag(), ime::Callback<ime::Index>([this](ime::Index index) {
            getGrid().setCollidableByIndex(index, true);
        }));

        int unblockId = GlobalEmitter->onEvent("unblockPath" + ghost->getTag(), ime::Callback<ime::Index>([this](ime::Index index) {
            getGrid().setCollidableByIndex(index, false);
        }));

        // Unsubscribe global event listeners
        onDestruction([tag = ghost->getTag(), unblockId, blockId] {
            GlobalEmitter->removeEventListener("blockPath" + tag, blockId);
            GlobalEmitter->removeEventListener("unblockPath" + tag, unblockId);
        });

        setPathFinder(std::make_unique<ForwardDirectionalBFS>(tileMap.getSizeInTiles(), ghost, reverseDirectionNow_));
    }

    ///////////////////////////////////////////////////////////////
    void GhostGridMover::generateRandomDestination() {
        ime::Direction reverseGhostDir = PositionTracker::getDirection(getTarget()->getTag()) * -1;
        assert(reverseGhostDir != ime::Unknown && "A ghost must have a valid direction before initiating random movement");

        if (reverseDirectionNow_) {
            reverseDirectionNow_ = false;
            requestDirectionChange(reverseGhostDir);
            return;
        }

        // Ghost only allowed to move non-diagonally
        for (const auto& dir : {ime::Left, ime::Right, ime::Down, ime::Up}) {
            // Prevent ghost from going backwards
            if (!canReverseDirection_ && dir == reverseGhostDir)
                continue;

            directionAttempts_.emplace_back(dir);
        }

        // Randomize the directions so that the direction the ghost chooses
        // to go is not predictable
        auto static randomEngine = std::default_random_engine{std::random_device{}()};
        std::shuffle(directionAttempts_.begin(), directionAttempts_.end(), randomEngine);

        ime::Index currentTile = PositionTracker::getPosition(getTarget()->getTag());
        ime::Index targetTile {-1, -1};

        do {
            // Tried all possible non-reverse directions with no luck (ghost in stuck in a dead-end)
            // Attempt to reverse direction and go backwards. This an exception to the no reverse
            // direction rule. Without the exception, the game will be stuck in an infinite loop
            if (directionAttempts_.empty()) {
                requestDirectionChange(reverseGhostDir);
                break;
            }

            auto dir = directionAttempts_.back();
            directionAttempts_.pop_back(); // Prevent the same direction from being evaluated more than once

            targetTile.row = currentTile.row + dir.y;
            targetTile.colm = currentTile.colm + dir.x;

            if (!isBlocked(targetTile)) {
                directionAttempts_.clear();
                requestDirectionChange(dir);
                break;
            }

        } while(true);
    }

    ///////////////////////////////////////////////////////////////
    bool GhostGridMover::isBlocked(const ime::Index& targetTileIndex) const {
        bool isObstacleInFront = false;
        const ime::Tile& targetTile = getGrid().getTile(targetTileIndex);

        // The documentation of ime::TileMap::forEachChildInTile function
        // says it does bounds checking but doesn't :(  --> IME v2.1.0.
        if (targetTile.getIndex().colm < 0 || targetTile.getIndex().colm < 0)
            return true;

        getGrid().forEachChildInTile(targetTile, [&isObstacleInFront] (ime::GameObject* actor) {
            // Sadly, no way to terminate loop early :( --> IME v2.1.0.
            if (actor->isObstacle())
                isObstacleInFront = true;
        });

        return isObstacleInFront;
    }

    ///////////////////////////////////////////////////////////////
    void GhostGridMover::setRandomMoveEnable(bool enable) {
        // Random movement already enabled
        if (isRandomMove_ == enable)
            return;

        isRandomMove_ = enable;

        if (enable) {
            if (!isTargetMoving())
                generateRandomDestination(); // Generate initial random position

            // Generate a new random position when target finishes its current move
            moveFinishId_ = onAdjacentMoveEnd([this](ime::Index) {
                generateRandomDestination();
            });
        } else {
            unsubscribe(moveFinishId_);
            moveFinishId_ = -1;
        }
    }

    ///////////////////////////////////////////////////////////////
    bool GhostGridMover::isRandomMoveEnabled() const {
        return isRandomMove_;
    }

    ///////////////////////////////////////////////////////////////
    void GhostGridMover::setReverseDirEnable(bool reverse) {
        if (canReverseDirection_ == reverse)
            return;

        canReverseDirection_ = reverse;

        if (reverse)
            setPathFinder(std::make_unique<ime::BFS>(getGrid().getSizeInTiles()));
        else
            setPathFinder(std::make_unique<ForwardDirectionalBFS>(getGrid().getSizeInTiles(), getTarget(), reverseDirectionNow_));

        emitChange(ime::Property{"reverseDirEnable", canReverseDirection_});
    }

    ///////////////////////////////////////////////////////////////
    bool GhostGridMover::isReverseDirEnabled() const {
        return canReverseDirection_;
    }

    ///////////////////////////////////////////////////////////////
    GhostGridMover::~GhostGridMover() {
        if (ime::GameObject* ghost = getTarget(); ghost)
            ghost->unsubscribe("state", stateChangeId_);
    }

} // namespace pm
