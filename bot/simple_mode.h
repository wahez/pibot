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

#pragma once

#include "hardware.h"
#include "mode.h"


namespace Modes {


    using namespace std::literals;


    struct Simple : Base
    {
        Bot::Hardware& hardware;
        Bot::Hardware::EventPoller::Subscription pollerSubscription;
        CamJam3::DistanceSensor::Subscription distanceSubscription;

        explicit Simple(Bot::Hardware& hw)
            : hardware(hw)
        {
            hardware.event_poller.set_interval(10ms);
            pollerSubscription = hardware.event_poller.subscribe([this](const Input::Event& event)
            {
                this->hardware.bot.move(event.direction, event.speed);
            });
            auto& distance = hardware.bot.get_distance_sensor();
            distance.set_interval(1s);
            distance.set_resolution(0.002 * SI::meters);
            distanceSubscription = distance.subscribe([this](auto&&, auto distance)
            {
                hardware.wiimote->rumble(distance < 0.20 * SI::meters);
            });
        }

        Type get_type() const override { return Type::simple; }

        ~Simple()
        {
            hardware.bot.move(0, 0);
            hardware.wiimote->rumble(false);
        }
    };


}
