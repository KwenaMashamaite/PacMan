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
#include <IME/utility/DiskFileReader.h>
#include <algorithm>

namespace pm {
    ///////////////////////////////////////////////////////////////
    Scoreboard::Scoreboard(const std::string &filename) :
        highScoresFile_(filename)
    {}

    ///////////////////////////////////////////////////////////////
    void Scoreboard::load() {
        auto highScores = std::stringstream();
        ime::utility::DiskFileReader().readFileInto(highScoresFile_, highScores);
        auto line = std::string();
        while (std::getline(highScores, line)) {
            auto posOfSpaceBetweenNameAndScore = line.find_first_of(':');
            auto scoreAndLevel = line.substr(posOfSpaceBetweenNameAndScore + 1);
            auto posOfSpaceBetweenScoreAndLevel = scoreAndLevel.find_first_of(' ');
            auto score = Score();
            score.setOwner(line.substr(0, posOfSpaceBetweenNameAndScore));
            score.setValue(std::stoi(line.substr(posOfSpaceBetweenNameAndScore + 1, posOfSpaceBetweenScoreAndLevel)));
            score.setLevel(std::stoi(scoreAndLevel.substr(posOfSpaceBetweenScoreAndLevel + 1)));
            highScores_.push_back(score);
        }
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
        auto newHighscoreList = std::stringstream();
        newHighscoreList << highScores_.front().getOwner() + ":" + std::to_string(highScores_.front().getValue()) + " " + std::to_string(highScores_.front().getLevel());
        std::for_each(++highScores_.begin(), highScores_.end(),[&](auto& score) {
            newHighscoreList << "\n" + score.getOwner() + ":" + std::to_string(score.getValue()) + " " + std::to_string(score.getLevel());
        });

        ime::utility::DiskFileReader().writeToFile(newHighscoreList, highScoresFile_);
    }

    ///////////////////////////////////////////////////////////////
    void Scoreboard::forEachScore(std::function<void(const Score&)> callback) {
        for (const auto& score : highScores_)
            callback(score);
    }

} // namespace pm
