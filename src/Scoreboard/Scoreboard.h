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

#ifndef PACMAN_SCOREBOARD_H
#define PACMAN_SCOREBOARD_H

#include "Score.h"
#include <vector>
#include <string>
#include <functional>

namespace pm {
    /**
     * @brief Loads and persists game top scores
     */
    class Scoreboard {
    public:
        /**
         * @brief Constructor
         * @param filename The name of the file that contains the high scores
         *
         * @a filename must be preceded by the path
         */
        explicit Scoreboard(const std::string &filename);

        /**
         * @brief Load high scores from the disk
         *
         * The high scores will be loaded from the file provided during
         * instantiation
         */
        void load();

        /**
         * @brief Add a score to the Scoreboard
         * @param score Score to be added
         *
         * The Scoreboard is updated after the score is added. In addition,
         * note that the Scoreboard sorts entries in descending order
         */
        void addScore(const Score &score);

        /**
         * @brief Get the highest score
         * @return Highest score
         */
        const Score& getTopScore() const;

        /**
         * @brief Get the number of top scores in the Scoreboard
         * @return The number of top scores in the Scoreboard
         */
        std::size_t getSize() const;

        /**
         * @brief Write scores to a file on the disk
         * @param name Name of the player the current score belongs to
         * @param level The level the player is on when score is saved
         *
         * This function will write the current top scores to the file
         * provided during instantiation. The file is updated only if
         * the current score is greater than the lowest high score from
         * the last file read
         */
        void updateHighScoreFile();

        /**
         * @brief Execute a function for each score in the Scoreboard
         * @param callback Function to be executed
         */
        void forEachScore(std::function<void(const Score&)> callback);

    private:
        /**
         * @brief Load default leaderboard
         */
        void loadDefaultScores();

    private:
        std::vector<Score> highScores_; //!< High scores read from dis
        std::string highScoresFile_;    //!< High scores file to be read/written
    };
}

#endif
