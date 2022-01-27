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

#include "PauseMenuScene.h"
#include "MainMenuScene.h"
#include "GameplayScene.h"
#include "Views/PauseMenuSceneView.h"
#include "Common/Constants.h"
#include <IME/ui/widgets/Slider.h>
#include <IME/ui/widgets/Label.h>
#include <IME/core/engine/Engine.h>

namespace pm {
    ///////////////////////////////////////////////////////////////
    void PauseMenuScene::onEnter() {
        PauseMenuSceneView::init(getGui());
        initEventHandlers();
    }

    ///////////////////////////////////////////////////////////////
    void PauseMenuScene::initEventHandlers() {
        // Resume button click handler
        getGui().getWidget("btnResume")->on("click", ime::Callback<>([this] {
            getEngine().popScene();
        }));

        // Restart button click handler
        if (getCache().getValue<int>("LEVEL_RESTART_COUNT") > 0) {
            getGui().getWidget("btnRestart")->on("click", ime::Callback<>([this] {
                getCache().setValue("LEVEL_RESTART_COUNT", getCache().getValue<int>("LEVEL_RESTART_COUNT") - 1);
                getEngine().removeAllScenesExceptActive(); // Popping scene twice will call GamePlayScene::onResume(), which we don't want to do
                getEngine().popScene();
                getEngine().pushScene(std::make_unique<GameplayScene>());
            }));
        } else {
            getGui().removeWidget("btnRestart");
            getGui().getWidget("vlPauseMenu")->setHeight("35%");
        }

        // Main menu button click handler
        getGui().getWidget("btnMainMenu")->on("click", ime::Callback<>([this] {
            getCache().setValue("LEVEL_RESTART_COUNT", Constants::MAX_NUM_LEVEL_RESTARTS);
            getCache().setValue("PLAYER_LIVES", Constants::PLAYER_LiVES);
            getCache().setValue("CURRENT_LEVEL", 1);
            getCache().setValue("CURRENT_SCORE", 0);
            getEngine().removeAllScenesExceptActive();
            getEngine().popScene();
            getEngine().pushScene(std::make_unique<MainMenuScene>());
        }));

        // Exit button click handler
        getGui().getWidget("btnExit")->on("click", ime::Callback<>([this] {
            getEngine().quit();
        }));

        // Volume slider value change handler
        auto* slMasterVolume = getGui().getWidget<ime::ui::Slider>("slMasterVolume");
        auto masterVolume = getSCache().getPref("MASTER_VOLUME").getValue<float>();
        slMasterVolume->setValue(masterVolume);
        getGui().getWidget<ime::ui::Label>("lblMasterVolume")->setText(std::to_string((int) masterVolume));

        slMasterVolume->on("valueChange", ime::Callback<float>([this](float value) {
            getGui().getWidget<ime::ui::Label>("lblMasterVolume")->setText(std::to_string((int) value));
            getSCache().getPref("MASTER_VOLUME").setValue( value);
        }));

        // Key press handler
        getInput().onKeyUp([this](ime::Keyboard::Key key) {
            if (key == ime::Keyboard::Key::Escape || key == ime::Keyboard::Key::P)
                getEngine().popScene();
        });
    }

} // namespace pm
