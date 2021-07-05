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

#include "src/utils/ObjectCreator.h"
#include "src/models/actors/Actors.h"

namespace pm {
    ///////////////////////////////////////////////////////////////
    void ObjectCreator::createObjects(ime::PhysicsWorld& physicsWorld, Grid &grid) {
        grid.forEachCell([&physicsWorld, &grid](const ime::Tile& tile) {
            ime::GameObject::Ptr actor;

            if (tile.getId() == 'P') {
                actor = std::make_unique<PacMan>(grid.getScene());
                actor->attachRigidBody(physicsWorld.createBody(ime::RigidBody::Type::Kinematic));
            } else if (tile.getId() == '|' || tile.getId() == '#') {
                actor = std::make_unique<Wall>(grid.getScene());
            } else if (tile.getId() == 'E')
                actor = std::make_unique<Pellet>(grid.getScene(), Pellet::Type::Energizer);
            else if (tile.getId() == 'D')
                actor = std::make_unique<Pellet>(grid.getScene(), Pellet::Type::Dot);
            else
                return;

            actor->getUserData().addProperty({"scene", std::ref(grid.getScene())});
            grid.addActor(std::move(actor), tile.getIndex());
        });
    }

} // namespace pm
