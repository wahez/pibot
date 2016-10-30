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


struct Hardware
{
    Pi::Loop loop;
    std::unique_ptr<Input::WiiMote> wiimote;
    Pi::Bot bot;

    Hardware()
        : loop()
        , bot(loop)
    {}
};


struct SimpleMode : public Pi::AlarmHandler
{
    Hardware& hardware;
    SimpleMode(Hardware& hw)
        : hardware(hw)
    {}

    void activate()
    {
        hardware.loop.set_alarm(10ms, *this);
    }

    void fire() override
    {
        auto event = hardware.wiimote->getEvent();
        if (event.shutdown) hardware.loop.stop();
        hardware.bot.move(event.direction, event.speed);
        hardware.loop.set_alarm(10ms, *this);
    }
};


struct Program
{
    Hardware hardware;
    SimpleMode simpleMode;

    Program()
        : simpleMode(hardware)
    {}

    void run()
    {
        for (;;)
        {
            try
            {
                hardware.bot.move(0.5*M_PI, 1);
                hardware.loop.run_for(100ms);
                hardware.bot.move(1.5*M_PI, 1);
                hardware.loop.run_for(100ms);
                hardware.bot.move(0, 0);
                hardware.loop.run_for(100ms);
                std::cout << "Press 1+2 on the wiimote" << std::endl;
                //window.text("Press 1+2 on the wiimote");
                hardware.wiimote.reset(new Input::WiiMote());
                break;
            }
            catch (const std::runtime_error& ex)
            {
                std::cout << ex.what() << std::endl;
            }
        }
        std::cout << "OK" << std::endl;
        hardware.wiimote->rumble(true);
        hardware.loop.run_for(100ms);
        hardware.wiimote->rumble(false);

        simpleMode.activate();
        hardware.loop.run();
    }

};


int main() {
    Program program;
    program.run();
}

