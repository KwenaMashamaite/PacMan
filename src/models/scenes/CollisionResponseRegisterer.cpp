#include "src/models/actors/Pellet.h"
#include "src/common/Constants.h"
#include "src/models/actors/PacMan.h"
#include "src/models/actors/Ghost.h"
#include "src/models/actors/Fruit.h"
#include "CollisionResponseRegisterer.h"
#include <cassert>

namespace pm {
    ///////////////////////////////////////////////////////////////
    CollisionResponseRegisterer::CollisionResponseRegisterer(GameplayScene &game) : game_{game}
    {}

    ///////////////////////////////////////////////////////////////
    void CollisionResponseRegisterer::registerCollisionWithFruit(ime::GameObject *gameObject) {
        gameObject->onCollision(std::bind(&CollisionResponseRegisterer::resolveFruitCollision, this, std::placeholders::_2, std::placeholders::_1));
    }

    ///////////////////////////////////////////////////////////////
    void CollisionResponseRegisterer::registerCollisionWithPellets(ime::GameObject *gameObject) {
        gameObject->onCollision(std::bind(&CollisionResponseRegisterer::resolvePelletCollision, this, std::placeholders::_2));
    }

    ///////////////////////////////////////////////////////////////
    void CollisionResponseRegisterer::registerCollisionWithGhost(ime::GameObject *gameObject) {
        gameObject->onCollision(std::bind(&CollisionResponseRegisterer::resolveGhostCollision, this, std::placeholders::_2, std::placeholders::_1));
    }

    ///////////////////////////////////////////////////////////////
    void CollisionResponseRegisterer::registerCollisionWithTeleportationSensor(ime::GameObject *gameObject) {
        gameObject->onCollision(std::bind(&CollisionResponseRegisterer::resolveTeleportationSensorCollision, this, std::placeholders::_2, std::placeholders::_1));
    }

    ///////////////////////////////////////////////////////////////
    void CollisionResponseRegisterer::registerCollisionWithSlowDownSensor(ime::GameObject *gameObject) {
        gameObject->onCollision([this](ime::GameObject* target, ime::GameObject* sensor) {
            if (sensor->getClassName() != "Sensor")
                return;

            bool isLeavingTunnel = sensor->getTag() == "tunnelExitSensor";
            target->getCollisionExcludeList().add(isLeavingTunnel ? "tunnelExitSensor" : "tunnelEntrySensor");
            target->getCollisionExcludeList().remove(isLeavingTunnel ? "tunnelEntrySensor" : "tunnelExitSensor");
            target->getUserData().setValue("is_in_tunnel", !isLeavingTunnel);
            game_.updateGhostSpeed(target);
        });
    }

    ///////////////////////////////////////////////////////////////
    void CollisionResponseRegisterer::resolveFruitCollision(ime::GameObject* fruit, ime::GameObject* otherGameObject) {
        if (fruit->getClassName() != "Fruit")
            return;

        game_.uneatenFruitTimer_.stop();

        if (fruit->getTag() == "cherry")
            game_.updateScore(Constants::Points::CHERRY);
        else if (fruit->getTag() == "strawberry")
            game_.updateScore(Constants::Points::STRAWBERRY);
        else if (fruit->getTag() == "peach")
            game_.updateScore(Constants::Points::PEACH);
        else if (fruit->getTag() == "apple")
            game_.updateScore(Constants::Points::APPLE);
        else if (fruit->getTag() == "melon")
            game_.updateScore(Constants::Points::MELON);
        else if (fruit->getTag() == "galaxian")
            game_.updateScore(Constants::Points::GALAXIAN);
        else if (fruit->getTag() == "bell")
            game_.updateScore(Constants::Points::BELL);
        else if (fruit->getTag() == "key")
            game_.updateScore(Constants::Points::KEY);

        replaceFruitWithScore(fruit);
        game_.audio().play(ime::audio::Type::Sfx, "fruitEaten.wav");

        // Indefinitely suspend collisions with the fruit as it is destroyed after a delay
        // to prevent subsequent collisions with an eaten fruit
        otherGameObject->getCollisionExcludeList().add("fruits");
        game_.timer().setTimeout(ime::seconds(Constants::EATEN_FRUIT_DESTRUCTION_DELAY), [otherGameObject] {
            otherGameObject->getCollisionExcludeList().remove("fruits");
        });
    }

