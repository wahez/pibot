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

#include "camjam3.h"

#include <thread>
#include <chrono>


namespace Pi
{


    Motor::Motor(PinNumber forwardPin, PinNumber reversePin)
        : _forwardPin(forwardPin)
        , _reversePin(reversePin)
    {
        stop();
    }


    void Motor::stop()
    {
        _forwardPin.set(false);
        _reversePin.set(false);
    }


    void Motor::forward()
    {
        _forwardPin.set(true);
        _reversePin.set(false);
    }


    void Motor::reverse()
    {
        _forwardPin.set(false);
        _reversePin.set(true);
    }


    LineSensor::LineSensor(PinNumber pin)
        : _pin(pin)
    {
    }


    bool LineSensor::IsOnLine()
    {
        return _pin.read();
    }


    DistanceSensor::DistanceSensor(PinNumber trigger, PinNumber echo)
        : _trigger(trigger)
        , _echo(echo)
    {
        _trigger.set(false);
    }


    double DistanceSensor::distance()
    {
        using namespace std::literals;
        _trigger.set(true);
        std::this_thread::sleep_for(10us);
        auto start = std::chrono::high_resolution_clock::now();
        _trigger.set(false);

        auto end = start + 40ms;
        auto now = start;
        while (!_echo.read())
        {
            now = std::chrono::high_resolution_clock::now();
            if (now >= end) return 0;
        }
        using FloatSeconds = std::chrono::duration<double, std::chrono::seconds::period>;
        auto elapsed = FloatSeconds(now - start);
        auto distance = elapsed.count() * 343.260 / 2;
        return distance;
    }


    Bot::Bot()
        : _left(7, 8)
        , _right(10, 9)
        , _lineSensor(25)
    {}


    void Bot::forward()
    {
        _left.forward();
        _right.forward();
    }


    void Bot::reverse()
    {
        _left.reverse();
        _right.reverse();
    }


    void Bot::stop()
    {
        _left.stop();
        _right.stop();
    }


    void Bot::forwardLeft()
    {
        _left.stop();
        _right.forward();
    }


    void Bot::reverseLeft()
    {
        _left.stop();
        _right.reverse();
    }


    void Bot::forwardRight()
    {
        _left.forward();
        _right.stop();
    }


    void Bot::reverseRight()
    {
        _left.reverse();
        _right.stop();
    }


    void Bot::rotateLeft()
    {
        _left.reverse();
        _right.forward();
    }


    void Bot::rotateRight()
    {
        _left.forward();
        _right.reverse();
    }


}
