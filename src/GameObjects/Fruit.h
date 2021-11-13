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

#ifndef PACMAN_FRUIT_H
#define PACMAN_FRUIT_H

#include <IME/core/game_object/GameObject.h>
#include <IME/graphics/SpriteSheet.h>

namespace pm {
    /**
     * @brief A fruit that can be eaten by Pacman
     */
    class Fruit : public ime::GameObject {
    public:
        /**
         * @brief Fruit type
         */
        enum class Type {
            Cherry,      //!< Cherry
            Strawberry,  //!< Strawberry
            Peach,       //!< Peach
            Apple,       //!< Apple
            Melon,       //!< Melon
            Galaxian,    //!< Galaxian
            Bell,        //!< Bell
            Key          //!< Key
        };

        /**
         * @brief Constructor
         * @param scene The scene the object belongs to
         * @param type The type of fruit
         */
        Fruit(ime::Scene& scene, Type type);

        /**
         * @brief Get the name of the class
         * @return The name of the class
         */
        std::string getClassName() const override;
    };
}

#endif