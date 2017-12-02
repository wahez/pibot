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

#include "distance_sensor.h"
#include "motor.h"
#include "line_sensor.h"


namespace Loop { class Loop; }


namespace CamJam3
{


    class Bot
    {
    public:
        Bot(Loop::Loop&);

        void move(float direction, float speed);

        DistanceSensor& get_distance_sensor() { return _distance_sensor; }

    private:
        Motor _left;
        Motor _right;
        LineSensor _line_sensor;
        DistanceSensor _distance_sensor;

        void update(bool);
    };


}
