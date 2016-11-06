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

#include "gpio.h"
#include <loop/loop.h>
#include <loop/duty_cycle.h>


namespace CamJam3
{


    class Motor
    {
    public:
        Motor(Loop::Loop&, Pi::PinNumber forwardPin, Pi::PinNumber reversePin);

        void stop()    { move(0); }
        void forward() { move(1); }
        void reverse() { move(-1); }
        void move(float direction);

        void on();
        void off();

    private:
        Loop::DutyCycle _dutyCycle;
        Pi::OutputPin _forwardPin;
        Pi::OutputPin _reversePin;
        float _direction = 0;
    };


}
