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

#include "src/models/scenes/GameplayScene.h"
#include "src/models/scenes/PauseMenuScene.h"
#include "src/utils/ObjectCreator.h"
#include "src/models/actors/Actors.h"
#include "src/common/Constants.h"
#include "src/models/actors/controllers/PacManGridMover.h"
#include "src/models/actors/controllers/GhostGridMover.h"
#include <IME/core/engine/Engine.h>
#include <IME/ui/widgets/Label.h>

namespace pm {
    ///////////////////////////////////////////////////////////////
    GameplayScene::GameplayScene() :
        view_{gui()}
    {}

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onEnter() {
        createPhysWorld({0.0f, 0.0f}); // Since we using grid based physics only, no gravity is needed
        createGrid();
        initGui();
        createActors();
        createGridMovers();
        initCollisionResponses();
        initEngineEvents();
        intiGameEvents();
        startCountDown();
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::createGrid() {
        createTilemap(Constants::GRID_TILE_SIZE, Constants::GRID_TILE_SIZE);
        grid_ = std::make_unique<Grid>(tilemap(), *this, gameObjects());
        grid_->loadFromFile(engine().getConfigs().getPref("MAZE_DIR").getValue<std::string>() + "maze.txt");
        grid_->setPosition({-42, 0});
        grid_->setBackgroundImagePosition({7.0f, 49.0f});

        // Makes the tunnel exits touch the edges of the screen
        camera().move(8.0f, 0.0f);

#ifndef NDEBUG
        grid_->setVisible(true);
#else
        grid_->setVisible(false);
#endif
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::initGui() {
        // Update view placeholder text with current level data
        view_.init(cache().getValue<int>("CURRENT_LEVEL"), cache().getValue<int>("PLAYER_LIVES"));
        view_.setHighScore(cache().getValue<int>("HIGH_SCORE"));
        view_.setScore(cache().getValue<int>("CURRENT_SCORE"));

        // Create get ready text (Displayed before level start countdown)
        auto lblGetReady = ime::ui::Label::create("Ready!");
        lblGetReady->setTextSize(15.0f);
        lblGetReady->setHorizontalAlignment(ime::ui::Label::HorizontalAlignment::Center);
        lblGetReady->setVerticalAlignment(ime::ui::Label::VerticalAlignment::Center);
        lblGetReady->getRenderer()->setTextColour(ime::Colour::Yellow);
        lblGetReady->setOrigin(0.5f, 0.5f);
        lblGetReady->setPosition(tilemap().getTile(Constants::READY_TEXT_POSITION).getWorldCentre());
        gui().addWidget(std::move(lblGetReady), "lblReady");
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::createActors() {
        ObjectCreator::createObjects(physWorld(), *grid_);

        grid_->forEachActor([this](ime::GameObject* actor) {
            if (actor->getClassName() == "PacMan") {
                actor->getSprite().getAnimator().setTimescale(0.0f);
                static_cast<PacMan*>(actor)->setLivesCount(cache().getValue<int>("PLAYER_LIVES"));
            }
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::createGridMovers() {
        auto pacmanGridMover = std::make_unique<PacManGridMover>(tilemap(), gameObjects().findByTag<PacMan>("pacman"));
        pacmanGridMover->init();
        gridMovers().addObject(std::move(pacmanGridMover));

        // 2. Create movement controllers for all ghost
        gameObjects().forEachInGroup("Ghost", [this](ime::GameObject* ghostBase) {
            auto* ghost = static_cast<Ghost*>(ghostBase);
            auto ghostMover = std::make_unique<GhostGridMover>(tilemap(), ghost);

#ifndef NDEBUG
            ghostMover->setPathViewEnable(true);
#endif

            ghost->initFSM(ghostMover.get());
            gridMovers().addObject(std::move(ghostMover), "GhostMovers");
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::intiGameEvents() {
        // 1. Display pause menu when P or Esc is pressed
        input().onKeyUp([this](ime::Keyboard::Key key) {
            if ((key == ime::Keyboard::Key::P || key == ime::Keyboard::Key::Escape))
                pauseGame();
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::initEngineEvents() {
        // Pause game menu when user requests to close game window
        engine().getWindow().onClose([this] {
            pauseGame();
        });

        // Post frame handler
        engine().onFrameEnd([this] {
            // Remove inactive objects from the scene at the end of each frame
            gameObjects().removeIf([](const ime::GameObject* actor) {
                return !actor->isActive();
            });
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::initCollisionResponses() {
        ///@brief Teleports an actor to the other side of the tunnel
        ///@param gridMover The grid mover of the actor that triggered the sensor
        ///@param sensorTrigger The actor that triggered the sensor
        auto onTunnelExitSensorTrigger = [](ime::GridMover* gridMover, ime::GameObject* sensorTrigger) {
            ime::TileMap& grid = gridMover->getGrid();
            auto prevTile = grid.getTileOccupiedByChild(sensorTrigger);
            grid.removeChild(sensorTrigger);
            if (prevTile.getIndex().colm == 0) { // Triggered the left-hand side sensor
                grid.addChild(sensorTrigger,ime::Index{prevTile.getIndex().row, static_cast<int>(grid.getSizeInTiles().x - 1)});
            } else
                grid.addChild(sensorTrigger, {prevTile.getIndex().row, 0});

            gridMover->resetTargetTile();
            gridMover->requestDirectionChange(gridMover->getDirection());
        };

        ///@brief Increase the game score when a pellet is eaten and destroy it
        ///@param pelletBase The pellet that was eaten
        auto onPelletCollision = [this](ime::GameObject* pelletBase) {
            auto pellet = static_cast<Pellet*>(pelletBase);
            pellet->setActive(false);
            if (pellet->getPelletType() == Pellet::Type::Energizer) {
                updateScore(Constants::Points::ENERGIZER);
                audio().play(ime::audio::Type::Sfx, "powerPelletEaten.wav");
            } else {
                updateScore(Constants::Points::DOT);
                audio().play(ime::audio::Type::Sfx, "WakkaWakka.wav");
            }
        };

        ime::GridMover* pacmanGridMover = gridMovers().findByTag("pacmanGridMover");

        pacmanGridMover->onGameObjectCollision([=](ime::GameObject* pacman, ime::GameObject* other) {
            if (other->getTag() == "tunnelExitSensor")
                onTunnelExitSensorTrigger(pacmanGridMover, pacman);
            else if (other->getClassName() == "Pellet")
                onPelletCollision(other);
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::startCountDown() {
        timer().setTimeout(ime::seconds(Constants::LEVEL_START_DELAY), [this] {
            gui().getWidget("lblReady")->setVisible(false);
            auto* pacman = gameObjects().findByTag<PacMan>("pacman");
            pacman->getSprite().getAnimator().setTimescale(1.0f);
            pacman->setState(PacMan::State::Moving);

            auto* soundEffect = audio().play(ime::audio::Type::Sfx, "wieu_wieu_slow.ogg");
            soundEffect->setLoop(true);
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::pauseGame() {
        setOnPauseAction(ime::Scene::OnPauseAction::Show);
        audio().pauseAll();
        engine().pushScene(std::make_unique<PauseMenuScene>());
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onPause() {
        audio().pauseAll();
        engine().onFrameEnd(nullptr);
        engine().getWindow().onClose(nullptr);
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onResume() {
        initEngineEvents();
        setOnPauseAction(ime::Scene::OnPauseAction::Default);
        audio().setMasterVolume(cache().getValue<float>("MASTER_VOLUME"));
        audio().playAll();
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::update(ime::Time deltaTime) {
        view_.update(deltaTime);
        grid_->update(deltaTime);
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::updateScore(int points) {
        auto newScore = cache().getValue<int>("CURRENT_SCORE") + points;
        cache().setValue("CURRENT_SCORE", newScore);
        view_.setScore(newScore);

        if (newScore > cache().getValue<int>("HIGH_SCORE")) {
            cache().setValue("HIGH_SCORE", newScore);
            view_.setHighScore(newScore);
        }
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onExit() {
        engine().onFrameEnd(nullptr);
        engine().getWindow().onClose(nullptr);
    }

} // namespace pm