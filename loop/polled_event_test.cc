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

#include "polled_event.h"
#include "../doctest/doctest.h"


namespace Loop { namespace testing
{


    using namespace std::literals;


    TEST_CASE("PolledEvent::subscribe")
    {
        Loop loop;
        int eventCount = 0;
        PolledEvent<int> poller(loop, 10ms, [&](){ return ++eventCount; });
        int handlerCount = 0;
        poller.subscribe([&](int c) { CHECK(++handlerCount == c); }).release();
        loop.run_for(100ms);
        CHECK(handlerCount == eventCount);
        CHECK(eventCount != 0);
    }


    TEST_CASE("PolledEvent::multiple")
    {
        Loop loop;
        int eventCount = 0;
        PolledEvent<int> poller(loop, 10ms, [&](){ return ++eventCount; });
        int handlerCount1 = 0;
        poller.subscribe([&](int c) { CHECK(++handlerCount1 == c); }).release();
        int handlerCount2 = 0;
        poller.subscribe([&](int c) { CHECK(++handlerCount2 == c); }).release();
        loop.run_for(100ms);
        CHECK(eventCount != 0);
        CHECK(handlerCount1 == eventCount);
        CHECK(handlerCount2 == eventCount);
    }


    TEST_CASE("PolledEvent::unsubscribe")
    {
        Loop loop;
        int eventCount = 0;
        PolledEvent<int> poller(loop, 10ms, [&](){ return ++eventCount; });
        int handlerCount1 = 0;
        PolledEvent<int>::Subscription subscription;
        subscription = poller.subscribe([&](int c) { CHECK(++handlerCount1 == c); if (c > 5) subscription.reset(); });
        int handlerCount2 = 0;
        poller.subscribe([&](int c) { CHECK(++handlerCount2 == c); }).release();
        loop.run_for(100ms);
        CHECK(eventCount != 0);
        CHECK(handlerCount1 == 6);
        CHECK(handlerCount2 == eventCount);
    }


}}
