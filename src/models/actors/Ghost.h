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

#ifndef PACMAN_GHOST_H
#define PACMAN_GHOST_H

#include <IME/core/game_object/GameObject.h>

namespace pm {
    /**
     * @brief Ghost actor
     */
    class Ghost : public ime::GameObject {
    public:
        /**
         * @brief The colour of the ghost
         */
        enum class Colour {
            Red,     //!< Blinky
            Pink,    //!< Pinky
            Cyan,    //!< Inky
            Orange,  //!< Clyde
        };

        /**
         * @brief States a ghost can be in at any given time
         */
        enum class State {
            Unknown = -1, //!< Unknown Ghost state
            Idle,         //!< The ghost remains stationary in at is current position (cannot be eaten by PacMan)
            Scatter,      //!< The ghost cycles a grid corner (cannot be eaten by Pacman)
            Chase,        //!< The ghost chases pacman (cannot be eaten by PacMan)
            Frightened,   //!< The ghost runs away from pacman (can be eaten by PacMan)
            Eaten,        //!< The ghost retreats back to the ghost house (cannot be eaten by PacMan nor can it kill Pacman)
            Wonder,       //!< The ghost moves randomly in the grid (cannot be eaten by PacMan)
        };

        /**
         * @brief Constructor
         * @param scene The scene that the actor is in
         * @param colour The colour of the ghost
         */
        Ghost(ime::Scene& scene, Colour colour);

        /**
         * @brief Get the name of this class
         * @return The name of this class
         */
        std::string getClassName() const override;

        /**
         * @brief Get the current state of the ghost
         * @return The current state of the ghost
         *
         * @warning Calling this function before the ghosts FSM is initialized
         * is undefined behavior
         *
         * @see initFSM
         */
        State getState() const;

        /**
         * @brief Set the direction of the ghost
         * @param dir The new direction of the ghost
         */
        void setDirection(ime::Vector2i dir);

        /**
         * @brief Get the current direction
         * @return The current direction
         */
        ime::Vector2i getDirection() const;

        /**
         * @brief Destructor
         */
        ~Ghost() override;

    private:
        ime::Vector2i direction_;    //!< The direction of the ghost
    };
}

#endif
