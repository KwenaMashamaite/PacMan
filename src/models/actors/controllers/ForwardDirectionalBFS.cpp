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

#include "src/models/actors/controllers/ForwardDirectionalBFS.h"
#include "src/common/PositionTracker.h"
#include "src/models/actors/Wall.h"
#include <IME/core/tilemap/TileMap.h>
#include <cassert>
#include <iostream>

namespace pm {
    ///////////////////////////////////////////////////////////////
    ForwardDirectionalBFS::ForwardDirectionalBFS(const ime::Vector2u &gridSize, ime::GameObject* actor) :
        bfs_{gridSize},
        actor_{actor},
        wall_{nullptr}
    {
        assert(actor_);
        wall_ = std::make_unique<Wall>(actor_->getUserData().getValue<std::reference_wrapper<ime::Scene>>("scene"));
    }

    ///////////////////////////////////////////////////////////////
    std::stack<ime::Index> ForwardDirectionalBFS::findPath(ime::TileMap &grid, ime::Index sourceTile, ime::Index targetTile) {
        ime::Index actorTile;
        ime::Vector2i actorDirection;
        try {
            actorTile = PositionTracker::getPosition(actor_->getTag());
            actorDirection = PositionTracker::getDirection(actor_->getTag());
        } catch (...) {
            std::cout << "SPM Warning: Path generation ignored, the tag \"" + actor_->getTag() + "\" does not exists in spm::PositionTracker \n";
            return {};
        }

        assert(!(actorDirection.x == 0 && actorDirection.y == 0) && "Actor must have a valid direction before generating path");
        ime::Index tileBehindActor = {actorTile.row + (-1) * actorDirection.y, actorTile.colm + (-1) * actorDirection.x};

        // Flag the tile behind the actor as inaccessible
        grid.addChild(wall_.get(), tileBehindActor);

        std::stack<ime::Index> path = bfs_.findPath(grid, sourceTile, targetTile);

        // Unblock path
        grid.removeChild(wall_.get());

        return path;
    }

    ///////////////////////////////////////////////////////////////
    std::string ForwardDirectionalBFS::getType() const {
        return "ForwardDirectionalBFS";
    }

} // namespace pm
