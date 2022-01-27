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

#include "GameplayScene.h"
#include "PauseMenuScene.h"
#include "GameOverScene.h"
#include "CollisionResponseRegisterer.h"
#include "Utils/ObjectCreator.h"
#include "GameObjects/Actors.h"
#include "Common/Constants.h"
#include "PathFinders/PacManGridMover.h"
#include "PathFinders/GhostGridMover.h"
#include "Common/ObjectReferenceKeeper.h"
#include <IME/core/engine/Engine.h>
#include <IME/ui/widgets/Label.h>
#include <IME/utility/Utils.h>
#include <cassert>

namespace pm {
    ///////////////////////////////////////////////////////////////
    int GameplayScene::extraLivesGiven_{0};

    ///////////////////////////////////////////////////////////////
    GameplayScene::GameplayScene() :
        currentLevel_{-1},
        pointsMultiplier_{1},
        eatenPelletsCount_{0},
        view_{getGui()},
        chaseModeWaveLevel_{0},
        scatterModeWaveLevel_{0},
        numGhostsInHouse_{0},
        onWindowCloseId_{-1}
    {}

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onEnter() {
        currentLevel_ = getCache().getValue<int>("CURRENT_LEVEL");
        getAudio().setMasterVolume(getSCache().getPref("MASTER_VOLUME").getValue<float>());

        ObjectReferenceKeeper::clear();
        createGrid();
        initGui();
        createActors();
        createGridMovers();
        initCollisionResponses();
        intiGameEvents();
        startCountDown();
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::createGrid() {
        createGrid2D(Constants::GRID_TILE_SIZE, Constants::GRID_TILE_SIZE);
        grid_ = std::make_unique<Grid>(getGrid());
        grid_->loadFromFile(getEngine().getConfigs().getPref("MAZE_DIR").getValue<std::string>() + "maze.txt");
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
        view_.init(getCache().getValue<int>("CURRENT_LEVEL"), getCache().getValue<int>("PLAYER_LIVES"));
        view_.setHighScore(getCache().getValue<int>("HIGH_SCORE"));
        view_.setScore(getCache().getValue<int>("CURRENT_SCORE"));
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::createActors() {
        ObjectCreator::createObjects(*grid_);

        grid_->forEachActor([this](ime::GridObject* actor) {
            if (actor->getClassName() == "PacMan") {
                ObjectReferenceKeeper::registerActor(actor);
                static_cast<PacMan*>(actor)->setLivesCount(getCache().getValue<int>("PLAYER_LIVES"));
            } else if (actor->getClassName() == "Ghost") {
                ObjectReferenceKeeper::registerActor(actor);
                actor->getUserData().addProperty({"is_in_tunnel", false});
                actor->getCollisionExcludeList().add("tunnelExitSensor");
                lockGhostInHouse(actor);
            }
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::createGridMovers() {
        // Pacman
        auto* pacman = getGameObjects().findByTag<PacMan>("pacman");
        auto pacmanGridMover = std::make_unique<PacManGridMover>(getGrid(), pacman );

        pacmanGridMover->init(ime::TriggerKeys{
            ime::Keyboard::stringToKey(getSCache().getPref("MOVE_LEFT_BUTTON").getValue<std::string>()),
            ime::Keyboard::stringToKey(getSCache().getPref("MOVE_RIGHT_BUTTON").getValue<std::string>()),
            ime::Keyboard::stringToKey(getSCache().getPref("MOVE_UP_BUTTON").getValue<std::string>()),
            ime::Keyboard::stringToKey(getSCache().getPref("MOVE_DOWN_BUTTON").getValue<std::string>())
        });

        updatePacmanSpeed(pacman);

        getGridMovers().addObject(std::move(pacmanGridMover));

        // Ghosts
        getGameObjects().forEachInGroup("Ghost", [this](ime::GameObject* ghostBase) {
            auto* ghost = static_cast<Ghost*>(ghostBase);
            auto ghostMover = std::make_unique<GhostGridMover>(getGrid(), ghost);
            updateGhostSpeed(dynamic_cast<ime::GridObject*>(ghostBase));
            ghost->initFSM();

            int stateChangeId = ghost->onPropertyChange("state", [this, ghost](const ime::Property&) {
                updateGhostSpeed(ghost);
            });

            ghostMover->onDestruction([gridMover = ghostMover.get(), stateChangeId] {
                if (auto* pGhost = gridMover->getTarget())
                    pGhost->removeEventListener("state", stateChangeId);
            });

            getGridMovers().addObject(std::move(ghostMover), "GhostMovers");
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::intiGameEvents() {
        onWindowCloseId_ = getWindow().onClose([this] {
            pauseGame();
        });

        getInput().onKeyUp([this](ime::Keyboard::Key key) {
            if ((key == ime::Keyboard::Key::P || key == ime::Keyboard::Key::Escape))
                pauseGame();
        });

        getEventEmitter().addOnceEventListener("levelComplete", ime::Callback<>([this] {
            stopTimers();

            if (getGameObjects().hasGroup("Fruit"))
                getGameObjects().getGroup("Fruit").removeAll();

            getGameObjects().getGroup("Ghost").removeAll();
            getGridMovers().getGroup("GhostMovers").removeAll();

            // Momentarily freeze pacman before flashing the grid
            getGameObjects().findByTag("pacman")->getSprite().getAnimator().setTimescale(0);
            getGridMovers().findByTag("pacmanGridMover")->setMovementFreeze(true);

            getTimer().setTimeout(ime::milliseconds(500), [this] {
                completeLevel();
            });
        }));
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onFrameEnd() {
        getGameObjects().removeIf([](const ime::GameObject* actor) {
            return !actor->isActive();
        });

        if (getGameObjects().getGroup("Pellet").getCount() == 0)
            getEventEmitter().emit("levelComplete");
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::initCollisionResponses() {
        collisionResponseRegisterer_ = std::make_unique<CollisionResponseRegisterer>(*this);

        /*-------------- Pacman collision handlers -----------------------*/
        auto* pacman = getGameObjects().findByTag<PacMan>("pacman");
        collisionResponseRegisterer_->registerCollisionWithPellets(pacman);
        collisionResponseRegisterer_->registerCollisionWithFruit(pacman);
        collisionResponseRegisterer_->registerCollisionWithGhost(pacman);
        collisionResponseRegisterer_->registerCollisionWithTeleportationSensor(pacman);

        /*-------------- Ghosts collision handlers -----------------------*/
        getGameObjects().forEachInGroup("Ghost", [this] (ime::GameObject* ghost) {
            collisionResponseRegisterer_->registerCollisionWithTeleportationSensor(static_cast<ime::GridObject*>(ghost));
            collisionResponseRegisterer_->registerCollisionWithSlowDownSensor(static_cast<ime::GridObject*>(ghost));
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::startCountDown() {
        getGui().getWidget("lblReady")->setVisible(true);

        getGameObjects().forEach([] (ime::GameObject* actor) {
            actor->getSprite().getAnimator().setTimescale(0.0f);
        });

        getInput().setAllInputEnable(false);

        getTimer().setTimeout(ime::seconds(isBoot_ ? 2.15 * Constants::LEVEL_START_DELAY : Constants::LEVEL_START_DELAY), [this] {
            getGameObjects().forEach([] (ime::GameObject* actor) {
                actor->getSprite().getAnimator().setTimescale(1.0f);
            });

            getGui().getWidget("lblReady")->setVisible(false);
            auto* pacman = getGameObjects().findByTag<PacMan>("pacman");
            pacman->getSprite().getAnimator().setTimescale(1.0f);
            pacman->setState(PacMan::State::Moving);

            auto* soundEffect = getAudio().play(ime::audio::Type::Sfx, "wieu_wieu_slow.ogg");
            soundEffect->setLoop(true);

            startGhostHouseTimer();
            startGhostScatterMode();
            getInput().setAllInputEnable(true);
            emit(GameEvent::LevelStarted);
        });

        if (isBoot_) {
            isBoot_ = false;
            getAudio().play(ime::audio::Type::Sfx, "ready.wav");
        }
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::pauseGame() {
        setOnPauseAction(ime::Scene::OnPauseAction::Show);
        getAudio().pauseAll();
        getEngine().pushScene(std::make_unique<PauseMenuScene>());
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::completeLevel() {
        getAudio().stopAll();
        getGameObjects().removeByTag("pacman");
        getCache().setValue("CURRENT_LEVEL", currentLevel_ + 1);
        ime::Time gridAnimDuration = grid_->playFlashAnimation();

        // Starts a new level shortly after the grid stops flashing
        getTimer().setTimeout(gridAnimDuration + ime::seconds(1), [this] {
            getEngine().popScene();
            getEngine().pushScene(std::make_unique<GameplayScene>());
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::resetLevel() {
        resetActors();
        startCountDown();
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::resetActors() {
        getGridMovers().removeAll();

        // Pacman
        auto* pacman = getGameObjects().findByTag<PacMan>("pacman");
        getGrid().removeChild(pacman);
        getGrid().addChild(pacman, Constants::PACMAN_SPAWN_TILE);
        static_cast<PacMan*>(pacman)->setState(PacMan::State::Idle);
        static_cast<PacMan*>(pacman)->setDirection(ime::Left);

        // Ghosts
        numGhostsInHouse_ = 0;
        getGameObjects().forEachInGroup("Ghost", [this](ime::GameObject* ghostBase) {
            auto* ghost = dynamic_cast<Ghost*>(ghostBase);
            ghost->getSprite().setVisible(true);
            ghost->getUserData().setValue("is_in_tunnel", false);

            // Reset ghost positions in the grid
            getGrid().removeChild(ghost);
            if (ghost->getTag() == "blinky") {
                getGrid().addChild(ghost, Constants::BLINKY_SPAWN_TILE);
                static_cast<Ghost*>(ghost)->setDirection(ime::Left);
            } else if (ghost->getTag() == "pinky") {
                getGrid().addChild(ghost, Constants::PINKY_SPAWN_TILE);
                static_cast<Ghost *>(ghost)->setDirection(ime::Down);
            } else if (ghost->getTag() == "inky") {
                getGrid().addChild(ghost, Constants::INKY_SPAWN_TILE);
                static_cast<Ghost*>(ghost)->setDirection(ime::Up);
            } else {
                getGrid().addChild(ghost, Constants::CLYDE_SPAWN_TILE);
                static_cast<Ghost*>(ghost)->setDirection(ime::Up);
            }

            lockGhostInHouse(ghost);
        });

        createGridMovers();
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::endGameplay() {
        getGameObjects().removeByTag("pacman");
        setOnPauseAction(ime::Scene::OnPauseAction::Show | ime::Scene::OnPauseAction::UpdateTime);
        getEngine().pushScene(std::make_unique<GameOverScene>());
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onPrePacmanDeathAnim() {
        getAudio().stopAll();
        stopTimers();

        chaseModeTimer_.stop();
        frightenedModeTimer_.stop();
        scatterModeTimer_.stop();
        uneatenFruitTimer_.stop();
        ghostHouseTimer_.stop();

        auto pacman = getGameObjects().findByTag<PacMan>("pacman");
        pacman->removeLife();
        view_.removeLife();
        getCache().setValue("PLAYER_LIVES", pacman->getLivesCount());

        // Destroy fruit if it was spawned
        getGameObjects().forEachInGroup("Fruit", [](ime::GameObject* fruit) {
            fruit->setActive(false);
        });

        getGridMovers().forEach([](ime::GridMover* gridMover) {
            gridMover->setMovementFreeze(true);
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onPostPacmanDeathAnim() {
        if (getGameObjects().findByTag<PacMan>("pacman")->getLivesCount() <= 0)
            endGameplay();
        else
            resetLevel();
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onPause() {
        getWindow().suspendedEventListener(onWindowCloseId_, true);
        getAudio().pauseAll();
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onResume() {
        setOnPauseAction(ime::Scene::OnPauseAction::Default);
        getWindow().suspendedEventListener(onWindowCloseId_, false);
        getAudio().setMasterVolume(getSCache().getPref("MASTER_VOLUME").getValue<float>());
        getAudio().playAll();
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::onUpdate(ime::Time deltaTime) {
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
        auto newScore = getCache().getValue<int>("CURRENT_SCORE") + points;
        getCache().setValue("CURRENT_SCORE", newScore);
        view_.setScore(newScore);

        if (newScore > getCache().getValue<int>("HIGH_SCORE")) {
            getCache().setValue("HIGH_SCORE", newScore);
            view_.setHighScore(newScore);
        }

        if (newScore >= Constants::FIRST_EXTRA_LIFE_MIN_SCORE && extraLivesGiven_ == 0 ||
            newScore >= Constants::SECOND_EXTRA_LIFE_MIN_SCORE && extraLivesGiven_ == 1)
        {
            extraLivesGiven_++;
            auto* pacman = getGameObjects().findByTag<PacMan>("pacman");
            pacman->addLife();
            getCache().setValue("PLAYER_LIVES", pacman->getLivesCount());
            view_.addLife();

            getAudio().play(ime::audio::Type::Sfx, "extraLife.wav");
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

        getGameObjects().forEachInGroup("Ghost", [event, &args](ime::GameObject* ghost) {
            static_cast<Ghost*>(ghost)->handleEvent(event, args);
        });
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::updatePacmanSpeed(ime::GridObject* pacman) const {
        assert(pacman->getGridMover() && "Cannot update pacman's speed without a grid mover");

        float speed;
        if (currentLevel_ == 1)
            speed = 0.80f * Constants::PACMAN_SPEED;
        else if (currentLevel_ >= 2 && currentLevel_ <= 4)
            speed = 0.90f * Constants::PACMAN_SPEED;
        else if (currentLevel_ >= 5 && currentLevel_ <= 20)
            speed = Constants::PACMAN_SPEED;
        else
            speed = 0.90f * Constants::PACMAN_SPEED;

        pacman->getGridMover()->setSpeed(ime::Vector2f{speed, speed});
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::updateGhostSpeed(ime::GridObject* ghost) const {
        assert(ghost->getGridMover() && "Cannot update ghost's speed without a grid mover");

        float speed = 0.0f;
        auto* g = static_cast<Ghost*>(ghost);

        if (g->getState() == Ghost::State::Eaten)
            speed = 2.0f * Constants::PACMAN_SPEED;
        else if (currentLevel_ == 1) {
            if (g->getState() == Ghost::State::Frightened)
                speed = 0.50f * Constants::PACMAN_SPEED;
            else if (g->getUserData().getValue<bool>("is_in_tunnel"))
                speed = 0.40 * Constants::PACMAN_SPEED;
            else
                speed = 0.75 * Constants::PACMAN_SPEED;
        } else if (currentLevel_ >= 2 && currentLevel_ <= 4) {
            if (g->getState() == Ghost::State::Frightened)
                speed = 0.55f * Constants::PACMAN_SPEED;
            else if (ghost->getUserData().getValue<bool>("is_in_tunnel"))
                speed = 0.45f * Constants::PACMAN_SPEED;
            else
                speed = 0.85f * Constants::PACMAN_SPEED;
        } else {
            if (g->getState() == Ghost::State::Frightened) // Stops triggering from level 19 onwards
                speed = 0.60f * Constants::PACMAN_SPEED;
            else if (g->getUserData().getValue<bool>("is_in_tunnel"))
                speed = 0.50f * Constants::PACMAN_SPEED;
            else
                speed = 0.95f * Constants::PACMAN_SPEED;
        }

        ghost->getGridMover()->setSpeed(ime::Vector2f{speed, speed});
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::setMovementFreeze(bool freeze) {
        getGridMovers().findByTag("pacmanGridMover")->setMovementFreeze(freeze);

        getGridMovers().forEachInGroup("GhostMovers", [freeze](ime::GridMover* gridMover) {
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

        if (frightenedModeTimer_.isRunning())
            frightenedModeTimer_.setInterval(frightenedModeTimer_.getRemainingDuration() + duration);
        else {
            if (scatterModeTimer_.isRunning())
                scatterModeTimer_.pause();
            else if (chaseModeTimer_.isRunning())
                chaseModeTimer_.pause();
            else {
                assert(false && "Ghost entered frightened mode from an invalid state: valid states are scatter and chase");
            }

            frightenedModeTimer_.setInterval(duration);
            frightenedModeTimer_.onTimeout([this] {
                pointsMultiplier_ = 1;
                emit(GameEvent::FrightenedModeEnd);

                // A paused timer implies that the ghost was in the state
                // controlled by the timer before being frightened
                if (scatterModeTimer_.isPaused())
                    startGhostScatterMode();
                else if (chaseModeTimer_.isPaused())
                    startGhostChaseMode();
                else {
                    if (ime::utility::generateRandomNum(1, 100) >= 50)
                        startGhostChaseMode();
                    else
                        startGhostScatterMode();
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
                duration = ime::seconds(1.0f / getEngine().getWindow().getFrameRateLimit()); // one frame
        }

        // Transition to chase mode when timer expires
        scatterModeTimer_.onTimeout([this] {
            emit(GameEvent::ScatterModeEnd);
            startGhostChaseMode();
        });

        scatterModeTimer_.setInterval(duration);
        scatterModeTimer_.start();
        emit(GameEvent::ScatterModeBegin);
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::lockGhostInHouse(ime::GridObject* ghost) {
        if ((ghost->getTag() == "pinky" && currentLevel_ == 1) ||
            (ghost->getTag() == "inky" && currentLevel_ <= 2) ||
            (ghost->getTag() == "clyde" && currentLevel_ <= 3))
        {
            static_cast<Ghost*>(ghost)->lockInGhostHouse(true);
            numGhostsInHouse_ += 1;
        }
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
            chaseModeTimer_.onTimeout([] {});
        } else {
            chaseModeTimer_.setInterval(duration);

            chaseModeTimer_.onTimeout([this] {
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
            if (numGhostsInHouse_ == 3) { // Release pinky
                static_cast<Ghost*>(getGameObjects().getGroup("Ghost").findByTag("pinky"))->lockInGhostHouse(false);
                ghostHouseTimer_.setInterval(ime::seconds(Constants::INKY_HOUSE_ARREST_DURATION));
            } else if (numGhostsInHouse_ == 2) { // Release inky
                static_cast<Ghost*>(getGameObjects().getGroup("Ghost").findByTag("inky"))->lockInGhostHouse(false);
                ghostHouseTimer_.setInterval(ime::seconds(Constants::CLYDE_HOUSE_ARREST_DURATION));
            } else // Release clyde
                static_cast<Ghost*>(getGameObjects().getGroup("Ghost").findByTag("clyde"))->lockInGhostHouse(false);

            numGhostsInHouse_ -= 1;
        };

        ghostHouseTimer_.setInterval(ime::seconds(Constants::PINKY_HOUSE_ARREST_DURATION)); // Pinky is freed first
        ghostHouseTimer_.onTimeout(freeGhost);
        ghostHouseTimer_.setRepeatCount(2);
        ghostHouseTimer_.start();
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::stopTimers() {
        chaseModeTimer_.stop();
        frightenedModeTimer_.stop();
        scatterModeTimer_.stop();
        uneatenFruitTimer_.stop();
        ghostHouseTimer_.stop();
    }

    ///////////////////////////////////////////////////////////////
    void GameplayScene::flashGhosts() {
        ime::Time static flashAnimCutoffTime = ime::seconds(2);

        if (frightenedModeTimer_.isRunning()) {
            getGameObjects().forEachInGroup("Ghost", [this](ime::GameObject* ghostBase) {
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
        ObjectReferenceKeeper::clear();
        stopTimers();
        getWindow().removeEventListener(onWindowCloseId_);
    }

    ///////////////////////////////////////////////////////////////
    GameplayScene::~GameplayScene() {
        ObjectReferenceKeeper::clear();
    }

} // namespace pm