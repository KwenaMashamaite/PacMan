#include <src/models/actors/Pellet.h>
#include <src/common/Constants.h>
#include <src/models/actors/PacMan.h>
#include <src/models/actors/Ghost.h>
#include "CollisionResponseRegisterer.h"

namespace pm {
    CollisionResponseRegisterer::CollisionResponseRegisterer(GameplayScene &game) : game_{game}{

    }

    void CollisionResponseRegisterer::registerPacmanHandlers(ime::GameObject &pacman) {
        auto onPelletCollision = [this](ime::GameObject* pelletBase) {
            game_.eatenPelletsCount_ += 1;

            auto pellet = static_cast<Pellet*>(pelletBase);
            pellet->setActive(false);
            if (pellet->getPelletType() == Pellet::Type::Energizer) {
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
                game_.spawnFruit();
            }
        };

        auto onFruitCollision = [this](ime::GameObject* fruit) {
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

            game_.replaceFruitWithScore(fruit);
            game_.audio().play(ime::audio::Type::Sfx, "fruitEaten.wav");
        };

        auto onGhostCollision = [this](ime::GameObject* pacman, ime::GameObject* ghost) {
            auto pacmanState = static_cast<PacMan*>(pacman)->getState();
            auto ghostState = static_cast<Ghost*>(ghost)->getState();

            // Prevent pacman from being killed while dying - Happens when at
            // least two ghosts enters pacmans tile at the same time
            if (pacmanState == PacMan::State::Dying)
                return;

            if (ghostState == Ghost::State::Frightened) {
                game_.updateScore(Constants::Points::GHOST * game_.pointsMultiplier_);
                game_.setMovementFreeze(true);

                game_.replaceGhostWithScore(pacman, ghost);
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
        };

        pacman.onCollision([=](ime::GameObject* pacman, ime::GameObject* other) {
            if (other->getClassName() == "Pellet")
                onPelletCollision(other);
            else if (other->getClassName() == "Fruit") {
                onFruitCollision(other);

                // The fruit is destroyed after a delay, prevent subsequent collisions in the mean-time
                pacman->getCollisionExcludeList().add("fruits");

                // Restore pacman-fruit collision
                game_.timer().setTimeout(ime::seconds(Constants::EATEN_FRUIT_DESTRUCTION_DELAY), [pacman] {
                    pacman->getCollisionExcludeList().remove("fruits");
                });
            }
            else if (other->getClassName() == "Ghost")
                onGhostCollision(pacman, other);
        });
    }

    void CollisionResponseRegisterer::registerGhostHandlers(ime::GameObject &ghost) {

    }
}