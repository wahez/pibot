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


namespace Pi
{


    class Motor
    {
    public:
        Motor(PinNumber forwardPin, PinNumber reversePin);

        void stop()    { move(0); }
        void forward() { move(1); }
        void reverse() { move(-1); }
        void move(float direction);

    private:
        OutputPin _forwardPin;
        OutputPin _reversePin;
    };


    class LineSensor
    {
    public:
        LineSensor(PinNumber);

        bool IsOnLine();

    private:
        InputPin _pin;
    };


    class DistanceSensor
    {
    public:
        DistanceSensor(PinNumber trigger, PinNumber echo);

        float distance();

    private:
        OutputPin _trigger;
        InputPin _echo;
    };


    class Bot
    {
    public:
        Bot();

        void move(float left, float right);

    private:
        Motor _left;
        Motor _right;
        LineSensor _lineSensor;
    };


}
