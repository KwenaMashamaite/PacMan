#ifndef PACMAN_COLLISIONRESPONSEREGISTRE_H
#define PACMAN_COLLISIONRESPONSEREGISTRE_H

#include <IME/core/game_object/GameObject.h>
#include <src/models/scenes/GameplayScene.h>

namespace pm {
    class CollisionResponseRegisterer {
    public:
        CollisionResponseRegisterer(GameplayScene& game);
        void registerPacmanHandlers(ime::GameObject& pacman);
        void registerGhostHandlers(ime::GameObject& ghost);

    private:
        GameplayScene& game_;
    };
}

#endif
