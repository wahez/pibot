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


    Motor::Motor(Loop& loop, PinNumber forwardPin, PinNumber reversePin)
        : _dutyCycle(loop, 10ms, *this)
        , _forwardPin(forwardPin)
        , _reversePin(reversePin)
    {
        stop();
    }


    void Motor::move(float direction)
    {
        _direction = direction;
        if (_direction > 0)
        {
            _dutyCycle.set_duty_cycle(direction);
        }
        else if (_direction < 0)
        {
            _dutyCycle.set_duty_cycle(-direction);
        }
        else
        {
            _dutyCycle.set_duty_cycle(0);
        }
    }


    void Motor::up()
    {
        if (_direction > 0.01)
        {
            _forwardPin.set(true);
            _reversePin.set(false);
        }
        else if (_direction < -0.01)
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


    void Motor::down()
    {
        _forwardPin.set(false);
        _reversePin.set(false);
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


    float DistanceSensor::distance(float resolution, float max_distance)
    {
        using FloatSeconds = std::chrono::duration<float, std::chrono::seconds::period>;
        constexpr float speed_of_sound = 343.260;
        auto sleep_time = FloatSeconds(2 * resolution / speed_of_sound);
        auto timeout = FloatSeconds(2 * max_distance / speed_of_sound);
        auto wait_for = [&](auto&& pred)
        {
            auto start = std::chrono::high_resolution_clock::now();
            auto end = start + timeout;
            while (!pred() && start <= end)
            {
                std::this_thread::sleep_for(sleep_time);
                start = std::chrono::high_resolution_clock::now();
            }
            return start;
        };
        using namespace std::literals;
        _trigger.set(true);
        std::this_thread::sleep_for(10us);
        _trigger.set(false);
        auto start = wait_for([this]() { return  this->_echo.read(); });
        auto now   = wait_for([this]() { return !this->_echo.read(); });
        auto elapsed = FloatSeconds(now - start);
        auto distance = elapsed.count() * speed_of_sound / 2;
        return distance;
    }


    Bot::Bot(Loop& loop)
        : _left(loop, 8, 7)
        , _right(loop, 9, 10)
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
