#ifndef CHRISTMASLIGHTS_MODE_RANDOMFADE_H
#define CHRISTMASLIGHTS_MODE_RANDOMFADE_H

#include "Mode.h"
#include "lights/control.h" // Include for access to FastLEDs, config, etc.
#include "util/hsv_shifting.h"
namespace lights
{
class RandomFade : public Mode
{
  public:
    void startup();
    void tick();
    void handleMessage(uint8_t *payload, size_t length);
  protected:
    void prefillRandom();
    void changeTwinkle(bool twinkleOn, bool writeConfig);
    void changeColorBandSize(uint8_t bandSize, bool writeConfig);
    void twinkle(int t_led);
    uint8_t hueRotationSpeed = 4;
    const int twinkleRand = 500/LED_NUM_LEDS; // Each LED should have about 1:twinkleRand chance of blinking
    bool isTwinkleOn = false;
    uint8_t colorBandSize = 1;
};
} // namespace lights

#endif