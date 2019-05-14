#ifndef CHRISTMASLIGHTS_MODE_AMERICANFLAG_H
#define CHRISTMASLIGHTS_MODE_AMERICANFLAG_H

#include "Mode.h"
#include "lights/control.h" // Include for access to FastLEDs, config, etc.
#include "util/hsv_shifting.h"
namespace lights
{
  #define FLAG_STRIPE_WIDTH 6.0
  #define NUM_LEDS_GARAGE 40
  #define NUM_LEDS_BEDROOMS 50
  #define NUM_LEDS_LIVINGROOM 38
  class AmericanFlag : public Mode
  {
    public:
      void startup();
      void tick();
      void handleMessage(uint8_t *payload, size_t length);
    protected:
      void initFlag();
      void doStripes();
      void twinkle(uint8_t t_led);
      uint8_t flagWaveSpeed;
      float flagWaveValue = 0.0;
      bool waveUp = true;
      const int twinkleRand = 100/NUM_LEDS_BEDROOMS; // Each LED should have about 1:twinkleRand chance of blinking
  };
} // namespace lights

#endif