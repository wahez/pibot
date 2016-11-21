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

#include "gpio.h"
#include <loop/loop.h>
#include <loop/subscription_list.h>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/si/length.hpp>
#include <memory>


namespace SI = boost::units::si;


namespace CamJam3
{


    template<typename T>
    using Q = boost::units::quantity<T>;

    using Distance = Q<SI::length>;


    class DistanceSensor : public Loop::SubscriptionList<DistanceSensor&, Distance>
    {
    public:
        using Duration = std::chrono::high_resolution_clock::duration;

        DistanceSensor(Pi::PinNumber trigger, Pi::PinNumber echo, Loop::Loop&);
        ~DistanceSensor();

        void set_interval(Duration interval) { _interval = interval; }
        void set_resolution(Distance);

    private:
        friend struct StateMachine;
        Loop::Loop& _loop;
        Pi::OutputPin _trigger;
        Pi::InputPin _echo;
        Duration _interval;
        Duration _resolution;
        std::unique_ptr<struct StateMachine> _state;
    };


}
