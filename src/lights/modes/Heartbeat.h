#ifndef CHRISTMASLIGHTS_MODE_HEARTBEAT_H
#define CHRISTMASLIGHTS_MODE_HEARTBEAT_H

#include "Mode.h"
#include "lights/control.h" // Include for access to FastLEDs, config, etc.
#include "util/hsv_shifting.h"
namespace lights
{

  #define TICK_MICROS 50000

  class Heartbeat : public Mode
  {
    public:
      void startup();
      void tick();
      void handleMessage(uint8_t *payload, size_t length);
    protected:
      unsigned int tickCount = 0;
      unsigned int lubTicks = TICK_MICROS/2500;
      unsigned int dubTicks = lubTicks * 1.15;
      void setBPM(uint8_t newBPM, bool setConfig);
  };
} // namespace lights

#endif