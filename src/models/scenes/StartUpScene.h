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

#ifndef PACMAN_STARTUPSCENE_H
#define PACMAN_STARTUPSCENE_H

#include <IME/core/scene/Scene.h>

namespace pm {
    /**
     * @brief Defines the start up state of the game
     *
     * This scene is the first to be entered when the game is run. When
     * in this state, the game displays the copyright notice to the player
     * for a predefined amount of time after which the scene removes itself
     * from the game engine and the game transitions to the LoadingScene
     *
     * The user may force a scene transition by pressing the skip scene key
     */
    class StartUpScene : public ime::Scene {
    public:
        /**
         * @brief Enter the scene
         *
         * This function is called by the game engine when the scene
         * is entered for the first time
         */
        void onEnter() override;

        /**
         * @brief Transition the game to the next scene
         */
        void onExit() override;

    private:
        /**
         * @brief Start menu pop countdown
         */
        void startCountdown();

        /**
         * @brief Ask player for their name
         */
        void promptName();

        /**
         * @brief Make the scene skippable when enter key is pressed
         *
         * By default, the scene is no skippable
         */
        void enableSceneSkip();

        /**
         * @brief Save the player name and continue to gameplay
         */
        void save();
    };
}

#endif
