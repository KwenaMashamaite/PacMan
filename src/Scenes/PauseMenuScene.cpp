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
        PauseMenuSceneView::init(gui());
        initEventHandlers();
    }

    ///////////////////////////////////////////////////////////////
    void PauseMenuScene::initEventHandlers() {
        // 1. Resume button click handler
        gui().getWidget("btnResume")->on("click", ime::Callback<>([this] {
            engine().popScene();
        }));

        // 2. Restart button click handler
        if (cache().getValue<int>("LEVEL_RESTART_COUNT") > 0) {
            gui().getWidget("btnRestart")->on("click", ime::Callback<>([this] {
                cache().setValue("LEVEL_RESTART_COUNT", cache().getValue<int>("LEVEL_RESTART_COUNT") - 1);
                engine().removeAllScenesExceptActive(); // Popping scene twice will call GamePlayScene::onResume(), which we don't want to do
                engine().popScene();
                engine().pushScene(std::make_unique<GameplayScene>());
            }));
        } else {
            gui().removeWidget("btnRestart");
            gui().getWidget("vlPauseMenu")->setHeight("35%");
        }

        // 3. Main menu button click handler
        gui().getWidget("btnMainMenu")->on("click", ime::Callback<>([this] {
            cache().setValue("LEVEL_RESTART_COUNT", Constants::MAX_NUM_LEVEL_RESTARTS);
            cache().setValue("PLAYER_LIVES", Constants::PLAYER_LiVES);
            cache().setValue("CURRENT_LEVEL", 1);
            cache().setValue("CURRENT_SCORE", 0);
            engine().removeAllScenesExceptActive();
            engine().popScene();
            engine().pushScene(std::make_unique<MainMenuScene>());
        }));

        // 4. Exit button click handler
        gui().getWidget("btnExit")->on("click", ime::Callback<>([this] {
            engine().quit();
        }));

        // 5. Volume slider
        auto* slMasterVolume = gui().getWidget<ime::ui::Slider>("slMasterVolume");
        auto masterVolume = sCache().getPref("MASTER_VOLUME").getValue<float>();
        slMasterVolume->setValue(masterVolume);
        gui().getWidget<ime::ui::Label>("lblMasterVolume")->setText(std::to_string((int) masterVolume));

        slMasterVolume->on("valueChange", ime::Callback<float>([this](float value) {
            gui().getWidget<ime::ui::Label>("lblMasterVolume")->setText(std::to_string((int) value));
            sCache().getPref("MASTER_VOLUME").setValue( value);
        }));

        // Return to game when escape is pressed
        input().onKeyUp([this](ime::Keyboard::Key key) {
            if (key == ime::Keyboard::Key::Escape || key == ime::Keyboard::Key::P)
                engine().popScene();
        });
    }

} // namespace pm
