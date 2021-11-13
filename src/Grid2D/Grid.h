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

#ifndef PACMAN_GRID_H
#define PACMAN_GRID_H

#include <IME/core/tilemap/TileMap.h>

namespace pm {
    /**
     * @brief Contains all game objects in the game
     */
    class Grid {
    public:
        /**
         * @brief Constructor
         * @param tileMap Tilemap to base grid on
         * @param scene The scene in which the object is in
         * @param objects The Scenes game object container
         */
        Grid(ime::TileMap& tileMap, ime::Scene& scene, ime::GameObjectContainer& objects);

        /**
         * @brief Create the grid
         * @param filename The name of the file that contains the grid data
         */
        void loadFromFile(const std::string& filename);

        /**
         * @brief Show or hide the grid
         * @param visible True to show or false to hide
         */
        void setVisible(bool visible);

        /**
         * @brief Set the position of the grid
         * @param pos The new position of the grid
         *
         * By default the position is (0, 0)
         */
        void setPosition(ime::Vector2f pos);

        /**
         * @brief Set the position of the grids background image
         * @param pos The new position of the background
         *
         * The default position is (0, 0)
         */
        void setBackgroundImagePosition(ime::Vector2f pos);

        /**
         * @brief Set the grid background
         * @param level The current game level
         *
         * Level 0 = intro grid and level 1 to ... n = gameplay grid
         */
        void setBackground(int level);

        /**
         * @brief Add an actor to the grid
         * @param actor The actor to be added to the grid
         * @param index The index of the cell to add the actor to
         */
        void addActor(ime::GameObject::Ptr actor, ime::Index index);

        /**
         * @brief Get an actor from the grid by
         * @param id The id of the actor to retrieve
         * @return The actor with the given id if it exists, otherwise a nullptr
         */
        ime::GameObject* getActorById(int id) const;

        /**
         * @brief Remove an actor from the grid
         * @param id The id of the actor to be removed
         */
        void removeActor(int id);

        /**
         * @brief Apply a callback to each cell of the grid
         * @param callback The function to be applied
         */
        void forEachCell(const ime::Callback<const ime::Tile&>& callback);

        /**
         * @brief Apply a callback to each actor in the grid
         * @param callback The function to be applied
         */
        void forEachActor(const ime::Callback<ime::GameObject*>& callback);

        /**
         * @brief Play a flashing grid animation
         * @return The duration of the animation
         *
         * This function will toggle the grid colour between its current
         * colour and a white colour for predetermined number of seconds
         */
        ime::Time playFlashAnimation();

        /**
         * @brief Execute a callback when the grid animation finishes
         * @param callback The function to be executed
         *
         * @note Only one callback may be assigned to this event. A new
         * callback removes the previous callback.
         *
         * By default, there is no callback assigned to this event.
         * To remove the callback pass a nullptr
         */
        void onAnimationFinish(ime::Callback<> callback);

        /**
         * @brief Check if the grid flash animation is playing or not
         * @return True if animation is currently playing, otherwise false
         */
        bool isAnimationPlaying();

        /**
         * @brief Update the grid
         * @param deltaTime Time passed since ;ast scene update
         */
        void update(ime::Time deltaTime);

        /**
         * @brief Get the scene the grid is in
         * @return The scene the grid is in
         */
        ime::Scene& getScene();

    private:
        ime::TileMap& grid_;
        ime::Scene& scene_;
        ime::GameObjectContainer& actors_;
        ime::Sprite background_;
        ime::Callback<> onAnimFinish_;
    };
}


#endif //PACMAN_GRID_H
