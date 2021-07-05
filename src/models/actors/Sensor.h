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

#ifndef PACMAN_SENSOR_H
#define PACMAN_SENSOR_H

#include <IME/core/game_object/GameObject.h>

namespace pm {
    /**
     * @brief Sensor actor
     *
     * This class is used as a trigger for certain actions when an actor
     * enters it. For example, it is used to detect when PacMan or a Ghost
     * reaches the end of the tunnel
     */
    class Sensor : public ime::GameObject {
    public:
        /**
         * @brief Constructor
         * @param scene The scene the object belongs to
         */
        explicit Sensor(ime::Scene& scene);

        /**
         * @brief Get the class type
         * @return Name of the concrete class the pellet belongs to
         */
        std::string getClassName() const override;
    };
}

#endif //PACMAN_SENSOR_H