    ///////////////////////////////////////////////////////////////
    void CollisionResponseRegisterer::resolvePelletCollision(ime::GameObject* pellet) {
        if (pellet->getClassName() != "Pellet")
            return;

        game_.eatenPelletsCount_ += 1;
        pellet->setActive(false);

        if (static_cast<Pellet*>(pellet)->getPelletType() == Pellet::Type::Energizer) {
            game_.updateScore(Constants::Points::ENERGIZER);
            game_.startGhostFrightenedMode();

            game_.audio().play(ime::audio::Type::Sfx, "powerPelletEaten.wav");
        } else {
            game_.updateScore(Constants::Points::DOT);
            game_.audio().play(ime::audio::Type::Sfx, "WakkaWakka.wav");
        }

        if (game_.eatenPelletsCount_ == Constants::FIRST_FRUIT_APPEARANCE_PELLET_COUNT ||
            game_.eatenPelletsCount_ == Constants::SECOND_FRUIT_APPEARANCE_PELLET_COUNT)
        {
            spawnFruit();
        }
    }

    ///////////////////////////////////////////////////////////////
    void CollisionResponseRegisterer::resolveGhostCollision(ime::GameObject *ghost, ime::GameObject *pacman) {
        if (ghost->getClassName() != "Ghost" )
            return;

        if (pacman->getClassName() != "PacMan")
            return;

        auto pacmanState = static_cast<PacMan*>(pacman)->getState();
        auto ghostState = static_cast<Ghost*>(ghost)->getState();

        // Prevent pacman from being killed while dying - Happens when at
        // least two ghosts enters pacmans tile at the same time
        if (pacmanState == PacMan::State::Dying)
            return;

        if (ghostState == Ghost::State::Frightened) {
            game_.updateScore(Constants::Points::GHOST * game_.pointsMultiplier_);
            game_.setMovementFreeze(true);

            replaceGhostWithScore(ghost, pacman);
            game_.updatePointsMultiplier();

            game_.frightenedModeTimer_.pause();
            game_.uneatenFruitTimer_.pause();
            game_.timer().setTimeout(ime::seconds(Constants::ACTOR_FREEZE_DURATION), [this, ghost] {
                game_.setMovementFreeze(false);
                game_.frightenedModeTimer_.start();

                if (game_.uneatenFruitTimer_.getStatus() == ime::Timer::Status::Paused)
                    game_.uneatenFruitTimer_.start();

                static_cast<Ghost*>(ghost)->handleEvent(GameEvent::GhostEaten, {});
            });

            game_.audio().play(ime::audio::Type::Sfx, "ghostEaten.wav");
        } else if ((pacmanState == PacMan::State::Idle || pacmanState == PacMan::State::Moving) &&
                   (ghostState != Ghost::State::Eaten))
        {
            static_cast<PacMan*>(pacman)->setState(PacMan::State::Dying);

            game_.onPrePacmanDeathAnim();

            ime::Animator& pacmanAnimator = pacman->getSprite().getAnimator();
            pacmanAnimator.startAnimation("dying");
            auto deathAnimDuration = pacmanAnimator.getActiveAnimation()->getDuration();

            pacmanAnimator.on(ime::Animator::Event::AnimationStart, "dying", [=] {
                game_.gameObjects().forEachInGroup("Ghost", [](ime::GameObject* gameObject) {
                    gameObject->getSprite().setVisible(false);
                });

                game_.timer().setTimeout(deathAnimDuration + ime::milliseconds(400), [this] {
                    game_.onPostPacmanDeathAnim();
                });

                game_.audio().play(ime::audio::Type::Sfx, "pacmanDying.wav");
            }, true);
        }
    }

