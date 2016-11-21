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

#include "distance_sensor.h"
#include <boost/variant.hpp>
#include <boost/units/systems/si/time.hpp>
#include <chrono>


namespace CamJam3
{


    using namespace std::literals;


    namespace {

        static const auto SpeedOfSound = 343.260 * SI::meters/SI::seconds;

        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = Clock::time_point;
        using Duration = Clock::duration;

        struct Start {};
        struct Triggering {};
        struct WaitingForHigh { TimePoint started; };
        struct WaitingForLow  { TimePoint started; };
        using State = boost::variant<Start, Triggering, WaitingForHigh, WaitingForLow>;
        using Result = std::pair<Duration, State>;

        constexpr const Duration trigger_length = 10us;


        auto units_to_chrono(Q<SI::time> time)
        {
            return std::chrono::duration<double>(time/SI::seconds);
        }

        auto chrono_to_units(std::chrono::duration<double> time)
        {
            return time.count() * SI::seconds;
        }

    }


    struct StateMachine : public Loop::AlarmHandler, public boost::static_visitor<Result>
    {
        DistanceSensor& _sensor;
        State _state = Start{};

        StateMachine(DistanceSensor& sensor)
            : AlarmHandler(sensor._loop)
            , _sensor(sensor)
        {
            _sensor._loop.set_alarm(_sensor._interval, *this);
        }

        Result operator()(Start)
        {
            _sensor._trigger.set(true);
            return {trigger_length, Triggering()};
        }
        Result operator()(Triggering)
        {
            _sensor._trigger.set(false);
            return {_sensor._resolution, WaitingForHigh{ Clock::now() }};
        }
        Result operator()(const WaitingForHigh& state)
        {
            auto now = Clock::now();
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
            auto now = Clock::now();
            if (now > state.started + _sensor._interval)
            {
                return {_sensor._interval, Start()};
            }
            else if (_sensor._echo.read())
            {
                return {_sensor._resolution, state};
            }

            auto elapsed = chrono_to_units(now - state.started);
            auto distance = 0.5 * elapsed * SpeedOfSound;
            _sensor.notify(_sensor, distance);
            return {_sensor._interval, Start()};
        }

        void fire() override
        {
            Result delay_state = boost::apply_visitor(*this, _state);
            _state = delay_state.second;
            _sensor._loop.set_alarm(delay_state.first, *this);
        }
    };


    DistanceSensor::DistanceSensor(Pi::PinNumber trigger, Pi::PinNumber echo, Loop::Loop& loop)
        : _loop(loop)
        , _trigger(trigger)
        , _echo(echo)
        , _state(new StateMachine(*this))
    {
        set_interval(1s);
        set_resolution(0.002 * SI::meters);
    }


    DistanceSensor::~DistanceSensor() {}


    void DistanceSensor::set_resolution(Distance spacial_resolution)
    {
        auto time = 2.0 * spacial_resolution / SpeedOfSound;
        _resolution = std::chrono::duration_cast<decltype(_resolution)>(units_to_chrono(time));
    }


}
