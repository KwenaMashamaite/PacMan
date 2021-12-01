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

#include "Scoreboard.h"
#include <algorithm>
#include <fstream>

namespace pm {
    ///////////////////////////////////////////////////////////////
    Scoreboard::Scoreboard(const std::string &filename) :
        highScoresFile_(filename)
    {}

    ///////////////////////////////////////////////////////////////
    void Scoreboard::load() {
        std::ifstream infile(highScoresFile_, std::ios::in | std::ios::binary);

        if (infile.is_open()) {
            infile.seekg(0, std::ios::end);
            auto numRecords = infile.tellg() / sizeof(Score);
            infile.seekg(0, std::ios::beg);

            Score score;

            for (int i = 0; i < numRecords; i++) {
                infile.read(reinterpret_cast<char *>(&score), sizeof(Score));
                highScores_.push_back(score);
            }

            infile.close();
        } else
            loadDefaultScores();
    }

    ///////////////////////////////////////////////////////////////
    void Scoreboard::addScore(const Score &score) {
        highScores_.push_back(score);
        std::sort(std::begin(highScores_), std::end(highScores_),std::greater<>());
    }

    ///////////////////////////////////////////////////////////////
    const Score& Scoreboard::getTopScore() const {
        return highScores_.front();
    }

    ///////////////////////////////////////////////////////////////
    std::size_t Scoreboard::getSize() const {
        return highScores_.size();
    }

    ///////////////////////////////////////////////////////////////
    void Scoreboard::updateHighScoreFile() {
        std::ofstream outfile(highScoresFile_, std::ios::out | std::ios::binary);

        for (auto& highScore : highScores_) {
            outfile.write(reinterpret_cast<char*>(&highScore), sizeof(Score));
        }

        outfile.close();
    }

    ///////////////////////////////////////////////////////////////
    void Scoreboard::forEachScore(std::function<void(const Score&)> callback) {
        for (const auto& score : highScores_)
            callback(score);
    }

    ///////////////////////////////////////////////////////////////
    void Scoreboard::loadDefaultScores() {
        auto defaultScores = std::vector<Score>{
            Score{"Brandon Jnr", 150000, 17},
            Score{"Kyle Jenkins", 125000, 15},
            Score{"Megan Bever", 120000, 13},
            Score{"Paul Martinez", 85000, 12},
            Score{"Sydney Willis", 55000, 10},
            Score{"Kate Watts", 35000, 8},
            Score{"Micheal Craig", 22700, 7},
            Score{"Jake Martins", 150000, 4},
            Score{"Tommy Shelby", 9800, 2},
            Score{"John Merc", 4500, 1}
        };

        highScores_.swap(defaultScores);
        updateHighScoreFile();
    }

} // namespace pm
