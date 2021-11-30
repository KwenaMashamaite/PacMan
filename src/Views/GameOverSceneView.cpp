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

#include "GameOverSceneView.h"
#include <IME/ui/widgets/Label.h>
#include <IME/ui/widgets/Panel.h>
#include <IME/ui/widgets/VerticalLayout.h>
#include <IME/ui/widgets/HorizontalLayout.h>
#include <IME/ui/widgets/Button.h>

using namespace ime::ui;

namespace pm {
    ///////////////////////////////////////////////////////////////
    void GameOverSceneView::init(ime::ui::GuiContainer &gui) {
        gui.setFont("ChaletLondonNineteenSixty.ttf");

        auto pnlContainer = gui.addWidget<Panel>(Panel::create("93%", "50%"), "pnlContainer");
        pnlContainer->getRenderer()->setBackgroundColour(ime::Colour("#0D0D0DCC"));
        pnlContainer->getRenderer()->setBorders({0.6f, 1.0f, 0.6f, 1.0f});
        pnlContainer->getRenderer()->setBorderColour(ime::Colour(128, 128, 128, 60));
        pnlContainer->setOrigin(0.5f, 0.5f);
        pnlContainer->setPosition("50%", "50%");

        // Heading
        auto* lblHeading = pnlContainer->addWidget<Label>(Label::create(), "lblGameOver");
        lblHeading->setText("GAME OVER");
        lblHeading->getRenderer()->setFont("pacfont.ttf");
        lblHeading->getRenderer()->setTextStyle(ime::TextStyle::Bold);
        lblHeading->getRenderer()->setTextColour(ime::Colour::Red);
        lblHeading->setTextSize(45.0f);
        lblHeading->setOrigin(0.5f, 0.0f);
        lblHeading->setPosition("50%", "5%");

        // Container for action buttons
        auto vlContainer = VerticalLayout::create("63%", "50%");
        vlContainer->setName("vlButtonsContainer");
        vlContainer->setOrigin(0.5f, 0.0f);
        vlContainer->getRenderer()->setSpaceBetweenWidgets(7.0f);

        // Create action buttons
        auto btnRetryLevel = Button::create("Retry Level");
        btnRetryLevel->getRenderer()->setFont("DejaVuSans.ttf");
        btnRetryLevel->getRenderer()->setBorderColour(ime::Colour("#00000066"));
        btnRetryLevel->getRenderer()->setHoverTextStyle(ime::TextStyle::Bold);
        btnRetryLevel->getRenderer()->setBackgroundColour(ime::Colour::Transparent);
        btnRetryLevel->getRenderer()->setBackgroundHoverColour(ime::Colour::Transparent);
        btnRetryLevel->getRenderer()->setBorders({0.0f, 0.0f, 0.0f, 1.0f});
        btnRetryLevel->getRenderer()->setBorderHoverColour(ime::Colour("#1D1D1D"));
        btnRetryLevel->getRenderer()->setTextColour(ime::Colour::White);
        btnRetryLevel->getRenderer()->setTextHoverColour(ime::Colour::Yellow);
        btnRetryLevel->getRenderer()->setFocusedBorderColour(ime::Colour::Transparent);

        Button::Ptr btnStartOver = btnRetryLevel->copy();
        btnStartOver->setText("Start Over");

        Button::Ptr btnExitMainMenu = btnRetryLevel->copy();
        btnExitMainMenu->setText("Main Menu");

        Button::Ptr btnExitGame = btnRetryLevel->copy();
        btnExitGame->setText("Exit Game");

        vlContainer->addWidget(std::move(btnRetryLevel), "btnRetryLevel");
        vlContainer->addWidget(std::move(btnStartOver), "btnStartOver");
        vlContainer->addWidget(std::move(btnExitMainMenu), "btnExitMainMenu");
        vlContainer->addWidget(std::move(btnExitGame), "btnExitGame");
        vlContainer->setPosition("50%", ime::bindBottom(gui.getWidget("lblGameOver")).append("+35"));
        pnlContainer->addWidget(std::move(vlContainer));
    }

} // namespace pm
