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

#ifndef PACMAN_MAINMENUSCENEVIEW_H
#define PACMAN_MAINMENUSCENEVIEW_H

#include <IME/graphics/Sprite.h>
#include <IME/ui/GuiContainer.h>
#include "src/views/CommonView.h"

namespace pm {
    /**
     * @brief Main menu sub views
     */
    enum class SubView {
        MainMenu,    //!< View rendered when main menu state is rendered
        OptionsMenu, //!< View rendered when options view is clicked
        HighScores,  //!< View rendered when high scores button is clicked
    };

    /**
     * @brief Frontend for the main menu scene
     */
    class MainMenuSceneView {
    public:
        /**
         * @brief Construct view
         * @param gui Container for all UI widgets
         */
        explicit MainMenuSceneView(ime::ui::GuiContainer& gui);

        /**
         * @brief Initialize the view
         */
        void init();

    private:
        /**
         * @brief Switch to a different view within the main menu
         * @param view View to switch to
         */
        void setSubView(SubView view);

        /**
         * @brief Create the main view
         *
         * This view is rendered when the main menu scene is entered and
         * also when a subview returns (when back button is clicked)
         */
        void createMainMenuView();

        /**
         * @brief Create the options view
         *
         * This view contains the game options the player can modify. The
         * view is rendered when the options button from the main view is
         * clicked
         */
        void createOptionsMenuView();

        /**
         * @brief Create the high score view
         *
         * This view displays the game to high score. The view is displayed
         * when the high score button from the main view is clicked
         */
        void createHighScoresView();

        /**
         * @brief Create a button that returns to the main subview when clicked
         * @return Back button widget
         */
        ime::ui::Widget::Ptr createBackBtn();

    private:
        ime::ui::GuiContainer& gui_;   //!< Container for all main menu widgets
        SubView subView_;              //!< View that is currently rendered
    };
}

#endif
