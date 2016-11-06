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
#include <loop/duty_cycle.h>
#include <loop/subscription_list.h>

#include <memory>


namespace Pi
{


    class Motor
    {
    public:
        Motor(Loop::Loop&, PinNumber forwardPin, PinNumber reversePin);

        void stop()    { move(0); }
        void forward() { move(1); }
        void reverse() { move(-1); }
        void move(float direction);

        void on();
        void off();

    private:
        Loop::DutyCycle _dutyCycle;
        OutputPin _forwardPin;
        OutputPin _reversePin;
        float _direction = 0;
    };


    class LineSensor
    {
    public:
        LineSensor(PinNumber);

        bool IsOnLine();

    private:
        InputPin _pin;
    };


    class DistanceSensor : public Loop::SubscriptionList<DistanceSensor&, float>
    {
    public:
        DistanceSensor(PinNumber trigger, PinNumber echo, Loop::Loop&);
        ~DistanceSensor();

        void set_frequency(float Hz);
        void set_resolution(float meters);

    private:
        static const constexpr float SpeedOfSound = 343.260;
        using Duration = std::chrono::high_resolution_clock::duration;

        friend class StateMachine;
        Loop::Loop& _loop;
        OutputPin _trigger;
        InputPin _echo;
        Duration _interval;
        Duration _resolution;
        std::unique_ptr<class StateMachine> _state;
    };


    class Bot
    {
    public:
        Bot(Loop::Loop&);

        void move(float direction, float speed);

        DistanceSensor& get_distance_sensor() { return _distance_sensor; }

    private:
        Motor _left;
        Motor _right;
        LineSensor _line_sensor;
        DistanceSensor _distance_sensor;

        void update(bool);
    };


}
