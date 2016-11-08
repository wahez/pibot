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

#include "loop.h"
#include <functional>


namespace Loop
{


    class DutyCycle: public AlarmHandler
    {
    public:
        using Handler = std::function<void(bool up)>;
        DutyCycle(Loop&, std::chrono::milliseconds, Handler);

        void set_duty_cycle(float); // between 0 and 1

    private:
        Handler _handler;
        std::chrono::milliseconds _interval;
        float _duty_cycle = 0;
        bool _isUp = false;

        void fire() override;
    };


}
