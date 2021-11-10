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

#ifndef PACMAN_OBJECTREFERENCEKEEPER_H
#define PACMAN_OBJECTREFERENCEKEEPER_H

#include <IME/core/game_object/GameObject.h>
#include <IME/common/Vector2.h>
#include <string>
#include <unordered_map>
#include <utility>

namespace pm {
    /**
     * @brief Registers and provides global access to game objects
     */
    class ObjectReferenceKeeper {
    public:
        /**
         * @brief Register a game object for global access
         * @param gameObject The game object to be registered
         *
         * @warning @a gameObject must not be @a nullptr
         */
        static void registerActor(ime::GameObject* gameObject);

        /**
         * @brief Get access to an actor
         * @param tag The tag of the game ob
         * @return A pointer to the actor if it exists otherwise a nullptr
         */
        static ime::GameObject* getActor(const std::string& tag);

    private:
        inline static std::unordered_map<std::string, ime::GameObject*> gameObjects_{};
    };
}

#endif //PACMAN_OBJECTREFERENCEKEEPER_H
