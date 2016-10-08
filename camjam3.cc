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


    struct StateMachine : public AlarmHandler
    {
        using Timespan = std::chrono::microseconds;
        static constexpr const Timespan _trigger_length = 10us;
        static constexpr const Timespan _timeout = 100'000us;
        static constexpr const Timespan _resolution = 1us;
        static constexpr const Timespan _interval = 100us;
        DistanceSensor& _sensor;
        enum class Action { START_TRIGGER, END_TRIGGER, WAIT_FOR_HIGH, WAIT_FOR_LOW };
        Action _next_action = Action::START_TRIGGER;
        std::chrono::high_resolution_clock::time_point _start;

        StateMachine(DistanceSensor& sensor) : _sensor(sensor) {}

        using Result = std::pair<Timespan, Action>;

        Result start_trigger()
        {
            _sensor._trigger.set(true);
            return {_trigger_length, Action::END_TRIGGER};
        }

        Result wait_for_high()
        {
            auto now = std::chrono::high_resolution_clock::now();
            if (now > _start + _timeout)
            {
                return {_interval, Action::START_TRIGGER};
            }
            else if (!_sensor._echo.read())
            {
                return {_resolution, Action::WAIT_FOR_HIGH};
            }
            else
            {
                _start = now;
                return {_resolution, Action::WAIT_FOR_LOW};
            }
        }

        Result end_trigger()
        {
            _sensor._trigger.set(false);
            _start = std::chrono::high_resolution_clock::now();
            return wait_for_high();
        }

        Result wait_for_low()
        {
            auto now = std::chrono::high_resolution_clock::now();
            if (now > _start + _timeout)
            {
                return {_interval, Action::START_TRIGGER};
            }
            else if (_sensor._echo.read())
            {
                return {_resolution, Action::WAIT_FOR_LOW};
            }

            using FloatSeconds = std::chrono::duration<float, std::chrono::seconds::period>;
            auto elapsed = FloatSeconds(now - _start);
            auto distance = elapsed.count() * 343.260 / 2;
            _sensor._handler.distance(_sensor, distance);
            return {_interval, Action::START_TRIGGER};
        }

        Result exec_state()
        {
            using namespace std::literals;
            switch (_next_action)
            {
            case Action::END_TRIGGER:
                return end_trigger();
                // fallthrough
            case Action::WAIT_FOR_HIGH:
                return wait_for_high();
            case Action::WAIT_FOR_LOW:
                return wait_for_low();
            case Action::START_TRIGGER:
                return start_trigger();
            };
            return start_trigger();
        }

        void fire() override
        {
            Result delay_action = exec_state();
            _next_action = delay_action.second;
            _sensor._loop.set_alarm(delay_action.first, *this);
        }

    };
    constexpr const StateMachine::Timespan StateMachine::_trigger_length;
    constexpr const StateMachine::Timespan StateMachine::_timeout;
    constexpr const StateMachine::Timespan StateMachine::_resolution;
    constexpr const StateMachine::Timespan StateMachine::_interval;


    DistanceSensor::DistanceSensor(PinNumber trigger, PinNumber echo, Loop& loop, DistanceHandler& handler)
        : _loop(loop)
        , _handler(handler)
        , _trigger(trigger)
        , _echo(echo)
        , _state(new StateMachine(*this))
    {
        _trigger.set(false);
        _loop.set_alarm(0s, *_state);
    }


    DistanceSensor::~DistanceSensor() {}


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
