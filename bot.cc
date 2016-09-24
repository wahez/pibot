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
//#include "input.h"
#include "wiimote.h"
#include "loop.h"
#include <iostream>
#include <cmath>
#include <chrono>


using namespace std::literals;


struct Program : public Pi::AlarmHandler
{
    Pi::Loop loop;
    //Input::Window window;
    std::unique_ptr<Input::WiiMote> wiimote;

    Pi::Bot bot;

    Program()
        : bot(loop)
    {}

    void run()
    {
        for (;;)
        {
            try
            {
                bot.move(0.5*M_PI, 1);
                loop.run_for(100ms);
                bot.move(1.5*M_PI, 1);
                loop.run_for(100ms);
                bot.move(0, 0);
                loop.run_for(100ms);
                std::cout << "Press 1+2 on the wiimote" << std::endl;
                //window.text("Press 1+2 on the wiimote");
                wiimote.reset(new Input::WiiMote());
                break;
            }
            catch (const std::runtime_error& ex)
            {
                std::cout << ex.what() << std::endl;
            }
        }
        std::cout << "OK" << std::endl;
        wiimote->rumble(true);
        loop.run_for(100ms);
        wiimote->rumble(false);

        loop.set_alarm(10ms, *this);
        loop.run();
    }

    void fire() override
    {
//        auto event = window.getEvent();
        auto event = wiimote->getEvent();
        if (event.shutdown) loop.stop();
        bot.move(event.direction, event.speed);
        loop.set_alarm(10ms, *this);
    }
};


int main() {
    Program program;
    program.run();
}

