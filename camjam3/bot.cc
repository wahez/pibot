/*
    Copyright 2016 Walther Zwart

    This file is part of pibot++.

    pibot++ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    pibot++ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with pibot++. If not, see <http://www.gnu.org/licenses/>.
*/

#include "bot.h"
#include <cmath>


namespace CamJam3
{


    using namespace std::literals;


    namespace {

        template<size_t N>
        auto interpolate(const float(&container)[N], float x)
        {
            x -= std::floor(x);
            x *= N-1;
            size_t index = std::floor(x);
            auto fraction = x - index;
            return
                container[index] * (1-fraction) +
                container[index+1] * fraction;
        }

    }


    Bot::Bot(Loop::Loop& loop)
        : _left(loop, 10, 9)
        , _right(loop, 7, 8)
        , _line_sensor(25)
        , _distance_sensor(17, 18, loop)
    {}


    void Bot::move(float direction, float speed)
    {
        if (speed < 0.1)
        {
            _left.move(0);
            _right.move(0);
        }
        else
        {
            constexpr const float directionToMotorSpeed[] = {1, 0, -1, -1, -1, 0, 1, 1, 1};
            float rm = interpolate(directionToMotorSpeed, direction / (2*M_PI));
            float lm = interpolate(directionToMotorSpeed, 1-(direction / (2*M_PI)));
            _left.move(lm*speed);
            _right.move(rm*speed);
        }
    }


}
