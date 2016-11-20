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

#include "simulation.h"
#include <boost/optional.hpp>
#include <map>
#include <memory>
#include <cmath>
#include <ostream>


namespace CamJam3 { namespace Simulation
{


    namespace {


        std::ostream* output;


        using Clock = std::chrono::high_resolution_clock;
        using Duration = Clock::duration;
        using Time = Clock::time_point;
        using namespace std::literals;


        struct SingleLineMeter
        {
            double v;
            friend std::ostream& operator<<(std::ostream& os, SingleLineMeter d)
            {
                auto v = d.v;
                double i;
                os << '[';
                for (i = std::min(-1.0, v); i < std::min(0.0, v); i += 0.1)
                {
                    os << ' ';
                }
                for (; i < 0; i += 0.1)
                {
                    os << '-';
                }
                os << '|';
                for (; i < v; i += 0.1)
                {
                    os << '+';
                }
                for (; i < v; i += 0.1)
                {
                    os << '+';
                }
                for (; i < 1; i += 0.1)
                {
                    os << ' ';
                }
                os << ']';
                return os;
            }
        };


        void trottledDisplay(const std::string& name, double v)
        {
            static Time last = Clock::now();
            static double left = 0;
            static double right = 0;
            if (name == "left")
            {
                left = v;
            }
            else
            {
                right = v;
            }
            auto now = Clock::now();
            if (now - last > 100ms)
            {
                if (output)
                {
                    *output << '\r' << SingleLineMeter{left} << ' ' << SingleLineMeter{right} << "        " << std::flush;
                }
                last = now;
            }
        }


        template<typename T, typename Duration>
        struct ExponentialDecay
        {
            const Duration half_life;
            T value{};

            ExponentialDecay(Duration hl) : half_life(hl) {}

            void add(Duration d, T t)
            {
                auto decay = std::exp(-d.count() * 1.0/half_life.count());
                value = decay * value + (1 - decay) * t;
            }
        };


        struct PinHandler
        {
            virtual Level handle_read(Pi::PinNumber) = 0;
            virtual void handle_write(Pi::PinNumber, Level) = 0;
        };
        

        std::map<Pi::PinNumber, std::shared_ptr<PinHandler>> pin_handlers;


        struct SimMotor : PinHandler
        {
            const std::string name;
            const Pi::PinNumber forward;
            const Pi::PinNumber reverse;
            SimMotor(std::string nme, Pi::PinNumber f, Pi::PinNumber r)
                : name(nme)
                , forward(f)
                , reverse(r)
                , time_forward(ED{std::chrono::milliseconds(10)})
                , time_reverse(ED{std::chrono::milliseconds(10)})
            {}

            Level handle_read(Pi::PinNumber) override
            {
                throw std::runtime_error("read on motor");
            }
            void handle_write(Pi::PinNumber pin, Level level) override
            {
                if (pin != forward && pin != reverse) throw std::runtime_error("invalid write on motor");

                // TODO measure duty cycle and print
                auto now = Clock::now();
                if (forwardState && reverseState)
                {
                    auto interval = now - lastSample;
                    if (HIGH == *forwardState && LOW == *reverseState)
                    {
                        time_forward.add(interval, 1);
                        time_reverse.add(interval, 0);
                    }
                    else if (LOW == *forwardState && HIGH == *reverseState)
                    {
                        time_forward.add(interval, 0);
                        time_reverse.add(interval, 1);
                    }
                    else
                    {
                        time_forward.add(interval, 0);
                        time_reverse.add(interval, 0);
                    }
                }

                // update state
                lastSample = now;
                if (pin == forward) forwardState = level;
                if (pin == reverse) reverseState = level;

                trottledDisplay(name, time_forward.value - time_reverse.value);
            }

            Time lastSample;
            boost::optional<Level> forwardState;
            boost::optional<Level> reverseState;
            using ED = ExponentialDecay<double, Duration>;
            ED time_forward;
            ED time_reverse;
        };


        struct SimDistanceSensor : PinHandler
        {
            Pi::PinNumber trigger;
            Pi::PinNumber echo;
            SimDistanceSensor(Pi::PinNumber t, Pi::PinNumber e) : trigger(t), echo(e) {}
            Level handle_read(Pi::PinNumber pin) override
            {
                if (pin != echo) throw std::runtime_error("invalid read on distance_sensor");
                if (triggerEnd)
                {
                    auto now = Clock::now();
                    if (now > *triggerEnd + 10ms && now < *triggerEnd + 70ms)
                    {
                        return HIGH;
                    }
                }
                return LOW;
            }
            void handle_write(Pi::PinNumber pin, Level level) override
            {
                if (pin != trigger) throw std::runtime_error("invalid write on distance_sensor");
                if (!triggerEnd && LOW == level)
                {
                    triggerEnd = Clock::now();
                }
                else
                {
                    triggerEnd = boost::none;
                }
            }

            boost::optional<Time> triggerEnd;
        };


    }

    
    void add_motor(std::string name, Pi::PinNumber forward, Pi::PinNumber reverse)
    {
        auto motor = std::make_shared<SimMotor>(name, forward, reverse);
        pin_handlers[forward] = motor;
        pin_handlers[reverse] = motor;
    }


    void add_distance_sensor(Pi::PinNumber trigger, Pi::PinNumber echo)
    {
        auto sensor = std::make_shared<SimDistanceSensor>(trigger, echo);
        pin_handlers[trigger] = sensor;
        pin_handlers[echo] = sensor;
    }


    void set_display(std::ostream* os)
    {
        output = os;
    }


    void wiringPiSetupGpio()
    {
        add_motor("left", 8, 7);
        add_motor("right", 9, 10);
        add_distance_sensor(17, 18);
    }


    void digitalWrite(Pi::PinNumber pin, Level level)
    {
        pin_handlers.at(pin)->handle_write(pin, level);
    }


    Level digitalRead(Pi::PinNumber pin)
    {
        return pin_handlers.at(pin)->handle_read(pin);
    }


}}
