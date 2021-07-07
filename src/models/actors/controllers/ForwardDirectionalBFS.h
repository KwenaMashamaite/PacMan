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

#ifndef PACMAN_FORWARDDIRECTIONALBFS_H
#define PACMAN_FORWARDDIRECTIONALBFS_H

#include <IME/core/physics/grid/path/IPathFinderStrategy.h>
#include <IME/core/game_object/GameObject.h>
#include <IME/core/physics/grid/path/BFS.h>
#include <string>
#include <memory>

namespace pm {
    /**
     * @brief Finds a path in a ime::TileMap using the Breadth First Search algorithm
     *
     * The path is generated in the forward direction. This means that that
     * the actor cannot go backwards. As a result, the generated path will
     * not always be the shortest
     */
    class ForwardDirectionalBFS : public ime::IPathFinderStrategy {
    public:
        /**
         * @brief Initialize the algorithm
         * @param gridSize The size of the grid in tiles
         * @param actor The actor whose path is to be generated by the algorithm
         *
         * @note The @a actor must have its position tracked by the PositionTracker
         * prior to findPath() call
         *
         * @warning @a actor must not be a nullptr
         *
         * @see findPath
         */
        ForwardDirectionalBFS(const ime::Vector2u& gridSize, ime::GameObject* actor);

        /**
         * @brief Generate a path from a source tile to a target tile in a grid
         * @param grid Grid to generate path in
         * @param sourceTile The starting position in tiles
         * @param targetTile The destination in tiles
         * @return The path from the source tile to the destination tile if
         *         it exists, otherwise an empty path
         */
        std::stack<ime::Index> findPath(ime::TileMap& grid, ime::Index sourceTile,
            ime::Index targetTile) override;

        /**
         * @brief Get the type of path finding algorithm
         * @return The type of the path finding algorithm
         */
        std::string getType() const override;

    private:
        ime::GameObject* actor_;                 //!< The actor whose path is to be defined
        std::unique_ptr<ime::GameObject> wall_;  //!< Blocks actors reverse path
        ime::BFS bfs_;                           //!< Grid path finder
    };
}

#endif //PACMAN_FORWARDDIRECTIONALBFS_H