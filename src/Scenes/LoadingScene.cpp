
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

#include "LoadingScene.h"
#include "MainMenuScene.h"
#include "Views/LoadingSceneView.h"
#include <IME/core/engine/Engine.h>
#include <IME/ui/widgets/ProgressBar.h>
#include <IME/ui/widgets/Label.h>
#include <IME/core/resources/ResourceLoader.h>
#include <thread>

namespace pm {
    ///////////////////////////////////////////////////////////////
    void loadGameAssets() {
        ime::ResourceLoader::loadFromFile(ime::ResourceType::Font, {
            "ChaletLondonNineteenSixty.ttf", "AtariClassicExtrasmooth-LxZy.ttf",
            "namco.ttf", "pacfont.ttf", "DejaVuSans.ttf"
        });

        ime::ResourceLoader::loadFromFile(ime::ResourceType::Texture,  {
            "pacman_logo.png", "spritesheet.png", "main_menu_background.jpg",
            "main_menu_background_blurred.jpg"
        });

        ime::ResourceLoader::loadFromFile(ime::ResourceType::SoundEffect, {
            "fruitEaten.wav", "ghostEaten.wav", "pacmanDying.wav", "powerPelletEaten.wav",
            "wieu_wieu_slow.ogg", "WakkaWakka.ogg", "extraLife.wav", "ready.wav"
        });
    }

    ///////////////////////////////////////////////////////////////
    void LoadingScene::onEnter() {
        getWindow().setDefaultOnCloseHandlerEnable(false);
        LoadingSceneView::init(getGui());

        getGui().getWidget("pbrAssetLoading")->on("full", ime::Callback<>([this] {
            getGui().getWidget<ime::ui::Label>("lblLoading")->setText("Resources loaded successfully");
            getEngine().popScene();
        }));

        getTimer().setInterval(ime::milliseconds(60), [this] {
            getGui().getWidget<ime::ui::ProgressBar>("pbrAssetLoading")->incrementValue();
        });

        // The progress bar and the thread are not in sync. The thread finishes
        // almost intently and the progress bar is just for user experience
        std::thread(&loadGameAssets).detach();
    }

    ///////////////////////////////////////////////////////////////
    void LoadingScene::onExit() {
        getEngine().pushScene(std::make_unique<MainMenuScene>());
    }

} // namespace pm
