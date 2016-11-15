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

#ifndef NO_WIRINGPI
#include <wiringPi.h>
#endif


namespace Pi
{


    void Init()
    {
#ifndef NO_WIRINGPI
        static bool initted = false;
        if (!initted)
            wiringPiSetupGpio();
        initted = true;
#endif
    }


    OutputPin::OutputPin(PinNumber pin)
        : _pin(pin)
    {
        Init();
#ifndef NO_WIRINGPI
        pinMode(_pin, OUTPUT);
#else
        (void)_pin;
#endif
    }


    void OutputPin::set(bool value)
    {
#ifndef NO_WIRINGPI
        digitalWrite(_pin, value ? HIGH : LOW);
#endif
    }


    InputPin::InputPin(PinNumber pin)
        : _pin(pin)
    {
        Init();
#ifndef NO_WIRINGPI
        pinMode(_pin, INPUT);
#else
        (void)_pin;
#endif
    }


    bool InputPin::read()
    {
#ifndef NO_WIRINGPI
        return digitalRead(_pin) == HIGH;
#else
        return false;
#endif
    }


}
