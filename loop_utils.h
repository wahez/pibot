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
#include <map>
#include <functional>


namespace Pi
{


    class RepeatedBase : private AlarmHandler
    {
    public:
        using Duration = std::chrono::high_resolution_clock::duration;

        RepeatedBase(Loop&, Duration);
        ~RepeatedBase();

        virtual void repeat() = 0;

        void set_interval(Duration duration) { _duration = duration; }

    private:
        Loop& _loop;
        Duration _duration;
        void fire() override;
    };


    class Repeated : public RepeatedBase
    {
    public:
        using Handler = std::function<void()>;
        Repeated(Loop&, Duration, Handler);

    private:
        Handler _handler;
        void repeat() override;
    };


    template<typename Data>
    class PolledEvent : public RepeatedBase
    {
    public:
        using Tag = std::uint64_t;
        using Getter = std::function<Data()>;
        using Callback = std::function<void(const Data&)>;

        PolledEvent(Loop& loop, Duration duration, Getter data_getter)
            : RepeatedBase(loop, duration)
            , _data_getter(std::move(data_getter))
        {}

        Tag subscribe(Callback callback)
        {
            _subscribers[_next_tag] = std::move(callback);
            return _next_tag++;
        }

        void unsubscribe(Tag tag)
        {
            // TODO handle unsubscribe during iteration;
            _subscribers.erase(tag);
        }

    private:
        void repeat() override
        {
            Data data = _data_getter();
            for (const auto& tag_callback: _subscribers)
            {
                tag_callback.second(data);
            }
        }

        Getter _data_getter;
        std::map<Tag, Callback> _subscribers;
        Tag _next_tag = 0;
    };


    class DutyCycle: public AlarmHandler
    {
    public:
        using Handler = std::function<void(bool up)>;
        DutyCycle(Loop&, std::chrono::milliseconds, Handler);

        void set_duty_cycle(float); // between 0 and 1

    private:
        Loop& _loop;
        Handler _handler;
        std::chrono::milliseconds _interval;
        float _duty_cycle = 0;
        bool _isUp = false;

        void fire() override;
    };


}
