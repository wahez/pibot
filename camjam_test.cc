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
#include "doctest.h"

#include <iostream>
#include <thread>


namespace Pi { namespace testing
{


    TEST_CASE("DistanceSensorRaw")
    {
        using namespace std::literals;

        OutputPin trigger(17);
        InputPin echo(18);
        auto duration = 10us;
        for (int j = 0; j < 10; ++j)
        {
            trigger.set(true);
            std::this_thread::sleep_for(duration);
            trigger.set(false);
            for (auto i = 0; i < 100; ++i)
            {
                if (echo.read()) break;
                std::this_thread::sleep_for(duration);
            }
            auto start = std::chrono::high_resolution_clock::now();
            for (auto i = 0; i < 100; ++i)
            {
                if (!echo.read()) break;
                std::this_thread::sleep_for(duration);
            }
            auto end = std::chrono::high_resolution_clock::now();
            using FloatSeconds = std::chrono::duration<float, std::chrono::seconds::period>;
            auto elapsed = FloatSeconds(end - start);
            auto distance = elapsed.count() * 343.260 / 2;
            std::cout << (end-start).count() << " " << distance << std::endl;
            std::this_thread::sleep_for(100ms);
        }
        CHECK(true);
    }


    struct MockDistanceHandler : public DistanceHandler
    {
        using Callback = std::function<void(float)>;
        Callback callback;

        MockDistanceHandler(Callback cb) : callback(std::move(cb)) {}

        void distance(DistanceSensor&, float distance) override
        {
            callback(distance);
        }
    };


    TEST_CASE("DistanceSensor")
    {
        using namespace std::literals;

        Loop loop;
        MockDistanceHandler handler([](auto distance)
        {
            std::cout << "distance: " << distance << std::endl;
        });
        DistanceSensor ds(17, 18, loop, handler);
        loop.run_for(10s);
        CHECK(true);
    }


}}