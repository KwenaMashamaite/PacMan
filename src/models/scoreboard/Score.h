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

#ifndef PACMAN_SCORE_H
#define PACMAN_SCORE_H

#include <string>

namespace pm {
    /**
     * @brief Game score
     */
    class Score {
    public:
        /**
         * @brief Constructor
         */
        Score();

        /**
         * @brief Copy constructor
         */
        Score(const Score&) = default;

        /**
         * @brief Assignment operator
         */
        Score& operator=(const Score&) = default;

        /**
         * @brief Set the value of the score
         * @param value New value of the score
         *
         * This function will overwrite the previous the score value. To adjust
         * the score value by an offset.
         *
         * By default, the score is 0
         *
         * @see adjustScore
         */
        void setValue(int value);

        /**
         * @brief Get the value of the score
         * @return The value of the score
         */
        int getValue() const;

        /**
         * @brief Set the level the score was obtained on
         * @param level The level the score was obtained on
         *
         * By default the level is 0
         */
        void setLevel(unsigned int level);

        /**
         * @brief Get the level the score was obtained on
         * @return The level the score was obtained on
         */
        unsigned int getLevel() const;

        /**
         * @brief Set the name of the player the score belongs to
         * @param name The name of the player
         *
         * By default, this function returns an empty string
         */
        void setOwner(const std::string& name);

        /**
         * @brief Get the name of the player the score belongs to
         * @return The name of the player the score belongs to
         */
        const std::string& getOwner() const;

        /**
         * @brief Increase or decrease the score value
         * @param offset The value to increase/decrease the score value by
         *
         * A negative offset decreases the score value while a positive offset
         * increases offset the score
         *
         * @see setScore
         */
        void adjustValue(int offset);

        /**
         * @brief Check if a score is the same as this score
         * @param rhs Score to check against this score
         * @return True if the scores are the same, otherwise false
         *
         * Two score objects are the same if they have the same value and
         * the same owner
         */
        bool operator==(const Score& rhs) const;

        /**
         * @brief Check if a score is not the same as this score
         * @param rhs Score to check against this score
         * @return True if the scores are not the same, otherwise false
         *
         * Two score objects are not the same if either the value or
         * the owner don't match
         */
        bool operator!=(const Score& rhs) const;

    private:
        int value_;          //!< Points obtained by player
        unsigned int level_; //!< The highest level reached by player
        std::string owner_;  //!< Name of the player the score belongs to
    };

    /**
     * @relates Score
     * @brief Check if a score is less than another scores value
     * @param lhs Left operand
     * @param rhs Right operand
     * @return True if @a lhs is less than @a rhs
     *
     * This operator performs comparison using score values
     */
    extern bool operator<(const Score& lhs, const Score& rhs);

    /**
     * @relates Score
     * @brief Check if a score is greater than another score
     * @param lhs Left operand
     * @param rhs Right operand
     * @return True if @a lhs is greater than @a rhs
     *
     * This operator performs comparison using score values
     */
    extern bool operator>(const Score& lhs, const Score& rhs);
}

#endif
