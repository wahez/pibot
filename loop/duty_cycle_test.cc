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

#include "duty_cycle.h"
#include "../doctest/doctest.h"
#include <cmath>


namespace Loop { namespace testing
{


    using namespace std::literals;


    TEST_CASE("DutyCycle::duty_cycle")
    {
        Loop loop;
        int up_calls = 0;
        int down_calls = 0;
        std::chrono::high_resolution_clock::duration up_time(0);
        std::chrono::high_resolution_clock::duration down_time(0);
        auto lastState = false;
        auto last = std::chrono::high_resolution_clock::now();
        auto handler([&](bool state)
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
        DutyCycle dc(loop, 10ms, handler);
        SUBCASE("0")
        {
            dc.set_duty_cycle(0);
            loop.run_for(100ms);

            CHECK(up_calls == 9);
            CHECK(down_calls == 9);
            CHECK(up_time.count()/1'000'000 == 0);
            CHECK(std::abs(down_time.count()/1'000'000 - 90) < 3);
        }
        SUBCASE("0.50")
        {
            dc.set_duty_cycle(0.5);
            loop.run_for(100ms);

            CHECK(up_calls == 9);
            CHECK(down_calls == 9);
            CHECK(std::abs(up_time.count()/1'000'000 - 45) < 3);
            CHECK(std::abs(down_time.count()/1'000'000 - 50) < 3);
        }
        SUBCASE("1")
        {
            dc.set_duty_cycle(1);
            loop.run_for(100ms);

            CHECK(up_calls == 9);
            CHECK(down_calls == 8);
            CHECK(std::abs(up_time.count()/1'000'000 - 80) < 3);
            CHECK(std::abs(down_time.count()/1'000'000 - 10) < 3);
        }
    }


}}
