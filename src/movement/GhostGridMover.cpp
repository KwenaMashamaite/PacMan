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

#include "GhostGridMover.h"
#include "src/models/actors/Ghost.h"
#include <cassert>
#include <algorithm>
#include <random>
#include <limits>

namespace pm {
    ///////////////////////////////////////////////////////////////
    GhostGridMover::GhostGridMover(ime::TileMap& tileMap, Ghost* ghost) :
        ime::GridMover(tileMap, ghost),
        movementStarted_{false},
        moveStrategy_{Strategy::Random},
        targetTile_{nullptr}
    {
        assert(ghost && "spm::GhostGridMover target must not be a nullptr");

        onPropertyChange("direction", [ghost](const ime::Property& property) {
            ghost->setDirection(property.getValue<ime::Direction>());
        });

        onAdjacentMoveEnd(std::bind(&GhostGridMover::move, this));
        setTargetTile(ime::Index{0, 0});
    }

    ///////////////////////////////////////////////////////////////
    void GhostGridMover::move() {
        ime::Direction reverseGhostDir = getDirection() * -1;
        initPossibleDirections(reverseGhostDir);

        if (possibleDirections_.empty())
            requestDirectionChange(reverseGhostDir);
        else if (possibleDirections_.size() == 1)
            requestDirectionChange(possibleDirections_.front());
        else
            requestDirectionChange(getNextDirection());

        possibleDirections_.clear();
    }

    ///////////////////////////////////////////////////////////////
    void GhostGridMover::setMoveStrategy(GhostGridMover::Strategy strategy) {
        moveStrategy_ = strategy;
    }

    ///////////////////////////////////////////////////////////////
    void GhostGridMover::setTargetTile(ime::Index index) {
        assert(index.row >= 0 && index.row < getGrid().getSizeInTiles().y && "Row out of grid bounds");
        assert(index.colm >= 0 && index.colm < getGrid().getSizeInTiles().x && "Column out of grid bounds");
        targetTile_ = &getGrid().getTile(index);
    }

    ///////////////////////////////////////////////////////////////
    void GhostGridMover::startMovement() {
        if (!movementStarted_) {
            movementStarted_ = true;
            move();
        }
    }

    ///////////////////////////////////////////////////////////////
    void GhostGridMover::initPossibleDirections(const ime::Direction& reverseGhostDir) {
        for (const auto& dir : {ime::Up, ime::Left, ime::Down, ime::Right}) {
            if (dir == reverseGhostDir || isBlockedInDirection(dir).first)
                continue;

            possibleDirections_.emplace_back(dir);
        }
    }

    ///////////////////////////////////////////////////////////////
    ime::Direction GhostGridMover::getNextDirection() {
        if (moveStrategy_ == Strategy::Random) {
            auto static randomEngine = std::default_random_engine{std::random_device{}()};
            std::shuffle(possibleDirections_.begin(), possibleDirections_.end(), randomEngine);
            return possibleDirections_.front();
        } else if (moveStrategy_ == Strategy::Target) {
            std::vector<float> weights;

            for (const auto& dir : possibleDirections_) {
                weights.push_back(getGrid().getTile(ime::Index{getCurrentTileIndex().row + dir.y,
                    getCurrentTileIndex().colm + dir.x}).getWorldCentre().distanceTo(targetTile_->getWorldCentre()));
            }

            float minDistance = std::numeric_limits<float>::max();
            int index = -1;
            for (int i = 0; i < weights.size(); i++) {
                if (weights[i] < minDistance) {
                    minDistance = weights[i];
                    index = i;
                }
            }

            return index == -1 ? possibleDirections_.front() : possibleDirections_[index];
        }

        return ime::Unknown;
    }

} // namespace pm
