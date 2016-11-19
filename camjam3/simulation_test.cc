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

#include "simulation.h"
#include "bot.h"
#include "../doctest/doctest.h"
#include <sstream>
#include <iostream>
#include <thread>
#include <cmath>


namespace CamJam3 { namespace Simulation { namespace testing
{


    std::string evaluate(const std::ostringstream& input)
    {
        std::ostringstream os;
        for (auto c: input.str())
        {
            if (c =='\r')
            {
                os.str("");
            }
            else
            {
                os << c;
            }
        }
        return os.str();
    }


    const std::string neutral       = "[           |          ] [           |          ]        ";
    const std::string left_forward  = "[           |++++++++++] [-----------|          ]        ";
    const std::string right_forward = "[-----------|          ] [           |++++++++++]        ";
    const std::string half_forward  = "[           |+++++     ] [           |+++++     ]        ";


    using namespace std::literals;


    TEST_CASE("Simulation")
    {
        wiringPiSetupGpio();
        SUBCASE("invalid read")
        {
            CHECK_THROWS(digitalRead(8));
            CHECK_THROWS(digitalRead(7));
            CHECK_THROWS(digitalRead(9));
            CHECK_THROWS(digitalRead(4));
            CHECK_THROWS(digitalRead(5));
        }
        SUBCASE("Motor")
        {
            std::ostringstream os;
            set_display(&os);
            digitalWrite(8, HIGH);
            digitalWrite(7, HIGH);
            digitalWrite(9, HIGH);
            digitalWrite(10, HIGH);
            std::this_thread::sleep_for(1ms);
            digitalWrite(8, LOW);
            digitalWrite(7, LOW);
            digitalWrite(9, LOW);
            digitalWrite(10, LOW);
            std::this_thread::sleep_for(1ms);

            SUBCASE("neutral_both_on")
            {
                for (int i = 0; i < 100; ++i)
                {
                    digitalWrite(8, HIGH);
                    digitalWrite(7, HIGH);
                    digitalWrite(9, HIGH);
                    digitalWrite(10, HIGH);
                    std::this_thread::sleep_for(10ms);
                }
                CHECK(evaluate(os) == neutral);
            }
            SUBCASE("neutral_both_off")
            {
                for (int i = 0; i < 100; ++i)
                {
                    digitalWrite(8, LOW);
                    digitalWrite(7, LOW);
                    digitalWrite(9, LOW);
                    digitalWrite(10, LOW);
                    std::this_thread::sleep_for(10ms);
                }
                CHECK(evaluate(os) == neutral);
            }
            SUBCASE("left_forward")
            {
                for (int i = 0; i < 100; ++i)
                {
                    digitalWrite(8, HIGH);
                    digitalWrite(7, LOW);
                    digitalWrite(9, LOW);
                    digitalWrite(10, HIGH);
                    std::this_thread::sleep_for(10ms);
                }
                CHECK(evaluate(os) == left_forward);
            }
            SUBCASE("right_forward")
            {
                for (int i = 0; i < 100; ++i)
                {
                    digitalWrite(8, LOW);
                    digitalWrite(7, HIGH);
                    digitalWrite(9, HIGH);
                    digitalWrite(10, LOW);
                    std::this_thread::sleep_for(10ms);
                }
                CHECK(evaluate(os) == right_forward);
            }
            SUBCASE("half_forward")
            {
                for (int i = 0; i < 4000; ++i)
                {
                    digitalWrite(8, LOW);
                    digitalWrite(7, LOW);
                    digitalWrite(9, LOW);
                    digitalWrite(10, LOW);
                    std::this_thread::sleep_for(10us);
                    digitalWrite(8, HIGH);
                    digitalWrite(7, LOW);
                    digitalWrite(9, HIGH);
                    digitalWrite(10, LOW);
                    std::this_thread::sleep_for(12us);
                }
                CHECK(evaluate(os) == half_forward);
            }
            set_display(nullptr);
        }
    }


    TEST_CASE("SimulationBot")
    {
        Loop::Loop loop;
        Bot bot(loop);
        set_display(&std::cout);
        for (int i = 0; i < 10; i++)
        {
            for (double a = 0; a < 2*M_PI; a += 2*M_PI/20)
            {
                bot.move(a, 1);
                loop.run_for(100ms);
            }
        }
        set_display(nullptr);
    }


}}}
