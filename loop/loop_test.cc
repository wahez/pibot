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
#include "../doctest/doctest.h"

#include <functional>
#include <iostream>


namespace Loop { namespace testing
{


    using namespace std::literals;


    struct MockAlarmHandler : public AlarmHandler
    {
        MockAlarmHandler(Loop& loop, std::function<void()> f)
            : AlarmHandler(loop)
            , func(f) {}
        std::function<void()> func;
        void fire() override { func(); }
    };


    TEST_CASE("Loop::run empty returns")
    {
        Loop loop;
        loop.run();
        CHECK(true);
    }

    TEST_CASE("Loop::run")
    {
        Loop loop;
        int called = 0;
        MockAlarmHandler alarm{loop, [&](){ ++called; }};
        loop.set_alarm(10ms, alarm);
        loop.run();
        CHECK(called == 1);
    }

    TEST_CASE("Loop::run_for")
    {
        Loop loop;
        int fastCalled = 0;
        int slowCalled = 0;
        MockAlarmHandler fastAlarm{loop, [&](){ ++fastCalled; }};
        MockAlarmHandler slowAlarm{loop, [&](){ ++slowCalled; }};
        loop.set_alarm(30ms, slowAlarm);
        loop.set_alarm(10ms, fastAlarm);
        loop.run_for(20ms);
        CHECK(fastCalled == 1);
        CHECK(slowCalled == 0);
    }

    TEST_CASE("Loop::stop")
    {
        Loop loop;
        int slowCalled = 0;
        MockAlarmHandler fastAlarm{loop, [&](){ loop.stop(); }};
        MockAlarmHandler slowAlarm{loop, [&](){ ++slowCalled; }};
        loop.set_alarm(30ms, slowAlarm);
        loop.set_alarm(10ms, fastAlarm);
        loop.run();
        CHECK(slowCalled == 0);
    }


    TEST_CASE("Loop::remove_alarm")
    {
        Loop loop;
        int slowCalled = 0;
        MockAlarmHandler slowAlarm{loop, [&](){ ++slowCalled; }};
        MockAlarmHandler fastAlarm{loop, [&](){ loop.remove_alarm(slowAlarm); }};
        loop.set_alarm(30ms, slowAlarm);
        loop.set_alarm(10ms, fastAlarm);
        loop.run();
        CHECK(slowCalled == 0);
    }


}}
