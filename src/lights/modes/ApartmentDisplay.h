#ifndef CHRISTMASLIGHTS_MODE_APARTMENTDISPLAY_H
#define CHRISTMASLIGHTS_MODE_APARTMENTDISPLAY_H

#include "Mode.h"
#include "lights/control.h" // Include for access to FastLEDs, config, etc.
#include "util/hsv_shifting.h"
#include <Ticker.h>
namespace lights
{
  /**
   * A mostly-custom display where the light string starts around the window, and then goes
   * onto a small tree.
   */
class ApartmentDisplay : public Mode
{
  public:
    void startup();
    void tick();
    void handleMessage(uint8_t *payload, size_t length);
    void startActivity(uint8_t activityNum);
    
  protected:
    enum TreeStatus {IDLE,STAR_RUN};
    TreeStatus currentStatus = IDLE;

    void prefillHues();
    void prefillPixel(int pixelIndex);

    void idleTick();
    void starRunTick();
    void startStarRun();

    void changeTwinkle(bool twinkleOn, bool writeConfig);
    void changeOrnamentSpacing(uint8_t bandSize, bool writeConfig);
    void changeStarSize(uint8_t starSize, bool writeConfig);
    void twinkle(int t_led);
    uint8_t hueRotationSpeed = 4;
    const int twinkleRand = 500/LED_NUM_LEDS; // Each LED should have about 1:twinkleRand chance of blinking
    const int windowLightCount = 90;
    bool isTwinkleOn = false;
    uint8_t starSize = 6;
    uint8_t ornamentSpacing = 0; // 0 or 1 disables ornaments
};
} // namespace lights

#endif