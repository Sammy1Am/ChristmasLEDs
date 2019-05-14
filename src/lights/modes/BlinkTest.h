#ifndef CHRISTMASLIGHTS_MODE_BLINKTEST_H
#define CHRISTMASLIGHTS_MODE_BLINKTEST_H

#include "Mode.h"
#include "../control.h" // Include for access to FastLEDs, config, etc.

namespace lights
{
class BlinkTest : public Mode {
  public:
    void startup();
    void tick();
    void handleMessage(uint8_t *payload, size_t length);
};
} // namespace lights

#endif