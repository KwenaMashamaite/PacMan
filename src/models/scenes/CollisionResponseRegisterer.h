#ifndef PACMAN_COLLISIONRESPONSEREGISTRE_H
#define PACMAN_COLLISIONRESPONSEREGISTRE_H

#include <IME/core/game_object/GameObject.h>
#include <src/models/scenes/GameplayScene.h>

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
        void registerCollisionWithFruit(ime::GameObject* gameObject);

        /**
         * @brief Register a pellet collision handler
         * @param gameObject The game object to register the collision handler on
         */
        void registerCollisionWithPellets(ime::GameObject* gameObject);

        /**
         * @brief Register a ghost collision handler
         * @param gameObject The game object to register the collision handler on
         */
        void registerCollisionWithGhost(ime::GameObject* gameObject);

        /**
         * @brief Register a teleportation sensor handler
         * @param gameObject The game object to register the collision handler on
         */
        void registerCollisionWithTeleportationSensor(ime::GameObject* gameObject);

        /**
         * @brief Register a speed slow down sensor handler
         * @param gameObject The game object to register the collision handler on
         */
        void registerCollisionWithSlowDownSensor(ime::GameObject* gameObject);

    private:
        /**
         * @brief Resolve a collision between a fruit object and another game object
         * @param fruit The fruit in collision with another game object
         * @param otherGameObject The game object in collision with the fruit
         */
        void resolveFruitCollision(ime::GameObject* fruit, ime::GameObject* otherGameObject);

        /**
         * @brief Resolve a collision between a pellet object and another game object
         * @param pellet The pellet in collision with another game object
         */
        void resolvePelletCollision(ime::GameObject* pellet);

        /**
         * @brief Resolve the collision between a ghost and another game object
         * @param ghost The ghost in collision with another game object
         * * @param otherGameObject The game object in collision with the ghost
         */
        void resolveGhostCollision(ime::GameObject* ghost, ime::GameObject* otherGameObject);

        /**
         * @brief Resolve the collision between a teleportation sensor and its trigger
         * @param sensor The triggered teleportation sensor
         * @param objectOnSensor The game object that triggered the sensor
         */
        void resolveTeleportationSensorCollision(ime::GameObject* sensor, ime::GameObject* objectOnSensor);

    private:
        /**
         * @brief Replace a fruit texture with a corresponding score texture
         * @param fruit The fruit to be replaced with a score texture
         * @param otherGameObject The game object in collision with the fruit
         *
         * The fruit will remain on the screen for some time before it disappears
         */
        void replaceFruitWithScore(ime::GameObject* fruit);

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
        void replaceGhostWithScore(ime::GameObject* ghost, ime::GameObject* otherGameObject);

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
