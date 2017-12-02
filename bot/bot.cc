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

#include "hardware.h"
#include "startup_mode.h"
#include "simple_mode.h"
#include "autonomous_mode.h"


using namespace std::literals;


struct Program
{
    Bot::Hardware hardware;
    std::unique_ptr<Modes::Base> mode;

    void run()
    {
        mode = std::make_unique<Modes::Startup>(hardware);
        auto subscription = hardware.event_poller.subscribe([this](const Input::Event& event)
        {
            if (event.switch_mode)
            {
                switch (mode->get_type())
                {
                    case Modes::Type::autonomous:
                    mode = std::make_unique<Modes::Simple>(hardware);
                    break;
                    case Modes::Type::startup:
                    mode = std::make_unique<Modes::Simple>(hardware);
                    break;
                    case Modes::Type::simple:
                    mode = std::make_unique<Modes::Autonomous>(hardware);
                    break;
                    case Modes::Type::shutdown:
                    break;
                }
            }
            if (event.shutdown) this->hardware.loop.stop();
        });
        hardware.loop.run();
    }
};


int main() {
    Program program;
    program.run();
}

