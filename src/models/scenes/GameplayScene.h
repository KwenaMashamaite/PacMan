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

#ifndef PACMAN_GAMEPLAYSCENE_H
#define PACMAN_GAMEPLAYSCENE_H

#include <IME/core/scene/Scene.h>
#include "src/models/world/Grid.h"
#include "src/views/GameplaySceneView.h"

namespace pm {
    /**
     * @brief Defines the playing state of the game
     */
    class GameplayScene : public ime::Scene {
    public:
        /**
         * @brief Constructor
         */
        GameplayScene();

        /**
         * @brief Enter the scene
         *
         * This function is called by the game engine when the scene
         * is entered for the first time
         */
        void onEnter() override;

        /**
         * @brief Pause scene
         *
         * This function is called by the game engine when the game transitions
         * from this scene (without destroying it) to another scene
         */
        void onPause() override;

        /**
         * @brief Resume scene
         *
         * This function is called by the game engine when the game returns
         * to this scene from another one
         */
        void onResume() override;

        /**
         * @brief Update the scene
         * @param deltaTime Time passed since last update
         *
         * This function is called by the game engine when it is time to
         * update. Note that @a deltaTime is frame rate dependent
         */
        void update(ime::Time deltaTime) override;

    private:
        /**
         * @brief Initialize the gui
         */
        void initGui();

        /**
         * @brief Create the gameplay grid
         */
        void createGrid();

        /**
         * @brief Add actors to the grid
         */
        void createActors();

        /**
         * @brief Initialize game events
         */
        void intiGameEvents();

        /**
         * @brief Initialize third party engine events
         */
        void initEngineEvents();

        /**
         * @brief Transition game to pause menu
         */
        void pauseGame();

    private:
        GameplaySceneView view_;     //!< Scene view without the gameplay grid
        std::unique_ptr<Grid> grid_; //!< Gameplay grid view
    };
}

#endif
