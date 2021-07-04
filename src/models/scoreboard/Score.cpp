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

#include "Score.h"

namespace pm {
    ///////////////////////////////////////////////////////////////
    Score::Score() :
        value_{0},
        level_{0}
    {}

    ///////////////////////////////////////////////////////////////
    void Score::setValue(int value) {
        value_ = value;
    }

    ///////////////////////////////////////////////////////////////
    int Score::getValue() const {
        return value_;
    }

    ///////////////////////////////////////////////////////////////
    void Score::setLevel(unsigned int level) {
        level_ = level;
    }

    ///////////////////////////////////////////////////////////////
    unsigned int Score::getLevel() const {
        return level_;
    }

    ///////////////////////////////////////////////////////////////
    void Score::setOwner(const std::string &name) {
        owner_ = name;
    }

    ///////////////////////////////////////////////////////////////
    const std::string &Score::getOwner() const {
        return owner_;
    }

    ///////////////////////////////////////////////////////////////
    void Score::adjustValue(int offset) {
        setValue(getValue() + offset);
    }

    ///////////////////////////////////////////////////////////////
    bool Score::operator==(const Score &rhs) const {
        return value_ == rhs.value_ && owner_ == rhs.owner_;
    }

    ///////////////////////////////////////////////////////////////
    bool Score::operator!=(const Score &rhs) const{
        return !(*this == rhs);
    }

    ///////////////////////////////////////////////////////////////
    bool operator<(const Score &lhs, const Score &rhs) {
        return lhs.getValue() < rhs.getValue();
    }

    ///////////////////////////////////////////////////////////////
    bool operator>(const Score &lhs, const Score &rhs) {
        return lhs.getValue() > rhs.getValue();
    }

} // namespace pm
