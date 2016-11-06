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

#include "repeated.h"
#include "doctest.h"


namespace Loop { namespace testing
{


    using namespace std::literals;


    TEST_CASE("Repeated")
    {
        Loop loop;
        int count = 0;
        Repeated repeated(loop, 10ms, [&]() { ++count; });
        loop.run_for(35ms);
        CHECK(count == 3);
    }


    TEST_CASE("Repeated::set_interval")
    {
        Loop loop;
        int count = 0;
        Repeated repeated(loop, 10ms, [&]() { repeated.set_interval(20ms); ++count; });
        loop.run_for(35ms);
        CHECK(count == 2);
    }


}}
