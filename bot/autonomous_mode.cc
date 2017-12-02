/*
    Copyright 2017 Walther Zwart

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

#include "autonomous_mode.h"


namespace Modes {


    using namespace std::literals;


    Autonomous::Autonomous(Bot::Hardware& hw)
        : hardware(hw)
    {
        // TODO: which button can we use to influence autonomous mode?
        constexpr auto interval = 100ms;
        auto& distance = hardware.bot.get_distance_sensor();
        distance.set_interval(interval);
        distance.set_resolution(0.002 * SI::meters);
        distanceSubscription = distance.subscribe([this](auto&&, auto distance)
        {
            // TODO turn if needed
            if (distance > 20*SI::meter)
            {
                auto speed = distance / SI::meter + 0.15;
                hardware.bot.move(0, speed);
            }
            else if (distance < 10*SI::meter)
            {
                hardware.bot.move(M_PI, 0.5);
            }
            else
            {
                hardware.bot.move(0, 0);
            }
        });
    }

    Autonomous::~Autonomous()
    {
        hardware.bot.move(0, 0);
    }


}
