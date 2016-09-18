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

#include "gpio.h"

#include <wiringPi.h>


namespace Pi
{


    void Init()
    {
        static bool initted = false;
        if (!initted)
            wiringPiSetupGpio();
        initted = true;
    }


    OutputPin::OutputPin(PinNumber pin)
        : _pin(pin)
    {
        Init();
        pinMode(_pin, OUTPUT);
    }


    void OutputPin::set(bool value)
    {
        digitalWrite(_pin, value ? HIGH : LOW);
    }


    InputPin::InputPin(PinNumber pin)
        : _pin(pin)
    {
        Init();
        pinMode(_pin, INPUT);
    }


    bool InputPin::read()
    {
        return digitalRead(_pin) == HIGH;
    }


}
