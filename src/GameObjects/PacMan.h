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

#ifndef PACMAN_PACMAN_H
#define PACMAN_PACMAN_H

#include <IME/core/game_object/GameObject.h>

namespace pm {
    /**
     * @brief Player controlled actor
     */
    class PacMan : public ime::GameObject {
    public:
        /**
         * @brief The different states pacman can be in (one state at a time)
         */
        enum class State {
            Unknown = -1, //!< Unknown state
            Idle,         //!< Pacman ignores all requests to move him (can be eaten by a Ghost)
            Moving,       //!< Pacman is moving around the maze (can be eaten by a Ghost)
            Dying         //!< Pacman is dying and cannot be moved (cannot be eaten by a ghost)
        };

        /**
         * @brief Constructor
         * @param scene The scene the actor belongs to
         */
        explicit PacMan(ime::Scene& scene);

        /**
         * @brief Set the number of lives
         * @param numOfLives New number of lives
         *
         * By default, the player lives is pm::Constants::PLAYER_LiVES
         */
        void setLivesCount(int numOfLives);

        /**
         * @brief Get number of lives remaining
         * @return The number of lives remaining
         */
        int getLivesCount() const;

        /**
         * @brief Get the name of this class
         * @return The name of this class
         */
        std::string getClassName() const override;

        /**
         * @brief Change the state of Pacman
         * @param state The new state of pacman
         *
         * By default, the state is pm::PacMan::State::Idle
         */
        void setState(State state);

        /**
         * @brief Get current state
         * @return The current state
         */
        State getState() const;

        /**
         * @brief Set current direction
         * @param direction New direction
         *
         * By default, the direction is ime::Left
         */
        void setDirection(const ime::Vector2i& direction);

        /**
         * @brief Get the current direction
         * @return The current direction
         */
        ime::Vector2i getDirection() const;

    private:
        /**
         * @brief Change pacmans current animation
         * @param dir Pacmans current direction
         */
        void switchAnimation(ime::Vector2i dir);

        /**
         * @brief Initialize Animations
         */
        void initAnimations();

    private:
        int livesCount_;             //!< The GameObjects current number of lives
        ime::Vector2i direction_;    //!< Pacmans current direction
        State state_;                //!< Pacmans current state
    };
}

#endif
