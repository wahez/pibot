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

#include "wiimote.h"
#include <camjam3/bot.h>
#include <loop/loop.h>
#include <loop/polled_event.h>
#include <memory>
#include <chrono>


namespace Bot {


    using namespace std::literals;


    struct Hardware
    {
        Loop::Loop loop;
        CamJam3::Bot bot;
        std::unique_ptr<Input::WiiMote> wiimote;

        using EventPoller = Loop::PolledEvent<Input::Event>;
        EventPoller event_poller;

        Hardware()
            : loop()
            , bot(loop)
            , event_poller(loop, 10ms, []() { return Input::Event{}; })
        {}

        void set_wiimote(std::unique_ptr<Input::WiiMote> wm)
        {
            wiimote = std::move(wm);
            event_poller.set_getter([this]() { return this->wiimote->getEvent(); });
        }
    };


}