    ///////////////////////////////////////////////////////////////
    void CollisionResponseRegisterer::resolveTeleportationSensorCollision(ime::GameObject* sensor, ime::GameObject* objectOnSensor) {
        if (sensor->getClassName() == "Sensor" && sensor->getTag() == "teleportationSensor") {
            assert(objectOnSensor->getGridMover() && "Teleportation sensor trigger resolution cannot occur without a grid mover");

            ime::GridMover* gridMover = objectOnSensor->getGridMover();
            ime::TileMap& grid = gridMover->getGrid();
            const ime::Tile& currentTile = grid.getTileOccupiedByChild(objectOnSensor);
            grid.removeChild(objectOnSensor);

            if (currentTile.getIndex().colm == 0) { // Triggered the left-hand side sensor
                grid.addChild(objectOnSensor,ime::Index{currentTile.getIndex().row, static_cast<int>(grid.getSizeInTiles().x - 1)});
            } else
                grid.addChild(objectOnSensor, {currentTile.getIndex().row, 0});

            gridMover->resetTargetTile();
            gridMover->requestDirectionChange(gridMover->getDirection());
        }
    }

    ///////////////////////////////////////////////////////////////
    void CollisionResponseRegisterer::replaceFruitWithScore(ime::GameObject* fruit) {
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
        game_.timer().setTimeout(ime::seconds(Constants::EATEN_FRUIT_DESTRUCTION_DELAY), [this, id = fruit->getObjectId()] {
            if (auto* fruit = game_.gameObjects().findById(id))
                fruit->setActive(false);
        });
    }

    ///////////////////////////////////////////////////////////////
    void CollisionResponseRegisterer::replaceGhostWithScore(ime::GameObject* ghost, ime::GameObject* otherGameObject) {
        // Freeze animations to prevent texture changes while score texture is displayed
        otherGameObject->getSprite().getAnimator().setTimescale(0.0f);
        ghost->getSprite().getAnimator().setTimescale(0.0f);

        otherGameObject->getSprite().setVisible(false);
        ghost->getSprite().setTexture("spritesheet.png");

        if (game_.pointsMultiplier_ == 1)
            ghost->getSprite().setTextureRect(ime::UIntRect{137, 116, 16, 16}); // 200
        else if (game_.pointsMultiplier_ == 2)
            ghost->getSprite().setTextureRect(ime::UIntRect{154, 116, 16, 16}); // 400
        else if (game_.pointsMultiplier_ == 4)
            ghost->getSprite().setTextureRect(ime::UIntRect{171, 116, 16, 16}); // 800
        else
            ghost->getSprite().setTextureRect(ime::UIntRect{188, 116, 16, 16}); // 1600

        game_.timer().setTimeout(ime::seconds(Constants::ACTOR_FREEZE_DURATION), [otherGameObject, ghost] {
            otherGameObject->getSprite().getAnimator().setTimescale(1.0f);
            ghost->getSprite().getAnimator().setTimescale(1.0f);

            otherGameObject->getSprite().setVisible(true);
        });
    }

    ///////////////////////////////////////////////////////////////
    void CollisionResponseRegisterer::spawnFruit() {
        Fruit::Type fruitType;
        if (game_.currentLevel_ == 1)
            fruitType = Fruit::Type::Cherry;
        else if (game_.currentLevel_ == 2)
            fruitType = Fruit::Type::Strawberry;
        else if (game_.currentLevel_ == 3 || game_.currentLevel_ == 4)
            fruitType = Fruit::Type::Peach;
        else if (game_.currentLevel_ == 5 || game_.currentLevel_ == 6)
            fruitType = Fruit::Type::Apple;
        else if (game_.currentLevel_ == 7 || game_.currentLevel_ == 8)
            fruitType = Fruit::Type::Melon;
        else if (game_.currentLevel_ == 9 || game_.currentLevel_ == 10)
            fruitType = Fruit::Type::Galaxian;
        else if (game_.currentLevel_ == 11 || game_.currentLevel_ == 12)
            fruitType = Fruit::Type::Bell;
        else
            fruitType = Fruit::Type::Key;

        auto fruit = std::make_unique<Fruit>(game_, fruitType);

        // Destroy fruit if left uneaten for some time
        game_.uneatenFruitTimer_.setTimeoutCallback([fruitPtr = fruit.get()] {
            fruitPtr->setActive(false);
        });

        game_.uneatenFruitTimer_.setInterval(ime::seconds(Constants::UNEATEN_FRUIT_DESTRUCTION_DELAY));
        game_.uneatenFruitTimer_.start();

        game_.grid_->addActor(std::move(fruit), Constants::FRUIT_SPAWN_POSITION);
    }
}