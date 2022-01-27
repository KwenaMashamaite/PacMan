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

#include "MainMenuScene.h"
#include "GameplayScene.h"
#include "Common/Constants.h"
#include "Scoreboard/Scoreboard.h"
#include <IME/ui/widgets/VerticalLayout.h>
#include <IME/core/engine/Engine.h>
#include <IME/ui/widgets/Label.h>
#include <IME/ui/widgets/Button.h>
#include <IME/ui/widgets/TabsContainer.h>

namespace pm {
    ///////////////////////////////////////////////////////////////
    MainMenuScene::MainMenuScene() :
        view_{getGui()}
    {}

    ///////////////////////////////////////////////////////////////
    void MainMenuScene::onEnter() {
        initGui();
        initLeaderboard();
        initEventHandlers();
        getGui().setTabKeyUsageEnabled(false);
    }

    ///////////////////////////////////////////////////////////////
    void MainMenuScene::initGui() {
        view_.init();

        ime::ui::Panel* pnlControlsContainer = getGui().getWidget<ime::ui::TabsContainer>("tbsOptions")->getPanel("pnlControls");
        pnlControlsContainer->getWidget<ime::ui::Button>("btnMoveLeft")->setText(getSCache().getPref("MOVE_LEFT_BUTTON").getValue<std::string>());
        pnlControlsContainer->getWidget<ime::ui::Button>("btnMoveRight")->setText(getSCache().getPref("MOVE_RIGHT_BUTTON").getValue<std::string>());
        pnlControlsContainer->getWidget<ime::ui::Button>("btnMoveUp")->setText(getSCache().getPref("MOVE_UP_BUTTON").getValue<std::string>());
        pnlControlsContainer->getWidget<ime::ui::Button>("btnMoveDown")->setText(getSCache().getPref("MOVE_DOWN_BUTTON").getValue<std::string>());
    }

    ///////////////////////////////////////////////////////////////
    void MainMenuScene::initLeaderboard() {
        auto scoreboard = getCache().getValue<std::shared_ptr<Scoreboard>>("SCOREBOARD");
        auto namesContainer = getGui().getWidget<ime::ui::VerticalLayout>("vlNames");
        auto scoreContainer = getGui().getWidget<ime::ui::VerticalLayout>("vlScores");
        auto levelContainer = getGui().getWidget<ime::ui::VerticalLayout>("vlLevels");

        // Replace placeholder text with actual Scoreboard data
        scoreboard->forEachScore([&, count = 1] (const Score& score) mutable {
            if (count > Constants::MAX_NUM_HIGH_SCORES_TO_DISPLAY)
                return;

            namesContainer->getWidget<ime::ui::Label>("lblEntry" + std::to_string(count))->setText(score.owner_);
            scoreContainer->getWidget<ime::ui::Label>("lblEntry" + std::to_string(count))->setText(std::to_string(score.value_));
            levelContainer->getWidget<ime::ui::Label>("lblEntry" + std::to_string(count))->setText(std::to_string(score.level_));
            count++;
        });
    }

    ///////////////////////////////////////////////////////////////
    void MainMenuScene::initEventHandlers() {
        getGui().getWidget("btnPlay")->on("leftMouseDown", ime::Callback<>([this] {
            getEngine().popScene();
            getEngine().pushScene(std::make_unique<GameplayScene>());
        }));

        getGui().getWidget("btnQuit")->on("click", ime::Callback<>([this] {
            getEngine().quit();
        }));

        getInput().onKeyUp([this](ime::Keyboard::Key key) {
            if (view_.getActivePanel() == SubView::OptionsMenu && key != ime::Keyboard::Key::Unknown) {
                ime::ui::Panel* pnlControlsContainer = getGui().getWidget<ime::ui::TabsContainer>("tbsOptions")->getPanel("pnlControls");

                if (pnlControlsContainer->getWidget<ime::ui::Button>("btnMoveLeft")->isFocused()) {
                    pnlControlsContainer->getWidget<ime::ui::Button>("btnMoveLeft")->setText(ime::Keyboard::keyToString(key));
                    getSCache().getPref("MOVE_LEFT_BUTTON").setValue(ime::Keyboard::keyToString(key));
                }else if (pnlControlsContainer->getWidget<ime::ui::Button>("btnMoveRight")->isFocused()) {
                    pnlControlsContainer->getWidget<ime::ui::Button>("btnMoveRight")->setText(ime::Keyboard::keyToString(key));
                    getSCache().getPref("MOVE_RIGHT_BUTTON").setValue(ime::Keyboard::keyToString(key));
                } else if (pnlControlsContainer->getWidget<ime::ui::Button>("btnMoveUp")->isFocused()) {
                    pnlControlsContainer->getWidget<ime::ui::Button>("btnMoveUp")->setText(ime::Keyboard::keyToString(key));
                    getSCache().getPref("MOVE_UP_BUTTON").setValue(ime::Keyboard::keyToString(key));
                } else if (pnlControlsContainer->getWidget<ime::ui::Button>("btnMoveDown")->isFocused()) {
                    pnlControlsContainer->getWidget<ime::ui::Button>("btnMoveDown")->setText(ime::Keyboard::keyToString(key));
                    getSCache().getPref("MOVE_DOWN_BUTTON").setValue(ime::Keyboard::keyToString(key));
                }
            }
        });
    }

} // namespace pm
