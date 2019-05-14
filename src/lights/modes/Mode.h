#ifndef MODE_H
#define MODE_H

#include <Arduino.h>

namespace lights {
    class Mode
    {
    public:
        virtual void startup() = 0;
        virtual void tick() = 0;
        virtual void handleMessage(uint8_t * payload, size_t length) = 0;
        virtual void startActivity(uint8_t activityNum) {};
    };
}

#endif
