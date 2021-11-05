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
#include "src/models/scenes/GameOverScene.h"
#include "src/models/scenes/CollisionResponseRegisterer.h"
#include <IME/core/engine/Engine.h>
#include <IME/ui/widgets/Label.h>
#include <src/utils/Utils.h>
#include <cassert>

namespace pm {
    ///////////////////////////////////////////////////////////////
    GameplayScene::GameplayScene() :
        currentLevel_{-1},
        pointsMultiplier_{1},
        eatenPelletsCount_{0},
        view_{gui()},
        chaseModeWaveLevel_{0},
        scatterModeWaveLevel_{0},
        numGhostsInHouse_{0}
    {}

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onEnter() {
        currentLevel_ = cache().getValue<int>("CURRENT_LEVEL");
        audio().setMasterVolume(cache().getValue<float>("MASTER_VOLUME"));

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
        grid_->setPosition(ime::Vector2f{-34, 0});
        grid_->setBackgroundImagePosition(ime::Vector2f{246.0f, 298.0f});

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
        lblGetReady->setPosition(tilemap().getTile(Constants::FRUIT_SPAWN_POSITION).getWorldCentre());
        gui().addWidget(std::move(lblGetReady), "lblReady");
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::createActors() {
        ObjectCreator::createObjects(physWorld(), *grid_);

        grid_->forEachActor([this](ime::GameObject* actor) {
            if (actor->getClassName() == "PacMan") {
                static_cast<PacMan*>(actor)->setLivesCount(cache().getValue<int>("PLAYER_LIVES"));
            } else if (actor->getClassName() == "Ghost") {
                actor->getUserData().addProperty({"is_in_tunnel", false});
                actor->getCollisionExcludeList().add("tunnelExitSensor");

                // Flag ghost as inside ghost house - Can leave at any time
                actor->getUserData().addProperty({"is_in_ghost_house", !(actor->getTag() == "blinky")});

                // Lock ghosts in ghost house - Can't leave until it's locked sentence expires
                if (actor->getTag() == "blinky" ||
                    (actor->getTag() == "pinky" && currentLevel_ > 1) ||
                    (actor->getTag() == "inky" && currentLevel_ > 2) ||
                    (actor->getTag() == "clyde" && currentLevel_ > 3))
                {
                    actor->getUserData().addProperty({"is_locked_in_ghost_house", false});
                } else {
                    actor->getUserData().addProperty({"is_locked_in_ghost_house", true});
                    numGhostsInHouse_ += 1;
                }
            }
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::createGridMovers() {
        auto pacmanGridMover = std::make_unique<PacManGridMover>(tilemap(), gameObjects().findByTag<PacMan>("pacman"));
        pacmanGridMover->init();
        updatePacmanSpeed(pacmanGridMover.get());

        pacmanGridMover->onAdjacentMoveBegin([this](ime::Index index) {
            emit(GameEvent::PacManMoved);
        });

        gridMovers().addObject(std::move(pacmanGridMover));

        // 2. Create movement controllers for all ghost
        gameObjects().forEachInGroup("Ghost", [this](ime::GameObject* ghostBase) {
            auto* ghost = static_cast<Ghost*>(ghostBase);
            auto ghostMover = std::make_unique<GhostGridMover>(tilemap(), ghost);
            updateGhostSpeed(ghostMover.get());
            ghost->initFSM(ghostMover.get());
            ghostMover->setTag(ghost->getTag() + "GridMover");

#ifndef NDEBUG
            ghostMover->setPathViewEnable(true);
#endif

            int stateChangeId = ghost->onPropertyChange("state", [this, gridMover = ghostMover.get()](const ime::Property&) {
                updateGhostSpeed(gridMover);
            });

            ghostMover->onDestruction([gridMover = ghostMover.get(), stateChangeId] {
                if (auto* pGhost = gridMover->getTarget())
                    pGhost->unsubscribe("state", stateChangeId);
            });

            gridMovers().addObject(std::move(ghostMover), "GhostMovers");
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::intiGameEvents() {
        input().onKeyUp([this](ime::Keyboard::Key key) {
            if ((key == ime::Keyboard::Key::P || key == ime::Keyboard::Key::Escape))
                pauseGame();
        });

        eventEmitter().on("levelComplete", ime::Callback<>([this] {
            engine().onFrameEnd(nullptr);
            scatterModeTimer_.stop();
            chaseModeTimer_.stop();
            frightenedModeTimer_.stop();
            ghostHouseTimer_.stop();
            uneatenFruitTimer_.stop();

            if (gameObjects().hasGroup("Fruit"))
                gameObjects().getGroup("Fruit").removeAll();

            gameObjects().getGroup("Ghost").removeAll();
            gridMovers().getGroup("GhostMovers").removeAll();

            // Momentarily freeze pacman before flashing the grid
            gameObjects().findByTag("pacman")->getSprite().getAnimator().setTimescale(0);
            gridMovers().findByTag("pacmanGridMover")->setMovementFreeze(true);


            timer().setTimeout(ime::milliseconds(500), [this] {
                completeLevel();
            });
        }));
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::initEngineEvents() {
        engine().getWindow().onClose([this] {
            pauseGame();
        });

        engine().onFrameEnd([this] {
            gameObjects().removeIf([](const ime::GameObject* actor) {
                return !actor->isActive();
            });

            if (gameObjects().getGroup("Pellet").getCount() == 0)
                eventEmitter().emit("levelComplete");
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::initCollisionResponses() {
        ///@brief Teleports an actor to the other side of the tunnel
        ///@param gridMover The grid mover of the actor that triggered the sensor
        ///@param sensorTrigger The actor that triggered the sensor
        auto onTeleportationSensorTrigger = [](ime::GridMover* gridMover, ime::GameObject* sensorTrigger) {
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

        ime::GridMover* pacmanGridMover = gridMovers().findByTag("pacmanGridMover");

        static CollisionResponseRegisterer collisionResponseRegisterer(*this);
        collisionResponseRegisterer.registerPacmanHandlers(*pacmanGridMover->getTarget());

        pacmanGridMover->onGameObjectCollision([=](ime::GameObject* pacman, ime::GameObject* other) {
            if (other->getTag() == "teleportationSensor")
                onTeleportationSensorTrigger(pacmanGridMover, pacman);
        });

        /*-------------- Ghosts collision handlers -----------------------*/

        ///@brief Slow down ghost when it enters a tunnel
        auto onTunnelEntrySensorTrigger = [this](ime::GridMover* ghostGridMover, ime::GameObject* ghost) {
            ghost->getCollisionExcludeList().add("tunnelEntrySensor");
            ghost->getCollisionExcludeList().remove("tunnelExitSensor");
            ghost->getUserData().setValue("is_in_tunnel", true);
            updateGhostSpeed(ghostGridMover);
        };

        ///@brief Revert to normal speed when ghost exits tunnel
        auto onTunnelExitSensorTrigger = [this](ime::GridMover* ghostGridMover, ime::GameObject* ghost) {
            ghost->getCollisionExcludeList().add("tunnelExitSensor");
            ghost->getCollisionExcludeList().remove("tunnelEntrySensor");
            ghost->getUserData().setValue("is_in_tunnel", false);
            updateGhostSpeed(ghostGridMover);
        };

        ///@brief Subscribe collision handlers to ghost grid mover
        gridMovers().forEachInGroup("GhostMovers", [=] (ime::GridMover* ghostMover){
            ghostMover->onGameObjectCollision([=](ime::GameObject* ghost, ime::GameObject* other) {
                if (other->getClassName() == "Sensor") {
                    if (other->getTag() == "tunnelEntrySensor")
                        onTunnelEntrySensorTrigger(ghostMover, ghost);
                    else if (other->getTag() == "tunnelExitSensor")
                        onTunnelExitSensorTrigger(ghostMover, ghost);
                    else if (other->getTag() == "ghostHouseGateSensor") {
                        // Flag ghost as being inside or outside the ghost house
                        ghost->getUserData().setValue("is_in_ghost_house", !ghost->getUserData().getValue<bool>("is_in_ghost_house"));
                    } else if (other->getTag() == "teleportationSensor")
                        onTeleportationSensorTrigger(ghostMover, ghost);
                }
            });
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::startCountDown() {
        gui().getWidget("lblReady")->setVisible(true);
        gameObjects().forEach([] (ime::GameObject* actor) {
            actor->getSprite().getAnimator().setTimescale(0.0f);
        });

        timer().setTimeout(ime::seconds(Constants::LEVEL_START_DELAY), [this] {
            gameObjects().forEach([] (ime::GameObject* actor) {
                actor->getSprite().getAnimator().setTimescale(1.0f);
            });

            gui().getWidget("lblReady")->setVisible(false);
            auto* pacman = gameObjects().findByTag<PacMan>("pacman");
            pacman->getSprite().getAnimator().setTimescale(1.0f);
            pacman->setState(PacMan::State::Moving);

            auto* soundEffect = audio().play(ime::audio::Type::Sfx, "wieu_wieu_slow.ogg");
            soundEffect->setLoop(true);

            startGhostHouseTimer();
            startGhostScatterMode();
            emit(GameEvent::LevelStarted);
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::pauseGame() {
        setOnPauseAction(ime::Scene::OnPauseAction::Show);
        audio().pauseAll();
        engine().pushScene(std::make_unique<PauseMenuScene>());
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::completeLevel() {
        audio().stopAll();
        gameObjects().removeByTag("pacman");
        cache().setValue("CURRENT_LEVEL", currentLevel_ + 1);
        ime::Time gridAnimDuration = grid_->playFlashAnimation();

        // Starts a new level shortly after the grid stops flashing
        timer().setTimeout(gridAnimDuration + ime::seconds(1), [this] {
            engine().popScene();
            engine().pushScene(std::make_unique<GameplayScene>());
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::resetLevel() {
        resetActors();
        startCountDown();
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::resetActors() {
        gridMovers().removeAll();

        // Reset pacmans position in the grid
        auto pacman = gameObjects().findByTag("pacman");
        tilemap().removeChild(pacman);
        tilemap().addChild(pacman, Constants::PACMAN_SPAWN_TILE);
        static_cast<PacMan*>(pacman)->setState(PacMan::State::Idle);
        static_cast<PacMan*>(pacman)->setDirection(ime::Left);

        // Reset ghosts
        gameObjects().forEachInGroup("Ghost", [this](ime::GameObject* ghost) {
            ghost->getSprite().setVisible(true);

            // Reset ghost positions in the grid
            tilemap().removeChild(ghost);
            if (ghost->getTag() == "blinky") {
                tilemap().addChild(ghost, Constants::BLINKY_SPAWN_TILE);
                static_cast<Ghost*>(ghost)->setDirection(ime::Left);
            } else if (ghost->getTag() == "pinky") {
                tilemap().addChild(ghost, Constants::PINKY_SPAWN_TILE);
                static_cast<Ghost *>(ghost)->setDirection(ime::Down);
            } else if (ghost->getTag() == "inky") {
                tilemap().addChild(ghost, Constants::INKY_SPAWN_TILE);
                static_cast<Ghost*>(ghost)->setDirection(ime::Up);
            } else {
                tilemap().addChild(ghost, Constants::CLYDE_SPAWN_TILE);
                static_cast<Ghost*>(ghost)->setDirection(ime::Up);
            }

            // Reset ghost house properties
            numGhostsInHouse_ = 0;
            ghost->getUserData().setValue("is_in_ghost_house", !(ghost->getTag() == "blinky"));

            if (ghost->getTag() == "blinky" ||
                (ghost->getTag() == "pinky" && currentLevel_ > 1) ||
                (ghost->getTag() == "inky" && currentLevel_ > 2) ||
                (ghost->getTag() == "clyde" && currentLevel_ > 3))
            {
                ghost->getUserData().addProperty({"is_locked_in_ghost_house", false});
            } else {
                ghost->getUserData().addProperty({"is_locked_in_ghost_house", true});
                numGhostsInHouse_ += 1;
            }
        });

        createGridMovers();
        initCollisionResponses();
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::endGameplay() {
        engine().popScene();
        engine().pushScene(std::make_unique<GameOverScene>());
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onPrePacmanDeathAnim() {
        audio().stopAll();

        chaseModeTimer_.stop();
        frightenedModeTimer_.stop();
        scatterModeTimer_.stop();
        uneatenFruitTimer_.stop();
        ghostHouseTimer_.stop();

        auto pacman = gameObjects().findByTag<PacMan>("pacman");
        pacman->setLivesCount(pacman->getLivesCount() - 1);
        cache().setValue("PLAYER_LIVES", pacman->getLivesCount());
        view_.updateLives(pacman->getLivesCount());

        // Destroy fruit if it was spawned
        gameObjects().forEachInGroup("Fruit", [](ime::GameObject* fruit) {
            fruit->setActive(false);
        });

        gridMovers().forEach([](ime::GridMover* gridMover) {
            gridMover->setMovementFreeze(true);
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onPostPacmanDeathAnim() {
        if (gameObjects().findByTag<PacMan>("pacman")->getLivesCount() <= 0)
            endGameplay();
        else
            resetLevel();
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
        ghostHouseTimer_.update(deltaTime);
        chaseModeTimer_.update(deltaTime);
        scatterModeTimer_.update(deltaTime);
        frightenedModeTimer_.update(deltaTime);
        uneatenFruitTimer_.update(deltaTime);
        flashGhosts();
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
    void GameplayScene::emit(GameEvent event) {
        ime::PropertyContainer args;
        switch (event) {
            case GameEvent::LevelStarted:
                args.addProperty({"level", currentLevel_});
                break;
            default:
                break;
        }

        gameObjects().forEachInGroup("Ghost", [event, &args](ime::GameObject* ghost) {
            static_cast<Ghost*>(ghost)->handleEvent(event, args);
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::updatePacmanSpeed(ime::GridMover *gridMover) const {
        float speed;
        if (currentLevel_ == 1)
            speed = 0.80f * Constants::PACMAN_SPEED;
        else if (currentLevel_ >= 2 && currentLevel_ <= 4)
            speed = 0.90f * Constants::PACMAN_SPEED;
        else if (currentLevel_ >= 5 && currentLevel_ <= 20)
            speed = Constants::PACMAN_SPEED;
        else
            speed = 0.90f * Constants::PACMAN_SPEED;

        gridMover->setMaxLinearSpeed(ime::Vector2f{speed, speed});
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::updateGhostSpeed(ime::GridMover *gridMover) const {
        float speed = 0.0f;
        auto* ghost = static_cast<Ghost*>(gridMover->getTarget());

        if (ghost->getState() == Ghost::State::Eaten)
            speed = 2.0f * Constants::PACMAN_SPEED;
        else if (currentLevel_ == 1) {
            if (ghost->getState() == Ghost::State::Frightened)
                speed = 0.50f * Constants::PACMAN_SPEED;
            else if (ghost->getUserData().getValue<bool>("is_in_tunnel"))
                speed = 0.40 * Constants::PACMAN_SPEED;
            else
                speed = 0.75 * Constants::PACMAN_SPEED;
        } else if (currentLevel_ >= 2 && currentLevel_ <= 4) {
            if (ghost->getState() == Ghost::State::Frightened)
                speed = 0.55f * Constants::PACMAN_SPEED;
            else if (ghost->getUserData().getValue<bool>("is_in_tunnel"))
                speed = 0.45f * Constants::PACMAN_SPEED;
            else
                speed = 0.85f * Constants::PACMAN_SPEED;
        } else {
            if (ghost->getState() == Ghost::State::Frightened) // Stops triggering from level 19 onwards
                speed = 0.60f * Constants::PACMAN_SPEED;
            else if (ghost->getUserData().getValue<bool>("is_in_tunnel"))
                speed = 0.50f * Constants::PACMAN_SPEED;
            else
                speed = 0.95f * Constants::PACMAN_SPEED;
        }

        gridMover->setMaxLinearSpeed(ime::Vector2f{speed, speed});
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::spawnFruit() {
        Fruit::Type fruitType;
        if (currentLevel_ == 1)
            fruitType = Fruit::Type::Cherry;
        else if (currentLevel_ == 2)
            fruitType = Fruit::Type::Strawberry;
        else if (currentLevel_ == 3 || currentLevel_ == 4)
            fruitType = Fruit::Type::Peach;
        else if (currentLevel_ == 5 || currentLevel_ == 6)
            fruitType = Fruit::Type::Apple;
        else if (currentLevel_ == 7 || currentLevel_ == 8)
            fruitType = Fruit::Type::Melon;
        else if (currentLevel_ == 9 || currentLevel_ == 10)
            fruitType = Fruit::Type::Galaxian;
        else if (currentLevel_ == 11 || currentLevel_ == 12)
            fruitType = Fruit::Type::Bell;
        else
            fruitType = Fruit::Type::Key;

        auto fruit = std::make_unique<Fruit>(*this, fruitType);

        // Destroy fruit if left uneaten for some time
        uneatenFruitTimer_.setTimeoutCallback([fruitPtr = fruit.get()] {
            fruitPtr->setActive(false);
        });

        uneatenFruitTimer_.setInterval(ime::seconds(Constants::UNEATEN_FRUIT_DESTRUCTION_DELAY));
        uneatenFruitTimer_.start();

        grid_->addActor(std::move(fruit), Constants::FRUIT_SPAWN_POSITION);
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::replaceFruitWithScore(ime::GameObject* fruit) {
        fruit->getSprite().setTexture("spritesheet.png");

        if (fruit->getTag() == "cherry")
            fruit->getSprite().setTextureRect(ime::UIntRect{1, 116, 16, 16});  // 100
        else if (fruit->getTag() == "strawberry")
            fruit->getSprite().setTextureRect(ime::UIntRect{18, 116, 16, 16}); // 300
        else if (fruit->getTag() == "peach")
            fruit->getSprite().setTextureRect(ime::UIntRect{35, 116, 16, 16}); // 500
        else if (fruit->getTag() == "apple")
            fruit->getSprite().setTextureRect(ime::UIntRect{52, 116, 16, 16}); // 700
        else if (fruit->getTag() == "melon")
            fruit->getSprite().setTextureRect(ime::UIntRect{69, 116, 16, 16}); // 1000
        else if (fruit->getTag() == "galaxian")
            fruit->getSprite().setTextureRect(ime::UIntRect{86, 116, 16, 16}); // 2000
        else if (fruit->getTag() == "bell")
            fruit->getSprite().setTextureRect(ime::UIntRect{103, 116, 16, 16}); // 3000
        else if (fruit->getTag() == "key")
            fruit->getSprite().setTextureRect(ime::UIntRect{120, 116, 16, 16}); // 5000

        // Destroy fruit after some seconds have passed since it was replaced by score texture
        timer().setTimeout(ime::seconds(Constants::EATEN_FRUIT_DESTRUCTION_DELAY), [this, id = fruit->getObjectId()] {
            if (auto* fruit = gameObjects().findById(id))
                fruit->setActive(false);
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::replaceGhostWithScore(ime::GameObject *pacman, ime::GameObject *ghost) {
        // Freeze animations to prevent texture changes while score texture is displayed
        pacman->getSprite().getAnimator().setTimescale(0.0f);
        ghost->getSprite().getAnimator().setTimescale(0.0f);

        pacman->getSprite().setVisible(false);
        ghost->getSprite().setTexture("spritesheet.png");

        if (pointsMultiplier_ == 1)
            ghost->getSprite().setTextureRect(ime::UIntRect{137, 116, 16, 16}); // 200
        else if (pointsMultiplier_ == 2)
            ghost->getSprite().setTextureRect(ime::UIntRect{154, 116, 16, 16}); // 400
        else if (pointsMultiplier_ == 4)
            ghost->getSprite().setTextureRect(ime::UIntRect{171, 116, 16, 16}); // 800
        else
            ghost->getSprite().setTextureRect(ime::UIntRect{188, 116, 16, 16}); // 1600

        timer().setTimeout(ime::seconds(Constants::ACTOR_FREEZE_DURATION), [pacman, ghost] {
            pacman->getSprite().getAnimator().setTimescale(1.0f);
            ghost->getSprite().getAnimator().setTimescale(1.0f);

            pacman->getSprite().setVisible(true);
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::setMovementFreeze(bool freeze) {
        gridMovers().findByTag("pacmanGridMover")->setMovementFreeze(freeze);

        gridMovers().forEachInGroup("GhostMovers", [freeze](ime::GridMover* gridMover) {
            Ghost::State state = static_cast<Ghost*>(gridMover->getTarget())->getState();
            if ((state == Ghost::State::Eaten && freeze) ||
                (!freeze && !gridMover->isMovementFrozen()))
            {
                return;
            }

            gridMover->setMovementFreeze(freeze);
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::startGhostFrightenedMode() {
        ime::Time duration;
        if (currentLevel_ > Constants::GHOST_VULNERABILITY_LEVEL_CUTOFF)
            duration = ime::seconds(0);
        else if (currentLevel_ == 9 || currentLevel_ == 12 || currentLevel_ == 13 ||
            currentLevel_ == 15 || currentLevel_ == 16 || currentLevel_ == 18)
        {
            duration = ime::seconds(1);
        }
        else if (currentLevel_ == 5 || currentLevel_ == 7 || currentLevel_ == 8 ||
            currentLevel_ == 11 || currentLevel_ == 12)
        {
            duration = ime::seconds(2);
        }
        else if (currentLevel_ == 4 || currentLevel_ == 14)
            duration = ime::seconds(3);
        else if (currentLevel_ == 3)
            duration = ime::seconds(4);
        else if (currentLevel_ == 6 || currentLevel_ == 10 || currentLevel_ == 2)
            duration = ime::seconds(5);
        else if (currentLevel_ == 1)
            duration = ime::seconds(6);

        if (frightenedModeTimer_.getStatus() == ime::Timer::Status::Running) // Increase frightened mode expiry time
            frightenedModeTimer_.setInterval(frightenedModeTimer_.getRemainingDuration() + duration);
        else {
            if (scatterModeTimer_.getStatus() == ime::Timer::Status::Running)
                scatterModeTimer_.pause();
            else if (chaseModeTimer_.getStatus() == ime::Timer::Status::Running)
                chaseModeTimer_.pause();
            else {
                assert(false && "Ghost entered frightened mode from an invalid state: valid states are scatter and chase");
            }

            frightenedModeTimer_.setInterval(duration);
            frightenedModeTimer_.setTimeoutCallback([this] {
                pointsMultiplier_ = 1;
                emit(GameEvent::FrightenedModeEnd);

                // A paused timer implies that the ghost was in the state
                // controlled by the timer before being frightened
                if (scatterModeTimer_.getStatus() == ime::Timer::Status::Paused)
                    startGhostScatterMode();
                else if (chaseModeTimer_.getStatus() == ime::Timer::Status::Paused)
                    startGhostChaseMode();
                else {
                    assert(false && "Cannot determine the state the ghost was in before it was frightened");
                }
            });

            frightenedModeTimer_.start();
            emit(GameEvent::FrightenedModeBegin);
        }
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::startGhostScatterMode() {
        if (scatterModeTimer_.isPaused()) {
            scatterModeTimer_.start();
            return;
        }

        scatterModeWaveLevel_ += 1;
        ime::Time duration;

        if (scatterModeWaveLevel_ <= 2) {
            if (currentLevel_ < 5)
                duration = ime::seconds(7.0f);
            else
                duration = ime::seconds(5.0f);
        } else if (scatterModeWaveLevel_ == 3)
            duration = ime::seconds(5);
        else {
            if (currentLevel_ == 1)
                duration = ime::seconds(5.0f);
            else
                duration = ime::seconds(1.0f / engine().getWindow().getFrameRateLimit()); // one frame
        }

        // Transition to chase mode when timer expires
        scatterModeTimer_.setTimeoutCallback([this] {
            emit(GameEvent::ScatterModeEnd);
            startGhostChaseMode();
        });

        scatterModeTimer_.setInterval(duration);
        scatterModeTimer_.start();
        emit(GameEvent::ScatterModeBegin);
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::startGhostChaseMode() {
        if (chaseModeTimer_.isPaused()) {
            chaseModeTimer_.start();
            return;
        }

        chaseModeWaveLevel_ += 1;
        ime::Time duration;

        // 1st and 2nd wave always lasts 20 seconds on all levels
        if (chaseModeWaveLevel_ <= 2)
            duration = ime::seconds(20.0f);
        else if (chaseModeWaveLevel_ == 3) {
            if (currentLevel_ == 1)
                duration = ime::seconds(20.0f);
            else if (currentLevel_ > 1 && currentLevel_ < 5)
                duration = ime::seconds(1033);
            else
                duration = ime::seconds(1037);
        }

        // After the third chase wave, the ghosts remain in chase mode indefinitely.
        // Even though the timer is not needed in the fourth chase wave, we keep
        // it running so that a ghost can know in which state it should be in after
        // frightened state expires
        if (chaseModeWaveLevel_ == 4) {
            chaseModeTimer_.setInterval(ime::hours(24)); // Any duration that is long enough not to timeout
            chaseModeTimer_.setTimeoutCallback([] {});
        } else {
            chaseModeTimer_.setInterval(duration);

            chaseModeTimer_.setTimeoutCallback([this] {
                emit(GameEvent::ChaseModeEnd);
                startGhostScatterMode();
            });
        }

        chaseModeTimer_.start();

        emit(GameEvent::ChaseModeBegin);
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::startGhostHouseTimer() {
        if (numGhostsInHouse_ == 0)
            return;

        auto freeGhost = [this] {
            ime::PropertyContainer args;

            if (chaseModeTimer_.isRunning() || chaseModeTimer_.isPaused())
                args.addProperty({"nextState", Ghost::State::Chase});
            else if (scatterModeTimer_.isRunning() || scatterModeTimer_.isPaused())
                args.addProperty({"nextState", Ghost::State::Scatter});
            else
                assert(false && "Failed to determine next state for released ghost");

            if (numGhostsInHouse_ == 3) { // Release pinky
                static_cast<Ghost*>(gameObjects().getGroup("Ghost").findByTag("pinky"))->handleEvent(GameEvent::GhostFreed, args);
                ghostHouseTimer_.setInterval(ime::seconds(Constants::INKY_HOUSE_ARREST_DURATION));
            } else if (numGhostsInHouse_ == 2) { // Release inky
                static_cast<Ghost*>(gameObjects().getGroup("Ghost").findByTag("inky"))->handleEvent(GameEvent::GhostFreed, args);
                ghostHouseTimer_.setInterval(ime::seconds(Constants::CLYDE_HOUSE_ARREST_DURATION));
            } else // Release clyde
                static_cast<Ghost*>(gameObjects().getGroup("Ghost").findByTag("clyde"))->handleEvent(GameEvent::GhostFreed, args);

            numGhostsInHouse_ -= 1;
        };

        ghostHouseTimer_.setInterval(ime::seconds(Constants::PINKY_HOUSE_ARREST_DURATION)); // Pinky is freed first
        ghostHouseTimer_.setTimeoutCallback(freeGhost);
        ghostHouseTimer_.setRepeat(2);
        ghostHouseTimer_.start();
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::flashGhosts() {
        ime::Time static flashAnimCutoffTime = ime::seconds(2);

        if (frightenedModeTimer_.isRunning()) {
            gameObjects().forEachInGroup("Ghost", [this](ime::GameObject* ghostBase) {
                auto* ghost = static_cast<Ghost*>(ghostBase);
                if (ghost->getState() == Ghost::State::Frightened) {
                    if (!ghost->isFlashAnimationPlaying() && frightenedModeTimer_.getRemainingDuration() <= flashAnimCutoffTime)
                        ghost->playFlashAnimation(true);
                    else if (ghost->isFlashAnimationPlaying() && frightenedModeTimer_.getRemainingDuration() > flashAnimCutoffTime)
                        ghost->playFlashAnimation(false);
                }
            });
        }
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::updatePointsMultiplier() {
        if (pointsMultiplier_ == 8)
            pointsMultiplier_ = 1; // Also resets to 1 when power mode timer expires
        else
            pointsMultiplier_ *= 2;
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onExit() {
        engine().onFrameEnd(nullptr);
        engine().getWindow().onClose(nullptr);
    }

} // namespace pm