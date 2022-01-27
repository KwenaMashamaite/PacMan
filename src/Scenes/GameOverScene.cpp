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
        score.value_ = getCache().getValue<int>("CURRENT_SCORE");
        score.level_ = getCache().getValue<int>("CURRENT_LEVEL");

        auto name = getSCache().getPref("PLAYER_NAME").getValue<std::string>();
        name.copy(score.owner_, name.length() + 1);

        auto scoreboard = getCache().getValue<std::shared_ptr<Scoreboard>>("SCOREBOARD");
        scoreboard->addScore(score);
        scoreboard->updateHighScoreFile();
    }

    ///////////////////////////////////////////////////////////////
    void GameOverScene::initGui() {
        view_.init(getGui());
    }

    ///////////////////////////////////////////////////////////////
    void GameOverScene::initButtonEvents() {
        if (getCache().getValue<int>("LEVEL_RESTART_COUNT") > 0) {
            // Replenish pacmans lives and restart level when "Restart Level" button is clicked
            getGui().getWidget("btnRetryLevel")->on("click", ime::Callback<>([this] {
                getCache().setValue("LEVEL_RESTART_COUNT", getCache().getValue<int>("LEVEL_RESTART_COUNT") - 1);
                getCache().setValue("PLAYER_LIVES", Constants::PLAYER_LiVES);
                getEngine().removeAllScenesExceptActive();
                getEngine().popScene(); // Destroy this scene
                getEngine().pushScene(std::make_unique<GameplayScene>());
            }));
        } else {
            getGui().removeWidget("btnRetryLevel");

            // Reduce vertical size of container to accommodate only two buttons instead of three
            getGui().getWidget("vlButtonsContainer")->setHeight("40%");
        }

        // Restart the game from the beginning
        getGui().getWidget("btnStartOver")->on("click", ime::Callback<>([this] {
            getCache().setValue("LEVEL_RESTART_COUNT", Constants::MAX_NUM_LEVEL_RESTARTS);
            getCache().setValue("PLAYER_LIVES", Constants::PLAYER_LiVES);
            getCache().setValue("CURRENT_LEVEL", 1);
            getCache().setValue("CURRENT_SCORE", 0);
            getEngine().removeAllScenesExceptActive();
            getEngine().popScene();
            getEngine().pushScene(std::make_unique<GameplayScene>());
        }));

        // Exit to the games main menu when "Exit to Main Menu" is clicked
        getGui().getWidget("btnExitMainMenu")->on("click", ime::Callback<>([this] {
            getCache().setValue("LEVEL_RESTART_COUNT", Constants::MAX_NUM_LEVEL_RESTARTS);
            getCache().setValue("CURRENT_LEVEL", 1);
            getCache().setValue("CURRENT_SCORE", 0);
            getCache().setValue("PLAYER_LIVES", Constants::PLAYER_LiVES);

            getEngine().removeAllScenesExceptActive();
            getEngine().popScene(); // Destroy this scene
            getEngine().pushScene(std::make_unique<MainMenuScene>());
        }));

        // Exit to desktop when "Exit Game" button is clicked
        getGui().getWidget("btnExitGame")->on("click", ime::Callback<>([this] {
            getEngine().quit();
        }));
    }

} // namespace pm
