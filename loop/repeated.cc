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

#include "repeated.h"


namespace Loop
{


    RepeatedBase::RepeatedBase(Loop& loop, Duration duration)
        : AlarmHandler(loop)
        , _duration(duration)
    {
        _loop.set_alarm(_duration, *this);
    }


    void RepeatedBase::fire()
    {
        _loop.set_alarm(_duration, *this);
        this->repeat();
    }


    Repeated::Repeated(Loop& loop, Duration duration, Handler handler)
        : RepeatedBase(loop, duration)
        , _handler(std::move(handler))
    {}


    void Repeated::repeat()
    {
        _handler();
    }


}
