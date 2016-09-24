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

#include "loop.h"

#include <thread>


namespace Pi
{


    void Loop::run()
    {
        _running = true;
        while (_running && !_alarms.empty())
        {
            auto now = Clock::now();
            auto nextAlarm = _alarms.top();
            _alarms.pop();
            auto timeToWait = nextAlarm.time - now;
            if (timeToWait.count() > 0)
            {
                std::this_thread::sleep_for(timeToWait);
            }
            nextAlarm.alarm->fire();
        }
    }


    void Loop::stop()
    {
        _running = false;
    }


    DutyCycle::DutyCycle(Loop& loop, std::chrono::milliseconds interval, DutyCycleHandler& handler)
        : _loop(loop)
        , _handler(&handler)
        , _interval(interval)
    {
        _loop.set_alarm(interval, *this);
    }


    void DutyCycle::set_duty_cycle(float duty_cycle)
    {
        _duty_cycle = std::min(1.0f, std::max(0.0f, duty_cycle));
    }


    void DutyCycle::fire()
    {
        _isUp = !_isUp;
        auto ms = _interval.count();
        if (_isUp)
        {
            _handler->up();
            ms *= _duty_cycle;
        }
        else
        {
            _handler->down();
            ms *= (1-_duty_cycle);
        }
        _loop.set_alarm(std::chrono::milliseconds(ms), *this);
    }


}
