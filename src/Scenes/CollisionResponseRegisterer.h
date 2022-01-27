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

#ifndef PACMAN_COLLISIONRESPONSEREGISTRE_H
#define PACMAN_COLLISIONRESPONSEREGISTRE_H

#include "Scenes/GameplayScene.h"
#include <IME/core/object/GridObject.h>

namespace pm {
    /**
     * @brief Defines and registers collision handlers on game objects
     */
    class CollisionResponseRegisterer {
    public:
        explicit CollisionResponseRegisterer(GameplayScene& game);
        /**
         * @brief Register a fruit collision handler
         * @param gameObject The game object to register the collision handler on
         */
        void registerCollisionWithFruit(ime::GridObject* gameObject);

        /**
         * @brief Register a pellet collision handler
         * @param gameObject The game object to register the collision handler on
         */
        void registerCollisionWithPellets(ime::GridObject* gameObject);

        /**
         * @brief Register a ghost collision handler
         * @param gameObject The game object to register the collision handler on
         */
        void registerCollisionWithGhost(ime::GridObject* gameObject);

        /**
         * @brief Register a teleportation sensor handler
         * @param gameObject The game object to register the collision handler on
         */
        void registerCollisionWithTeleportationSensor(ime::GridObject* gameObject);

        /**
         * @brief Register a speed slow down sensor handler
         * @param gameObject The game object to register the collision handler on
         */
        void registerCollisionWithSlowDownSensor(ime::GridObject* gameObject);

    private:
        /**
         * @brief Resolve a collision between a fruit object and another game object
         * @param fruit The fruit in collision with another game object
         * @param otherGameObject The game object in collision with the fruit
         */
        void resolveFruitCollision(ime::GridObject* fruit, ime::GridObject* otherGameObject);

        /**
         * @brief Resolve a collision between a pellet object and another game object
         * @param pellet The pellet in collision with another game object
         */
        void resolvePelletCollision(ime::GridObject* pellet);

        /**
         * @brief Resolve the collision between a ghost and another game object
         * @param ghost The ghost in collision with another game object
         * * @param otherGameObject The game object in collision with the ghost
         */
        void resolveGhostCollision(ime::GridObject* ghost, ime::GridObject* otherGameObject);

        /**
         * @brief Resolve the collision between a teleportation sensor and its trigger
         * @param sensor The triggered teleportation sensor
         * @param objectOnSensor The game object that triggered the sensor
         */
        void resolveTeleportationSensorCollision(ime::GridObject* sensor, ime::GridObject* objectOnSensor);

    private:
        /**
         * @brief Replace a fruit texture with a corresponding score texture
         * @param fruit The fruit to be replaced with a score texture
         * @param otherGameObject The game object in collision with the fruit
         *
         * The fruit will remain on the screen for some time before it disappears
         */
        void replaceFruitWithScore(ime::GridObject* fruit);

        /**
         * @brief Convert an eaten ghost and the game object that ate it into a single score texture
         * @param ghost The ghost in collision with another game object
         * @param otherGameObject The game object in collision with the ghost
         *
         * This function is called when pacman collides with a blue ghost,
         * after the collision, the ghost and pacman textures are momentarily
         * combined into a single score texture which corresponds to the number
         * of points the player earned for eating the ghost
         */
        void replaceGhostWithScore(ime::GridObject* ghost, ime::GridObject* otherGameObject);

        /**
         * @brief Spawn a fruit
         *
         * The type of fruit spawned depends on the current level of the game.
         * If left uneaten for some time, the fruit will disappear
         */
        void spawnFruit();

    private:
        GameplayScene& game_;
    };
}

#endif
