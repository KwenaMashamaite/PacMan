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

#ifndef PACMAN_STARTUPSCENEVIEW_H
#define PACMAN_STARTUPSCENEVIEW_H

#include <IME/ui/GuiContainer.h>

namespace pm {
    /**
     * @brief Frontend for the start up state
     *
     * This view is the first to be displayed when the user runs the game
     * executable. If the game is opened for the first time, the user is
     * asked for a name to associate with their game score
     */
    class StartUpSceneView {
    public:
        /**
         * @brief Initialize the view
         * @param gui Container for all UI controls in the view
         */
        void init(ime::ui::GuiContainer& gui);

    private:
        /**
         * @brief Create name prompt sub view
         * @param gui The container for sub menu widgets
         */
        void createNamePrompt(ime::ui::GuiContainer& gui);
    };
}

#endif
