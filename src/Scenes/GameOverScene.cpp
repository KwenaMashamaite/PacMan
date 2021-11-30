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

#include "GameOverScene.h"
#include "GameplayScene.h"
#include "MainMenuScene.h"
#include "Scoreboard/Scoreboard.h"
#include "Common/Constants.h"
#include <IME/core/engine/Engine.h>
#include <IME/ui/widgets/Label.h>

using namespace ime::ui;

namespace pm {
    ///////////////////////////////////////////////////////////////
    void GameOverScene::onEnter() {
        updateLeaderboard();
        initGui();
        initButtonEvents();
    }

    ///////////////////////////////////////////////////////////////
    void GameOverScene::updateLeaderboard() {
        Score score;
        score.value_ = cache().getValue<int>("CURRENT_SCORE");
        score.level_ = cache().getValue<int>("CURRENT_LEVEL");

        auto name = sCache().getPref("PLAYER_NAME").getValue<std::string>();
        name.copy(score.owner_, name.length() + 1);

        auto scoreboard = cache().getValue<std::shared_ptr<Scoreboard>>("SCOREBOARD");
        scoreboard->addScore(score);
        scoreboard->updateHighScoreFile();
    }

    ///////////////////////////////////////////////////////////////
    void GameOverScene::initGui() {
        view_.init(gui());
    }

    ///////////////////////////////////////////////////////////////
    void GameOverScene::initButtonEvents() {
        if (cache().getValue<int>("LEVEL_RESTART_COUNT") > 0) {
            // Replenish pacmans lives and restart level when "Restart Level" button is clicked
            gui().getWidget("btnRetryLevel")->on("click", ime::Callback<>([this] {
                cache().setValue("LEVEL_RESTART_COUNT", cache().getValue<int>("LEVEL_RESTART_COUNT") - 1);
                cache().setValue("PLAYER_LIVES", Constants::PLAYER_LiVES);
                engine().removeAllScenesExceptActive();
                engine().popScene(); // Destroy this scene
                engine().pushScene(std::make_unique<GameplayScene>());
            }));
        } else {
            gui().removeWidget("btnRetryLevel");

            // Reduce vertical size of container to accommodate only two buttons instead of three
            gui().getWidget("vlButtonsContainer")->setHeight("40%");
        }

        // Restart the game from the beginning
        gui().getWidget("btnStartOver")->on("click", ime::Callback<>([this] {
            cache().setValue("LEVEL_RESTART_COUNT", Constants::MAX_NUM_LEVEL_RESTARTS);
            cache().setValue("PLAYER_LIVES", Constants::PLAYER_LiVES);
            cache().setValue("CURRENT_LEVEL", 1);
            cache().setValue("CURRENT_SCORE", 0);
            engine().removeAllScenesExceptActive();
            engine().popScene();
            engine().pushScene(std::make_unique<GameplayScene>());
        }));

        // Exit to the games main menu when "Exit to Main Menu" is clicked
        gui().getWidget("btnExitMainMenu")->on("click", ime::Callback<>([this] {
            cache().setValue("LEVEL_RESTART_COUNT", Constants::MAX_NUM_LEVEL_RESTARTS);
            cache().setValue("CURRENT_LEVEL", 1);
            cache().setValue("CURRENT_SCORE", 0);
            cache().setValue("PLAYER_LIVES", Constants::PLAYER_LiVES);

            engine().removeAllScenesExceptActive();
            engine().popScene(); // Destroy this scene
            engine().pushScene(std::make_unique<MainMenuScene>());
        }));

        // Exit to desktop when "Exit Game" button is clicked
        gui().getWidget("btnExitGame")->on("click", ime::Callback<>([this] {
            engine().quit();
        }));
    }

} // namespace pm
