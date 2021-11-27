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
#include "Common/Constants.h"

namespace pm {
#pragma pack(push)
#pragma pack(1)

    /**
     * @brief Game score
     */
    struct Score {
        char owner_[Constants::MAX_NAME_CHARACTERS]; //!< Name of the player the score belongs to
        int value_;                                  //!< Points obtained by player
        unsigned int level_;                         //!< The highest level reached by player
    };
#pragma pack(pop)

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
