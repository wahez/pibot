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
#include "doctest.h"

#include <functional>
#include <iostream>


namespace Pi { namespace testing
{


    using namespace std::literals;


    struct MockAlarmHandler : public AlarmHandler
    {
        MockAlarmHandler(std::function<void()> f) : func(f) {}
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
        MockAlarmHandler alarm{ [&](){ ++called; }};
        loop.set_alarm(10ms, alarm);
        loop.run();
        CHECK(called == 1);
    }

    TEST_CASE("Loop::run_for")
    {
        Loop loop;
        int fastCalled = 0;
        int slowCalled = 0;
        MockAlarmHandler fastAlarm{ [&](){ ++fastCalled; }};
        MockAlarmHandler slowAlarm{ [&](){ ++slowCalled; }};
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
        MockAlarmHandler fastAlarm{ [&](){ loop.stop(); }};
        MockAlarmHandler slowAlarm{ [&](){ ++slowCalled; }};
        loop.set_alarm(30ms, slowAlarm);
        loop.set_alarm(10ms, fastAlarm);
        loop.run();
        CHECK(slowCalled == 0);
    }


    struct MockDutyCycleHandler : public DutyCycleHandler
    {
        MockDutyCycleHandler(std::function<void(bool)> f) : func(f) {}
        std::function<void(bool)> func;
        void up() override { func(true); }
        void down() override { func(false); }
    };

    TEST_CASE("DutyCycle::duty_cycle")
    {
        Loop loop;
        int up_calls = 0;
        int down_calls = 0;
        std::chrono::high_resolution_clock::duration up_time(0);
        std::chrono::high_resolution_clock::duration down_time(0);
        auto lastState = false;
        auto last = std::chrono::high_resolution_clock::now();
        MockDutyCycleHandler handler([&](bool state)
        {
            CHECK(state != lastState);
            lastState = state;
            auto now = std::chrono::high_resolution_clock::now();
            if (state)
            {
                ++up_calls;
                down_time += now - last;
            }
            else
            {
                ++down_calls;
                up_time += now - last;
            }
            last = now;
        });
        MockAlarmHandler stopper([&]() { loop.stop(); });
        loop.set_alarm(100ms, stopper);
        DutyCycle dc(loop, 10ms, handler);
        SUBCASE("0")
        {
            dc.set_duty_cycle(0);
            loop.run();

            CHECK(up_calls == 9);
            CHECK(down_calls == 9);
            CHECK(up_time.count()/1'000'000 == 0);
            CHECK(down_time.count()/1'000'000 == 90);
        }
        SUBCASE("0.50")
        {
            dc.set_duty_cycle(0.5);
            loop.run();

            CHECK(up_calls == 9);
            CHECK(down_calls == 9);
            CHECK(up_time.count()/1'000'000 == 45);
            CHECK(down_time.count()/1'000'000 == 50);
        }
        SUBCASE("1")
        {
            dc.set_duty_cycle(1);
            loop.run();

            CHECK(up_calls == 9);
            CHECK(down_calls == 8);
            CHECK(up_time.count()/1'000'000 == 80);
            CHECK(down_time.count()/1'000'000 == 10);
        }
    }


}}
