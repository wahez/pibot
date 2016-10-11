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
#include "loop.h"

#include <memory>


namespace Pi
{


    class Motor : public DutyCycleHandler
    {
    public:
        Motor(Loop&, PinNumber forwardPin, PinNumber reversePin);

        void stop()    { move(0); }
        void forward() { move(1); }
        void reverse() { move(-1); }
        void move(float direction);

    private:
        DutyCycle _dutyCycle;
        OutputPin _forwardPin;
        OutputPin _reversePin;
        float _direction = 0;

        void up() override;
        void down() override;
    };


    class LineSensor
    {
    public:
        LineSensor(PinNumber);

        bool IsOnLine();

    private:
        InputPin _pin;
    };


    struct DistanceHandler
    {
        virtual void distance(class DistanceSensor&, float distance) = 0;
    };


    class DistanceSensor
    {
    public:
        DistanceSensor(PinNumber trigger, PinNumber echo, Loop&, DistanceHandler&);
        ~DistanceSensor();

        void set_frequency(float Hz)
        {
            auto seconds = std::chrono::duration<float>(1/Hz);
            _interval = std::chrono::duration_cast<Duration>(seconds);
        }

        void set_resolution(float meters)
        {
            auto resolution = std::chrono::duration<float>(2 * meters / SpeedOfSound);
            _resolution = std::chrono::duration_cast<Duration>(resolution);
        }

    private:
        static const constexpr float SpeedOfSound = 343.260;
        using Duration = std::chrono::high_resolution_clock::duration;

        friend class StateMachine;
        Loop& _loop;
        DistanceHandler& _handler;
        OutputPin _trigger;
        InputPin _echo;
        Duration _interval;
        Duration _resolution;

        std::unique_ptr<class StateMachine> _state;
    };


    class Bot
    {
    public:
        Bot(Loop&);

        void move(float direction, float speed);

    private:
        Motor _left;
        Motor _right;
        LineSensor _lineSensor;

        void update(bool);
    };


}
