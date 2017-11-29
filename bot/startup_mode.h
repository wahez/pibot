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

#pragma once

#include "hardware.h"
#include "mode.h"
#include <loop/state_machine.h>
#include <iostream>


namespace Modes {


    using namespace std::literals;


    struct Startup : Base
    {
        struct RotatingLeft {};
        struct RotatingRight {};
        struct Stopped {};
        struct Connecting {};
        struct Confirmed {};
        Bot::Hardware& hardware;
        Loop::StateMachineT<
                Startup,
                RotatingLeft,
                RotatingRight,
                Stopped,
                Connecting,
                Confirmed>
            stateMachine;

        explicit Startup(Bot::Hardware& hw)
            : hardware(hw)
            , stateMachine(*this, hw.loop)
        { }

        ~Startup()
        {
            hardware.bot.move(0, 0);
        }

        auto operator()(RotatingLeft)
        {
            hardware.bot.move(0.5*M_PI, 1);
            return std::make_pair(100ms, RotatingRight{});
        }

        auto operator()(RotatingRight)
        {
            hardware.bot.move(1.5*M_PI, 1);
            return std::make_pair(100ms, Stopped{});
        }

        auto operator()(Stopped)
        {
            hardware.bot.move(0, 0);
            return std::make_pair(100ms, Connecting{});
        }

        std::pair<std::chrono::milliseconds, decltype(stateMachine.state)> operator()(Connecting)
        {
            try
            {
                std::cout << "Press 1+2 on the wiimote" << std::endl;
                //window.text("Press 1+2 on the wiimote");
                auto wiimote = std::make_unique<Input::WiiMote>();
                hardware.set_wiimote(std::move(wiimote));
                hardware.wiimote->rumble(true);
                return {100ms, Confirmed{}};
            }
            catch (const std::runtime_error& ex)
            {
                std::cout << ex.what() << std::endl;
                return {100ms, RotatingLeft{}};
            }
        }

        auto operator()(const Confirmed& state)
        {
            hardware.wiimote->rumble(false);
            Input::Event event;
            event.switch_mode = true;
            hardware.event_poller.notify(event);
            return nullptr;
        }

        Type get_type() const override { return Type::startup; }
    };


}
