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

#ifndef PACMAN_GAMEPLAYSCENE_H
#define PACMAN_GAMEPLAYSCENE_H

#include "Grid2D/Grid.h"
#include "Common/GameEvents.h"
#include "Views/GameplaySceneView.h"
#include <IME/core/scene/Scene.h>

namespace pm {
    class CollisionResponseRegisterer;

    /**
     * @brief Defines the playing state of the game
     */
    class GameplayScene : public ime::Scene {
    public:
        /**
         * @brief Constructor
         */
        GameplayScene();

        /**
         * @brief Enter the scene
         *
         * This function is called by the game engine when the scene
         * is entered for the first time
         */
        void onEnter() override;

        /**
         * @brief Pause scene
         *
         * This function is called by the game engine when the game transitions
         * from this scene (without destroying it) to another scene
         */
        void onPause() override;

        /**
         * @brief Resume scene
         *
         * This function is called by the game engine when the game returns
         * to this scene from another one
         */
        void onResume() override;

        /**
         * @brief Update the scene
         * @param deltaTime Time passed since last update
         *
         * This function is called by the game engine when it is time to
         * update. Note that @a deltaTime is frame rate dependent
         */
        void onUpdate(ime::Time deltaTime) override;

        /**
         * @brief Initialize third party engine events
         */
        void onFrameEnd() override;

        /**
         * @brief Pre-scene destruction handler
         *
         * This function is called by the game engine before the scene is
         * destroyed
         */
        void onExit() override;

        /**
         * @brief Destructor
         */
        ~GameplayScene();

    private:
        /**
         * @brief Initialize the gui
         */
        void initGui();

        /**
         * @brief Create the gameplay grid
         */
        void createGrid();

        /**
         * @brief Add GameObjects to the grid
         */
        void createActors();

        /**
         * @brief Create PathFinders controllers for pacman and the ghosts
         */
        void createGridMovers();

        /**
         * @brief Initialize game events
         */
        void intiGameEvents();

        /**
         * @brief Initialize movable GameObjects collision responses
         */
        void initCollisionResponses();

        /**
         * @brief Start the level start countdown
         */
        void startCountDown();

        /**
         * @brief Transition game to pause menu
         */
        void pauseGame();

        /**
         * @brief Flash the gameplay grid and advance to the next level
         */
        void completeLevel();

        /**
         * @brief Restarts the game level without restoring eaten pellets
         */
        void resetLevel();

        /**
         * @brief Reset pacman and the ghosts to their respective spawn tiles
         */
        void resetActors();

        /**
         * @brief End the gameplay
         *
         * This function is called when the player loses all lives
         */
        void endGameplay();

        /**
         * @brief Update the game Grid2D before playing death animation
         */
        void onPrePacmanDeathAnim();

        /**
         * @brief Update the game Grid2D after death animation finished playing
         */
        void onPostPacmanDeathAnim();

        /**
         * @brief Update cache and view score values
         * @param points The points to update the score by
         */
        void updateScore(int points);

        /**
         * @brief Emit a game event
         * @param event The event to be emitted
         */
        void emit(GameEvent event);

        /**
         * @brief Set the speed of pacman
         * @param pacmam PacMan
         */
        void updatePacmanSpeed(ime::GridObject* pacmam) const;

        /**
         * @brief Set the speed of a ghost
         * @param ghost The Ghost
         */
        void updateGhostSpeed(ime::GridObject* ghost) const;

        /**
         * @brief Start the ghost frightened mode timer
         *
         * Note that if the timer is paused, it will be resumed instead
         * of being reset
         */
        void startGhostFrightenedMode();

        /**
         * @brief Start the ghost scatter chase timer
         *
         * Note that if the timer is paused, it will simply be resumed instead
         * of being reset
         */
        void startGhostScatterMode();

        /**
         * @brief Start the ghost chase mode timer
         *
         * Note that if the timer is paused, it will simply be resumed instead
         * of being reset
         */
        void startGhostChaseMode();

        /**
         * @brief Attempt to lock a ghost in the ghost house
         * @param ghost The ghost to be locked inside
         */
        void lockGhostInHouse(ime::GridObject* ghost);

        /**
         * @brief Start the timer that controls when a ghost leaves the ghost
         *        house
         *
         * Recall that blinky starts outside pf the ghost house. The remaining
         * ghosts are released in the following order: pinky, inky then clyde.
         */
        void startGhostHouseTimer();

        /**
         * @brief Stop all active timers
         */
        void stopTimers();

        /**
         * @brief Make ghosts flash
         *
         * Note that a ghost only flashes when in pm::FrightenedState. The
         * flash is a visual cue to the player that the energizer is about
         * to expire
         */
        void flashGhosts();

        /**
         * @brief Freeze or unfreeze the PathFinders of pacman and the ghosts
         * @param freeze True to freeze or false to unfreeze the PathFinders
         */
        void setMovementFreeze(bool freeze);

        /**
         * @brief Update the ghost point multiplier
         *
         * The multiplier increases by a factor of 2 every time the player
         * eats a ghost. However, since the player cannot eat more than four
         * ghosts in a single power mode session, the multiplier is capped to
         * 8. After the player eats the 4th ghost, the multiplier resets
         * to 1 (default). It also resets to 1 when power mode expires
         */
        void updatePointsMultiplier();

    private:
        int currentLevel_;                  //!< Current game level
        int pointsMultiplier_;              //!< Ghost points multiplier when player eats ghosts in succession (in one power mode session)
        int eatenPelletsCount_;             //!< Keeps track of the number of pellets pacman has eaten
        static int extraLivesGiven_;        //!< Keeps track of the number of extra lives the player has been awarded with
        GameplaySceneView view_;            //!< Scene view without the gameplay grid
        std::unique_ptr<Grid> grid_;        //!< Gameplay grid view
        ime::Timer frightenedModeTimer_;    //!< Controls the duration of the ghosts frightened state
        ime::Timer scatterModeTimer_;       //!< Controls the duration of the ghosts scatter state
        ime::Timer chaseModeTimer_;         //!< Controls the duration of the ghosts chase state
        ime::Timer ghostHouseTimer_;        //!< Controls when a ghost should leave the ghost house
        ime::Timer uneatenFruitTimer_;      //!< Controls how long an uneaten fruit remains on the screen before being destroyed
        unsigned int chaseModeWaveLevel_;   //!< Stores the current ghost chase wave level (up to 5)
        unsigned int scatterModeWaveLevel_; //!< Stores the current scatter mode level (up to 4)
        unsigned int numGhostsInHouse_;     //!< The number of ghosts currently in the ghost house
        int onWindowCloseId_;               //!< The id number of the 'onClose' event handler
        static inline bool isBoot_ = true;  //!< A flag indicating whether the game just booted or not (is set to false after the first countdown)
        std::unique_ptr<CollisionResponseRegisterer> collisionResponseRegisterer_;

        friend class CollisionResponseRegisterer;
    };
}

#endif
