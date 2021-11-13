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

#ifndef PACMAN_GAMEPLAYSCENEVIEW_H
#define PACMAN_GAMEPLAYSCENEVIEW_H

#include <IME/ui/GuiContainer.h>
#include <IME/core/time/Timer.h>

namespace pm {
    /**
     * @brief Defines the view that appears across all Scenes
     *
     * This view includes the score and high score displayed at the top of
     * the screen, the flashing 1 UP text at the top left edge of the screen,
     * Pacman's current number of lives at the bottom left edge of the screen
     * and the level fruits at the bottom right of the screen
     *
     * This view is used by all game Scenes except the LoadingScene
     */
    class GameplaySceneView {
    public:
        /**
         * @brief Constructor
         * @param gui Container for all UI widgets in the view
         */
        explicit GameplaySceneView(ime::ui::GuiContainer& gui);

        /**
         * @brief Initialize the view
         * @param level The current game level
         * @param lives The current number of pacman lives
         */
        void init(unsigned int level, unsigned int lives);

        /**
         * @brief Set the score value to be displayed
         * @param score The score to display
         *
         * @warning This function must be called after the view is
         * initialized, otherwise undefined behavior
         *
         * @see init
         */
        void setScore(int score);

        /**
         * @brief Set the high score value to be displayed
         * @param highScore The high score to display
         *
         * @warning This function must be called after the view is
         * initialized, otherwise undefined behavior
         *
         * @see init
         */
        void setHighScore(int highScore);

        /**
         * @brief Update the view
         * @param deltaTime Time passed since view was last updated
         */
        void update(ime::Time deltaTime);

        /**
         * @brief Update the number of lives shown for pacman
         * @param pacmanLives The current number of pacman lives
         */
        void updateLives(unsigned int pacmanLives);

    private:
        /**
         * @brief Create view widgets
         */
        void createWidgets();

        /**
         * @brief Create current level fruit depiction
         * @param level The current game level
         */
        void createLevelIndicatorSprites(unsigned int level);

        /**
         * @brief Create current player lives depiction
         * @param The current number of pacman lives
         */
        void createPlayerLivesIndicatorSprites(unsigned int lives);

    private:
        ime::ui::GuiContainer& gui_;  //!< Container for all widgets
        ime::Timer::Ptr timer_;            //!< One up text flash Timer
    };
}

#endif
