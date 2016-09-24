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
#include <cmath>


namespace Pi
{


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


    Motor::Motor(PinNumber forwardPin, PinNumber reversePin)
        : _forwardPin(forwardPin)
        , _reversePin(reversePin)
    {
        stop();
    }


    void Motor::move(float direction)
    {
        if (direction > 0.5)
        {
            _forwardPin.set(true);
            _reversePin.set(false);
        }
        else if (direction < -0.5)
        {
            _forwardPin.set(false);
            _reversePin.set(true);
        }
        else
        {
            _forwardPin.set(false);
            _reversePin.set(false);
        }
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


    float DistanceSensor::distance()
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
        using FloatSeconds = std::chrono::duration<float, std::chrono::seconds::period>;
        auto elapsed = FloatSeconds(now - start);
        auto distance = elapsed.count() * 343.260 / 2;
        return distance;
    }


    Bot::Bot()
        : _left(8, 7)
        , _right(9, 10)
        , _lineSensor(25)
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
