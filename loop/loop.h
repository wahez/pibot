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

#include <queue>
#include <chrono>
#include <tuple>


namespace Loop
{


    class Loop;


    struct AlarmHandler
    {
        AlarmHandler(Loop& loop) : _loop(loop) {}
        ~AlarmHandler();
        AlarmHandler& operator=(const AlarmHandler&) = delete;
        virtual void fire() = 0;

    protected:
        Loop& _loop;
    };


    class Loop : private AlarmHandler
    {
    public:
        using Clock = std::chrono::high_resolution_clock;
        Loop();
        Loop(const Loop&) = delete;

        void run();

        template<typename Duration>
        void run_for(Duration timeout)
        {
            set_alarm(timeout, *this);
            run();
        }

        void stop();

        template<typename Duration>
        void set_alarm(Duration timeout, AlarmHandler& alarm)
        {
            _alarms.push(Alarm{Clock::now() + timeout, &alarm});
        }

        void remove_alarm(AlarmHandler&);

    private:
        void fire() override
        {
            stop();
        }

        struct Alarm
        {
            Clock::time_point time;
            AlarmHandler* alarm;

            bool operator<(const Alarm& other) const
            {
                return std::tie(time, alarm) > std::tie(other.time, other.alarm);
            }
        };
        bool _running = false;
        std::priority_queue<Alarm> _alarms;
    };


}
