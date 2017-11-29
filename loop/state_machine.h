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
#include "alarm.h"
#include <boost/variant/variant.hpp>


namespace Loop
{

    /*
     * StateMachineT can be templated on the handler and the possible states.
     * The handler should accepts calls with the state as an argument. It should
     * return a pair of a delay (duration) and the new state.
     * If the handler destroys itself or the StateMachineT it should return nullptr;
     */
    template<typename Visitor, typename... States>
    struct StateMachineT : AlarmHandler, boost::static_visitor<void>
    {
        Visitor& visitor;
        using State = boost::variant<States...>;
        State state;

        StateMachineT(Visitor& visitor, Loop& loop)
            : AlarmHandler(loop)
            , visitor(visitor)
        {
            loop.set_alarm(std::chrono::seconds{}, *this);
        }

        template<typename CurrentState>
        void operator()(CurrentState&& state)
        {
            store(visitor(std::forward<CurrentState>(state)));
        }

        void fire() override
        {
            boost::apply_visitor(*this, state);
        }

    private:
        void store(nullptr_t) { }

        template<typename Pair>
        void store(Pair&& delay_state)
        {
            this->state = std::forward<typename Pair::second_type>(delay_state.second);
            _loop.set_alarm(delay_state.first, *this);
        }
    };


}
