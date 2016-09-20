#pragma once


namespace Input {


    struct Event
    {
        bool shutdown = false;
        float direction = 0; // rad, 0 = forward
        float speed = 0; // 1 is max
    };


}
