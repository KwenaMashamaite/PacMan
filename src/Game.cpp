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

#include "Game.h"
#include "Scoreboard/Scoreboard.h"
#include "Scenes/StartUpScene.h"
#include "Common/Constants.h"

namespace pm {
    ///////////////////////////////////////////////////////////////
    ime::PrefContainer getEngineSettings() {
        ime::PrefContainer settings;
        settings.addPref(ime::Preference{"WINDOW_TITLE", ime::PrefType::String, std::string("Pac-Man")});
        settings.addPref(ime::Preference{"WINDOW_WIDTH", ime::PrefType::Int, 493});
        settings.addPref(ime::Preference{"WINDOW_HEIGHT", ime::PrefType::Int, 600});
        settings.addPref(ime::Preference{"WINDOW_ICON", ime::PrefType::String, std::string("res/Images/window_icon.png")});
        settings.addPref(ime::Preference{"FULLSCREEN", ime::PrefType::Bool, false});
        settings.addPref(ime::Preference{"V_SYNC", ime::PrefType::Bool, false});
        settings.addPref(ime::Preference{"FPS_LIMIT", ime::PrefType::Int, 120});
        settings.addPref(ime::Preference{"FONTS_DIR", ime::PrefType::String, std::string("res/Fonts/")});
        settings.addPref(ime::Preference{"TEXTURES_DIR", ime::PrefType::String, std::string("res/Images/")});
        settings.addPref(ime::Preference{"MUSIC_DIR", ime::PrefType::String, std::string("res/Music/")});
        settings.addPref(ime::Preference{"SOUND_EFFECTS_DIR", ime::PrefType::String, std::string("res/SoundEffects/")});
        settings.addPref(ime::Preference{"HIGH_SCORES_DIR", ime::PrefType::String, std::string("res/TextFiles/")});
        settings.addPref(ime::Preference{"MAZE_DIR", ime::PrefType::String, std::string("res/TextFiles/")});
        settings.addPref(ime::Preference{"CONFIGS_DIR", ime::PrefType::String, std::string("res/TextFiles/")});

        return settings;
    }

    ///////////////////////////////////////////////////////////////
    Game::Game() :
        engine_{"Pac-Man", getEngineSettings()}
    {}

    ///////////////////////////////////////////////////////////////
    void Game::initialize() {
        // Make game window unresizable
        engine_.getWindow().setStyle(ime::WindowStyle::Close);

        // Initialize default values and cache them for later access and modification
        engine_.initialize();
        ime::PrefContainer& settings = engine_.getConfigs();

        auto scoreboard = std::make_shared<Scoreboard>(settings.getPref("HIGH_SCORES_DIR").getValue<std::string>().append("/highscores.pcmg"));
        scoreboard->load();

        engine_.getPersistentData().addProperty({"SCOREBOARD", scoreboard});
        engine_.getPersistentData().addProperty({"SETTINGS_FILENAME", settings.getPref("CONFIGS_DIR").getValue<std::string>().append("/configs.txt")});
        engine_.getPersistentData().addProperty({"HIGH_SCORE", scoreboard->getTopScore().value_});
        engine_.getPersistentData().addProperty({"CURRENT_LEVEL", 1});
        engine_.getPersistentData().addProperty({"CURRENT_SCORE", 0});
        engine_.getPersistentData().addProperty({"PLAYER_LIVES", Constants::PLAYER_LiVES});
        engine_.getPersistentData().addProperty({"LEVEL_RESTART_COUNT", Constants::MAX_NUM_LEVEL_RESTARTS});

        // Load disk configs
        engine_.getSavablePersistentData().load(engine_.getPersistentData().getValue<std::string>("SETTINGS_FILENAME"));

        // Update file on disk with new player preferences if any
        engine_.onShutDown([this] {
            engine_.getSavablePersistentData().save();
        });
    }

    ///////////////////////////////////////////////////////////////
    void Game::start() {
        engine_.pushScene(std::make_unique<StartUpScene>());
        engine_.run();
    }

} // namespace pm
