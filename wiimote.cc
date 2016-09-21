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

#include "wiimote.h"

#include <bluetooth/bluetooth.h>
#include <cwiid.h>
#include <map>
#include <cmath>


namespace Input {


    struct WiiMoteImpl
    {
        cwiid_wiimote_t *wiimote = nullptr;
        unsigned char ledstate = 0;
    };


    namespace {


        void error_callback(cwiid_wiimote_t *wiimote, const char *s, va_list ap)
        {
            throw std::runtime_error(s);
        }


    }


    WiiMote::WiiMote()
        : _impl(new WiiMoteImpl())
    {
        cwiid_set_err(error_callback);
        bdaddr_t bdaddr_any{{0, 0, 0, 0, 0, 0}};
        _impl->wiimote = cwiid_open(&bdaddr_any, 0);
        if (!_impl->wiimote)
            throw std::runtime_error("Cannot connect to wiimote");

        if (cwiid_set_rpt_mode(_impl->wiimote, CWIID_RPT_NUNCHUK))
        {
            throw std::runtime_error("Cannot set nunchuk report mode");
        }
    }


    WiiMote::~WiiMote()
    {
        cwiid_close(_impl->wiimote);
    }


    void WiiMote::setLed(unsigned char led, bool value)
    {
        if (value)
            _impl->ledstate |= CWIID_LED1_ON;
        else
            _impl->ledstate &= ~CWIID_LED1_ON;

        if (cwiid_set_led(_impl->wiimote, _impl->ledstate))
        {
            throw std::runtime_error("Could not set led state");
        }
    }


    void WiiMote::rumble(bool state)
    {
        unsigned char rumble = state ? 1 : 0;
        if (cwiid_set_rumble(_impl->wiimote, rumble))
        {
            throw std::runtime_error("Could not set rumble state");
        }
    }


    Event WiiMote::getEvent()
    {
        cwiid_state state;
        if (cwiid_get_state(_impl->wiimote, &state))
        {
            throw std::runtime_error("Could not get wiimote state");
        }

        Event result;
        result.direction = 0;
        result.speed = 0;
        if (state.ext_type == CWIID_EXT_NUNCHUK)
        {

            result.shutdown = state.ext.nunchuk.buttons > 0;
            float scaledX = state.ext.nunchuk.stick[CWIID_X] / 256.0 - 0.5;
            float scaledY = state.ext.nunchuk.stick[CWIID_Y] / 256.0 - 0.5;
            result.speed = 2.4*std::sqrt(scaledX*scaledX + scaledY*scaledY);
            if (scaledY < 0)
                result.direction = std::atan(scaledX/scaledY) + M_PI;
            else if (scaledY > 0)
            {
                result.direction = std::atan(scaledX/scaledY);
                if (result.direction < 0)
                    result.direction += 2 * M_PI;
            }
            else
                result.direction = 0;
        }
        return result;
    }


}

