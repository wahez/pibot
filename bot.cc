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
#include "wiimote.h"
#include "loop.h"
#include <iostream>
#include <cmath>
#include <chrono>


using namespace std::literals;


struct Hardware
{
    Pi::Loop loop;
    Pi::Bot bot;
    std::unique_ptr<Input::WiiMote> wiimote;

    using EventPoller = Pi::PolledEvent<Input::Event>;
    std::unique_ptr<EventPoller> event_poller;

    Hardware()
        : loop()
        , bot(loop)
    {}

    void set_wiimote(std::unique_ptr<Input::WiiMote> wm)
    {
        wiimote = std::move(wm);
        if (!event_poller)
        {
            event_poller = std::make_unique<EventPoller>(loop, 10ms, [this]() { return this->wiimote->getEvent(); });
        }
    }
};


struct Mode
{
    virtual ~Mode() {}
};


struct SimpleMode : Mode
{
    Hardware& hardware;
    Hardware::EventPoller::Tag pollerTag;
    Pi::DistanceSensor::Tag distanceTag;

    SimpleMode(Hardware& hw)
        : hardware(hw)
    {
        hardware.event_poller->set_interval(10ms);
        pollerTag = hardware.event_poller->subscribe([this](const Input::Event& event)
        {
            this->hardware.bot.move(event.direction, event.speed);
        });
        distanceTag = hardware.bot.get_distance_sensor().subscribe([this](auto&&, float distance)
        {
            hardware.wiimote->rumble(distance < 0.10);
        });
    }

    ~SimpleMode()
    {
        hardware.bot.get_distance_sensor().unsubscribe(distanceTag);
        hardware.wiimote->rumble(false);
        hardware.event_poller->unsubscribe(pollerTag);
    }
};


struct Program
{
    Hardware hardware;
    std::unique_ptr<Mode> mode;

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
                auto wiimote = std::make_unique<Input::WiiMote>();
                hardware.set_wiimote(std::move(wiimote));
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

        mode = std::make_unique<SimpleMode>(hardware);
        hardware.event_poller->subscribe([this](const Input::Event& event)
        {
            if (event.shutdown) this->hardware.loop.stop();
        });
        hardware.loop.run();
    }
};


int main() {
    Program program;
    program.run();
}

