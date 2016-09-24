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
#include <functional>
#include <chrono>


namespace Pi
{


    class Loop
    {
    public:
        void run();

        void stop();

        template<typename Duration>
        void set_alarm(Duration timeout, std::function<void()> func)
        {
            _alarms.push(Alarm{std::chrono::high_resolution_clock::now() + timeout, std::move(func)});
        }

    private:
        struct Alarm
        {
            std::chrono::high_resolution_clock::time_point time;
            std::function<void()> func;

            bool operator<(const Alarm& other) const
            {
                return time < other.time;
            }
        };
        bool _running = false;
        std::priority_queue<Alarm> _alarms;
    };


}
