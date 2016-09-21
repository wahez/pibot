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

#include "input.h"

#include <curses.h>
#include <stdexcept>
#include <cmath>


namespace Input {


    Window::Window()
        : _handle(initscr())
    {
        if (_handle == nullptr)
        {
            throw std::runtime_error("Error initializing ncurses");
        }
        noecho();
    }


    void Window::text(const std::string& t)
    {
        mvaddstr(5, 10, t.c_str());
        refresh();
    }


    Event Window::getEvent()
    {
        constexpr float DIR = 2 * M_PI / 8;
        Event event;
        event.speed = 1;
        auto ch = getChar();
        switch (ch)
        {
        case ' ':
            event.shutdown = true;
            break;
        case 's':
            event.speed = 0;
            break;
        case 'w':
            event.direction = 0 * DIR;
            break;
        case 'e':
            event.direction = 1 * DIR;
            break;
        case 'd':
            event.direction = 2 * DIR;
            break;
        case 'c':
            event.direction = 3 * DIR;
            break;
        case 'x':
            event.direction = 4 * DIR;
            break;
        case 'z':
            event.direction = 5 * DIR;
            break;
        case 'a':
            event.direction = 6 * DIR;
            break;
        case 'q':
            event.direction = 7 * DIR;
            break;
        default:
            event.speed = 0;
        }
        return event;
    }


    char Window::getChar()
    {
        return getch();
    }


    Window::~Window()
    {
        delwin(static_cast<WINDOW*>(_handle));
        endwin();
    }


}

