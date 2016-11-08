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

add_library(loop
    alarm.h
    alarm.cc
    duty_cycle.h
    duty_cycle.cc
    loop.h
    loop.cc
    polled_event.h
    repeated.h
    repeated.cc
    subscription_list.h
)

target_include_directories(loop INTERFACE ..)

add_executable(loop_test
    ../doctest/tests.cc
    duty_cycle_test.cc
    loop_test.cc
    polled_event_test.cc
    repeated_test.cc
)
target_link_libraries(loop_test loop)
add_test(loop_test loop_test)
