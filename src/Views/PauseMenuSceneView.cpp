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

#include "PauseMenuSceneView.h"
#include <IME/ui/widgets/Panel.h>
#include <IME/ui/widgets/Label.h>
#include <IME/ui/widgets/VerticalLayout.h>
#include <IME/ui/widgets/HorizontalLayout.h>
#include <IME/ui/widgets/Button.h>
#include <IME/ui/widgets/ToggleButton.h>
#include <IME/ui/widgets/Slider.h>

namespace pm {
    ///////////////////////////////////////////////////////////////
    void pm::PauseMenuSceneView::init(ime::ui::GuiContainer& gui) {
        using namespace ime::ui;

        // Container for all pause menu widgets
        auto* pnlContainer = gui.addWidget<Panel>(Panel::create(), "pnlPauseMenu");
        pnlContainer->getRenderer()->setBackgroundColour(ime::Colour::Transparent);

        // Container for pause menu buttons container
        auto* pnlInnerContainer = pnlContainer->addWidget<Panel>(Panel::create("40%", "40%"), "pnlSubContainer");
        pnlInnerContainer->getRenderer()->setBackgroundColour(ime::Colour("#0D0D0DE6"));
        pnlInnerContainer->getRenderer()->setBorders({1.5f, 1.5f, 1.5f, 1.5f});
        pnlInnerContainer->getRenderer()->setBorderColour(ime::Colour("#212121"));
        pnlInnerContainer->setOrigin(0.5f, 0.5f);
        pnlInnerContainer->setPosition("50%", "50%");

        // Pause menu heading
        auto lblHeading = Label::create("PAUSED");
        lblHeading->getRenderer()->setTextStyle(ime::TextStyle::Bold);
        lblHeading->setHorizontalAlignment(Label::HorizontalAlignment::Center);
        lblHeading->getRenderer()->setFont("ChaletLondonNineteenSixty.ttf");
        lblHeading->getRenderer()->setTextColour(ime::Colour::Red);
        lblHeading->setOrigin(0.5f, 0.0f);
        lblHeading->setPosition("50%", "4%");
        lblHeading->setTextSize(20.0f);
        pnlInnerContainer->addWidget(std::move(lblHeading), "lblHeading");

        // Create pause menu buttons
        auto btnList = {std::pair{"Resume", "btnResume"},
                        std::pair{"Restart Level", "btnRestart"},
                        std::pair{"Exit to Main Menu", "btnMainMenu"},
                        std::pair{"Exit Game", "btnExit"}};

        // Container for pause menu buttons
        auto vlBtnContainer = VerticalLayout::create("90%", "45%");
        vlBtnContainer->setOrigin(0.5f, 0.5f);
        vlBtnContainer->setPosition("50%", "50%");
        vlBtnContainer->getRenderer()->setSpaceBetweenWidgets(7.0f);

        for (const auto& btnData : btnList) {
            auto btn = Button::create(btnData.first);
            btn->getRenderer()->setFont("DejaVuSans.ttf");
            btn->setTextSize(14.0f);
            btn->getRenderer()->setFont("DejaVuSans.ttf");
            btn->getRenderer()->setBorderColour(ime::Colour::Transparent);
            btn->getRenderer()->setHoverTextStyle(ime::TextStyle::Bold);
            btn->getRenderer()->setBackgroundColour(ime::Colour("#1F1F1F59"));
            btn->getRenderer()->setBackgroundHoverColour(ime::Colour("#1F1F1F"));
            btn->getRenderer()->setBorders({0.6f, 1.0f, 0.6f, 1.0f});
            btn->getRenderer()->setBorderHoverColour(ime::Colour("#1F1F1F"));
            btn->getRenderer()->setTextColour(ime::Colour::White);
            btn->getRenderer()->setTextHoverColour(ime::Colour::Yellow);
            btn->getRenderer()->setFocusedBorderColour(ime::Colour::Transparent);
            vlBtnContainer->addWidget(std::move(btn), btnData.second);
        }

        // Create button to toggle the mute state of the audio player
        auto hlContainer = HorizontalLayout::create("90%", "18");
        hlContainer->setOrigin(0.5f, 1.0f);
        hlContainer->setPosition("50%", "95%");

        auto lblAudio = hlContainer->addWidget<Label>(Label::create("Audio"), "lblAudio");
        lblAudio->getRenderer()->setTextColour(ime::Colour::White);
        lblAudio->setVerticalAlignment(ime::ui::Label::VerticalAlignment::Top);
        lblAudio->getRenderer()->setFont("ChaletLondonNineteenSixty.ttf");

        auto* slMasterVolume = hlContainer->addWidget<Slider>(Slider::create(0, 100), "slMasterVolume");
        slMasterVolume->getRenderer()->setThumbColour(ime::Colour::Black);
        slMasterVolume->getRenderer()->setThumbHoverColour(ime::Colour::White);

        auto lblMasterVolume = hlContainer->addWidget<Label>(Label::create("0"), "lblMasterVolume");
        lblMasterVolume->getRenderer()->setTextColour(ime::Colour::White);
        lblMasterVolume->setVerticalAlignment(ime::ui::Label::VerticalAlignment::Top);
        lblMasterVolume->getRenderer()->setFont("ChaletLondonNineteenSixty.ttf");

        hlContainer->setRatio(std::size_t{0}, 0.4f);
        hlContainer->setRatio(std::size_t{0}, 0.6f);
        hlContainer->setRatio(std::size_t{2}, 0.3f);

        pnlInnerContainer->addWidget(std::move(hlContainer), "hlAudioOption");
        pnlInnerContainer->addWidget(std::move(vlBtnContainer), "vlPauseMenu");
    }

} // namespace pm
