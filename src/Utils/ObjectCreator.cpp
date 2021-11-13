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

#include "ObjectCreator.h"
#include "GameObjects/Actors.h"

namespace pm {
    ///////////////////////////////////////////////////////////////
    void ObjectCreator::createObjects(Grid &grid) {
        grid.forEachCell([&grid](const ime::Tile& tile) {
            ime::GameObject::Ptr actor;

            if (tile.getId() == 'X') {
                actor = std::make_unique<PacMan>(grid.getScene());
            } else if (tile.getId() == '|' || tile.getId() == '#') {
                actor = std::make_unique<Wall>(grid.getScene());
            } else if (tile.getId() == 'T' || tile.getId() == 'H' || tile.getId() == '$' || tile.getId() == '-') {
                actor = std::make_unique<Sensor>(grid.getScene());

                if (tile.getId() == 'T') {
                    actor->setTag("teleportationSensor");
                    actor->setCollisionGroup("teleportationSensor");
                } else if (tile.getId() == 'H') {
                    actor->setTag("tunnelEntrySensor");
                    actor->setCollisionGroup("tunnelEntrySensor");
                } else { // Tunnel exit sensor + Dot
                    actor->setTag("tunnelExitSensor");
                    actor->setCollisionGroup("tunnelExitSensor");

                    grid.addActor(std::move(actor), tile.getIndex());
                    grid.addActor(std::make_unique<Pellet>(grid.getScene(), Pellet::Type::Dot), tile.getIndex());
                    return;
                }
            } else if (tile.getId() == 'E')
                actor = std::make_unique<Pellet>(grid.getScene(), Pellet::Type::Energizer);
            else if (tile.getId() == 'D')
                actor = std::make_unique<Pellet>(grid.getScene(), Pellet::Type::Dot);
            else {
                if (tile.getId() == 'B')
                    actor = std::make_unique<Ghost>(grid.getScene(), Ghost::Colour::Red);
                else if (tile.getId() == 'P')
                    actor = std::make_unique<Ghost>(grid.getScene(), Ghost::Colour::Pink);
                else if (tile.getId() == 'I')
                    actor = std::make_unique<Ghost>(grid.getScene(), Ghost::Colour::Cyan);
                else if (tile.getId() == 'C')
                    actor = std::make_unique<Ghost>(grid.getScene(), Ghost::Colour::Orange);
                else
                    return;
            }

            grid.addActor(std::move(actor), tile.getIndex());
        });
    }

} // namespace pm
