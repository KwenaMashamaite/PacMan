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

#ifndef PACMAN_MAINMENUSCENE_H
#define PACMAN_MAINMENUSCENE_H

#include <IME/core/scene/Scene.h>
#include "src/views/MainMenuSceneView.h"

namespace pm {
    /**
     * @brief Defines the main menu state of the game
     *
     * The scene the game transitions to from this state depends on what
     * the user selects. This state provides the player with the option
     * to play or exit the game. In addition, the player can view the game
     * controls and/or change the game settings.
     */
    class MainMenuScene : public ime::Scene {
    public:
        /**
         * @brief Constructor
         */
        MainMenuScene();

        /**
         * @brief Enter the scene
         *
         * This function is called by the game engine when the scene
         * is entered for the first time
         */
        void onEnter() override;

    private:
        /**
         * @brief Replace scoreboard placeholder text with actual player scores
         */
        void initLeaderboard();

        /**
         * @brief Add "click" event handlers to main menu buttons
         */
        void initEventHandlers();

    private:
        MainMenuSceneView view_;
    };
}

#endif
