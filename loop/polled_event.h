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

#include "subscription_list.h"
#include "repeated.h"


namespace Loop
{


    template<typename Data>
    class PolledEvent : public RepeatedBase, public SubscriptionList<Data>
    {
    public:
        using Tag = typename SubscriptionList<Data>::Tag;
        using Getter = std::function<Data()>;
        using Callback = typename SubscriptionList<Data>::Callback;

        PolledEvent(Loop& loop, Duration duration, Getter data_getter)
            : RepeatedBase(loop, duration)
            , _data_getter(std::move(data_getter))
        {}

        void set_getter(Getter getter) { _data_getter = std::move(getter); }

    private:
        void repeat() override
        {
            this->notify(_data_getter());
        }

        Getter _data_getter;
    };


}
