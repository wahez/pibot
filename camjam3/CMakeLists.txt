#    Copyright 2016 Walther Zwart
#
#    This file is part of pibot++.
#
#    pibot++ is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    pibot++ is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with pibot++. If not, see <http://www.gnu.org/licenses/>.

add_library(camjam3
    bot.h
    bot.cc
    distance_sensor.h
    distance_sensor.cc
    gpio.h
    gpio.cc
    line_sensor.h
    line_sensor.cc
    motor.h
    motor.cc
)

target_include_directories(camjam3 INTERFACE ..)
target_link_libraries(camjam3 loop)
if (NO_WIRINGPI)
    add_definitions(-DNO_WIRINGPI=1)
else()
    target_link_libraries(camjam3 wiringPi)
endif()

enable_testing()

add_executable(camjam3_test
    tests.cc
    distance_sensor_test.cc
)
target_link_libraries(camjam3_test camjam3)
add_test(camjam3_test camjam3_test)
