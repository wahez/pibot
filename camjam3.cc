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
#include <boost/variant.hpp>
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
        : _dutyCycle(loop, 10ms, [this](bool up) { if (up) this->on(); else this->off(); })
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


    void Motor::on()
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


    void Motor::off()
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


    namespace {


        using TimePoint = std::chrono::high_resolution_clock::time_point;
        using Duration = std::chrono::high_resolution_clock::duration;
        auto Now() { return std::chrono::high_resolution_clock::now(); }
        struct Start {};
        struct Triggering {};
        struct WaitingForHigh { TimePoint started; };
        struct WaitingForLow  { TimePoint started; };
        using State = boost::variant<Start, Triggering, WaitingForHigh, WaitingForLow>;
        using Result = std::pair<Duration, State>;

        constexpr const Duration trigger_length = 10us;


    }


    struct StateMachine : public AlarmHandler, public boost::static_visitor<Result>
    {
        DistanceSensor& _sensor;
        State _state = Start{};

        StateMachine(DistanceSensor& sensor) : _sensor(sensor) {}

        Result operator()(Start)
        {
            _sensor._trigger.set(true);
            return {trigger_length, Triggering()};
        }
        Result operator()(Triggering)
        {
            _sensor._trigger.set(false);
            return {_sensor._resolution, WaitingForHigh{ Now() }};
        }
        Result operator()(const WaitingForHigh& state)
        {
            auto now = std::chrono::high_resolution_clock::now();
            if (now > state.started + _sensor._interval)
            {
                return {_sensor._interval, Start()};
            }
            else if (!_sensor._echo.read())
            {
                return {_sensor._resolution, state};
            }
            else
            {
                return {_sensor._resolution, WaitingForLow{ now }};
            }
        }
        Result operator()(const WaitingForLow& state)
        {
            auto now = std::chrono::high_resolution_clock::now();
            if (now > state.started + _sensor._interval)
            {
                return {_sensor._interval, Start()};
            }
            else if (_sensor._echo.read())
            {
                return {_sensor._resolution, state};
            }

            using FloatSeconds = std::chrono::duration<float, std::chrono::seconds::period>;
            auto elapsed = FloatSeconds(now - state.started);
            auto distance = elapsed.count() * DistanceSensor::SpeedOfSound / 2;
            _sensor._handler.distance(_sensor, distance);
            return {_sensor._interval, Start()};
        }

        void fire() override
        {
            Result delay_state = boost::apply_visitor(*this, _state);
            _state = delay_state.second;
            _sensor._loop.set_alarm(delay_state.first, *this);
        }
    };


    DistanceSensor::DistanceSensor(PinNumber trigger, PinNumber echo, Loop& loop, DistanceHandler& handler)
        : _loop(loop)
        , _handler(handler)
        , _trigger(trigger)
        , _echo(echo)
        , _state(new StateMachine(*this))
    {
        set_frequency(1);
        set_resolution(0.002);
        _trigger.set(false);
        _loop.set_alarm(_interval, *_state);
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
