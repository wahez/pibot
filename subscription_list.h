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

#include <map>
#include <set>
#include <vector>
#include <functional>


namespace Pi
{


    template<typename... Args>
    class SubscriptionList
    {
    public:
        using Tag = std::uint64_t;
        using Callback = std::function<void(Args...)>;

        Tag subscribe(Callback callback)
        {
            if (_iterating)
                _added_subscribers.push_back(std::make_pair(_next_tag, std::move(callback)));
            else
                _subscribers[_next_tag] = std::move(callback);
            return _next_tag++;
        }

        void unsubscribe(Tag tag)
        {
            if (_iterating)
                _deletedTags.insert(tag);
            else
                _subscribers.erase(tag);
        }

        template<typename... ARGS>
        void notify(ARGS&&... args)
        {
            _iterating = true;
            for (const auto& tag_callback: _subscribers)
            {
                tag_callback.second(std::forward<ARGS>(args)...);
            }
            for (auto tag: _deletedTags)
            {
                _subscribers.erase(tag);
            }
            _deletedTags.clear();
            for (auto& tag_callback: _added_subscribers)
            {
                _subscribers[tag_callback.first] = std::move(tag_callback.second);
            }
            _added_subscribers.clear();
            _iterating = false;
        }

    private:
        std::map<Tag, Callback> _subscribers;
        std::set<Tag> _deletedTags;
        std::vector<std::pair<Tag, Callback>> _added_subscribers;
        bool _iterating = false;
        Tag _next_tag = 0;
    };


}
