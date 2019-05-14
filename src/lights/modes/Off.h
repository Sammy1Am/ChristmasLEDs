#include <Arduino.h>
#include "Mode.h"
#include "../control.h" // Include for access to FastLEDs, config, etc.

#ifndef CHRISTMASLIGHTS_MODE_OFF_H
#define CHRISTMASLIGHTS_MODE_OFF_H

namespace lights
{
class Off : public Mode
{
  public:
    void startup();
    void tick();
    void handleMessage(uint8_t *payload, size_t length);
};
} // namespace lights

#endif