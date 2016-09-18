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

#include "camjam3.h"
#include "input.h"

#include <chrono>
#include <thread>


int main() {
    using namespace std::literals;
    Pi::Bot bot;
    Input::Window window;

    window.text("Press a key (' ' to quit)...");

    for (;;)
    {
        auto ch = window.getChar();
        switch (ch)
        {
        case 'q':
            bot.forwardLeft();
            break;
        case 'w':
            bot.forward();
            break;
        case 'e':
            bot.forwardRight();
            break;
        case 'a':
            bot.rotateLeft();
            break;
        case 's':
            bot.stop();
            break;
        case 'd':
            bot.rotateRight();
            break;
        case 'z':
            bot.reverseLeft();
            break;
        case 'x':
            bot.reverse();
            break;
        case 'c':
            bot.reverseRight();
            break;
        }
        if (ch == ' ')
            break;
            
        std::this_thread::sleep_for(10ms);
    }
}

