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

#include "src/views/GameOverSceneView.h"
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
        createPreSaveMenu(gui);
        createNavButtons(gui);
    }

    ///////////////////////////////////////////////////////////////
    void GameOverSceneView::createPreSaveMenu(GuiContainer &gui) {
        auto pnlContainer = gui.addWidget<Panel>(Panel::create(), "pnlContainer");
        pnlContainer->getRenderer()->setBackgroundColour(ime::Colour::Transparent);

        // Container for widgets placed at the centre of the view
        auto pnlCentreContainer = pnlContainer->addWidget<Panel>(Panel::create("93%", "86%"), "pnlCentreContainer");
        pnlCentreContainer->getRenderer()->setBackgroundColour(ime::Colour("#27293d33"));
        pnlCentreContainer->getRenderer()->setBorders({0.6f, 1.0f, 0.6f, 1.0f});
        pnlCentreContainer->getRenderer()->setBorderColour(ime::Colour(128, 128, 128, 60));
        pnlCentreContainer->setOrigin(0.5f, 0.5f);
        pnlCentreContainer->setPosition("50%", "50%");

        // Top heading
        auto* lblHeading = pnlCentreContainer->addWidget<Label>(Label::create(), "lblHeading1");
        lblHeading->setText("GAME OVER");
        lblHeading->getRenderer()->setFont("pacfont.ttf");
        lblHeading->getRenderer()->setTextStyle(ime::TextStyle::Bold);
        lblHeading->getRenderer()->setTextColour(ime::Colour::Red);
        lblHeading->setTextSize(45.0f);
        lblHeading->setOrigin(0.5f, 0.0f);
        lblHeading->setPosition("50%", "1%");

        // Container for widgets user interacts with
        auto vlInnerCentreContainer = pnlCentreContainer->addWidget<VerticalLayout>(VerticalLayout::create("80%", "30%"), "pnlInnerCentreContainer");
        vlInnerCentreContainer->setOrigin(0.5f, 0.0f);
        vlInnerCentreContainer->setPosition("50%", ime::bindBottom(lblHeading).append("+15%"));

        // Heading
        auto lblHeading2 = Label::create("You died!!");
        lblHeading2->getRenderer()->setTextColour(ime::Colour("#d0d005"));
        lblHeading2->getRenderer()->setBackgroundColour(ime::Colour::Transparent);
        lblHeading2->setHorizontalAlignment(Label::HorizontalAlignment::Center);
        lblHeading2->getRenderer()->setTextStyle(ime::TextStyle::Italic);
        vlInnerCentreContainer->addWidget(std::move(lblHeading2), "lblHeading2");
        vlInnerCentreContainer->setRatio(std::size_t{0}, 0.20f);

        // Create info labels
        auto lblHighScore = Label::create("HIGH SCORE ");
        lblHighScore->setHorizontalAlignment(ime::ui::Label::HorizontalAlignment::Left);
        lblHighScore->setVerticalAlignment(ime::ui::Label::VerticalAlignment::Center);
        lblHighScore->getRenderer()->setTextStyle(ime::TextStyle::Bold);
        lblHighScore->getRenderer()->setTextColour(ime::Colour::White);
        lblHighScore->getRenderer()->setBorders({0.0, 1.0f, 0.0f, 1.0f});
        lblHighScore->getRenderer()->setBorderColour(ime::Colour::White);
        lblHighScore->getRenderer()->setBackgroundColour(ime::Colour::Transparent);

        auto lblName = lblHighScore->copy();
        lblName->setText("PLAYER");
        lblName->getRenderer()->setBorders({0.0, 1.3f, 0.0f, 1.3f});
        lblName->getRenderer()->setBorderColour(ime::Colour("#ffffff14"));
        lblName->getRenderer()->setBackgroundColour(ime::Colour("#11141B66"));

        auto lblLevel = lblName->copy();
        lblLevel->setText("LEVEL");

        auto lblScore = lblName->copy();
        lblScore->setText("SCORE ");

        // Create info labels
        auto lblHighScoreVal = lblHighScore->copy();
        lblHighScoreVal->setHorizontalAlignment(ime::ui::Label::HorizontalAlignment::Right);
        lblHighScoreVal->setText("00");

        auto lblNameVal = lblName->copy();
        lblNameVal->setHorizontalAlignment(ime::ui::Label::HorizontalAlignment::Right);
        lblNameVal->setText("Player 1");

        auto lblLevelVal = lblNameVal->copy();
        lblLevelVal->setText("0");

        auto lblScoreVal = lblNameVal->copy();
        lblScoreVal->setText("00");

        // Add labels
        const auto SPACE_BETWEEN_WIDGETS = 0.15f;
        auto vlText = VerticalLayout::create();
        vlText->addWidget(std::move(lblHighScore), "lblHighScore");
        vlText->addSpace(5 * SPACE_BETWEEN_WIDGETS);

        vlText->addWidget(std::move(lblName), "lblPlayerName");
        vlText->addSpace(SPACE_BETWEEN_WIDGETS);

        vlText->addWidget(std::move(lblLevel), "lblLevel");
        vlText->addSpace(SPACE_BETWEEN_WIDGETS);

        vlText->addWidget(std::move(lblScore), "lblScore");

        // Add label values
        auto vlTextBoxes = VerticalLayout::create();
        vlTextBoxes->addWidget(std::move(lblHighScoreVal), "lblHighScoreVal");
        vlTextBoxes->addSpace( 5 * SPACE_BETWEEN_WIDGETS);

        vlTextBoxes->addWidget(std::move(lblNameVal), "lblPlayerNameVal");
        vlTextBoxes->addSpace(SPACE_BETWEEN_WIDGETS);

        vlTextBoxes->addWidget(std::move(lblLevelVal), "lblLevelVal");
        vlTextBoxes->addSpace(SPACE_BETWEEN_WIDGETS);

        vlTextBoxes->addWidget(std::move(lblScoreVal), "lblScoreVal");

        auto hlInfo = HorizontalLayout::create();
        hlInfo->addWidget(std::move(vlText), "vlText");
        hlInfo->addWidget(std::move(vlTextBoxes), "vlTextBoxes");
        vlInnerCentreContainer->addWidget(std::move(hlInfo), "hlInfo");
    }

    void GameOverSceneView::createNavButtons(ime::ui::GuiContainer& gui) {
        auto vlContainer = VerticalLayout::create("100%", "100%");
        vlContainer->setOrigin(0.5f, 0.5f);
        vlContainer->setPosition("50%", "50%");
        vlContainer->getRenderer()->setSpaceBetweenWidgets(7.0f);

        // Create action buttons
        auto btnRetryLevel = Button::create("Retry Level");
        btnRetryLevel->getRenderer()->setFont("DejaVuSans.ttf");
        btnRetryLevel->getRenderer()->setRoundedBorderRadius(18.0f);
        btnRetryLevel->getRenderer()->setRoundedBorderRadius(18);
        btnRetryLevel->getRenderer()->setHoverTextStyle(ime::TextStyle::Italic);
        btnRetryLevel->getRenderer()->setBackgroundColour(ime::Colour("#4d4dff"));
        btnRetryLevel->getRenderer()->setBackgroundHoverColour(ime::Colour("#32CD32"));
        btnRetryLevel->getRenderer()->setTextColour(ime::Colour::White);
        btnRetryLevel->getRenderer()->setTextHoverColour(ime::Colour::Black);
        btnRetryLevel->getRenderer()->setFocusedBorderColour(ime::Colour::Transparent);

        auto btnExitMainMenu = btnRetryLevel->copy();
        btnExitMainMenu->setText("Main Menu");

        auto btnExitGame = btnRetryLevel->copy();
        btnExitGame->setText("Exit Game");

        vlContainer->addWidget(std::move(btnRetryLevel), "btnRetryLevel");
        vlContainer->addWidget(std::move(btnExitMainMenu), "btnExitMainMenu");
        vlContainer->addWidget(std::move(btnExitGame), "btnExitGame");

        // Container for action buttons
        auto pnlButtonsContainer = Panel::create("80%", "18%");
        pnlButtonsContainer->getRenderer()->setBackgroundColour(ime::Colour::Transparent);
        pnlButtonsContainer->setOrigin(0.5f, 0.0f);

        pnlButtonsContainer->addWidget(std::move(vlContainer), "vlButtonsContainer");
        pnlButtonsContainer->setPosition("50%", ime::bindBottom(gui.getWidget("pnlInnerCentreContainer")).append("+15%"));
        gui.getWidget<Panel>("pnlCentreContainer")->addWidget(std::move(pnlButtonsContainer), "pnlButtonsContainer");
    }

} // namespace pm
