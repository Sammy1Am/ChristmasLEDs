#include "hsv_shifting.h"

namespace hsvshift {

  CHSV hsv_leds[LED_NUM_LEDS]; // CHSV array for easy blending and hueshifting
  int8_t hueshift[LED_NUM_LEDS]; // Amount to shift each led's hue value every tick
  int8_t satshift[LED_NUM_LEDS]; // Amount to shift each led's saturation value every tick
  int8_t valshift[LED_NUM_LEDS]; // Amount to shift each led's value value every tick

  void applyShifts(CRGB leds[LED_NUM_LEDS]) {
    for (int l = 0; l < LED_NUM_LEDS; l++) {

      bool change = false; // Flag set to true if a chance happened for this LED

      if (hueshift[l] != 0){
        hsv_leds[l].hue += hueshift[l]; // Shift hue (overflow is fine here since it's a circle)
        change = true;
      }

      if (satshift[l] < 0){
        if (hsv_leds[l].sat <= abs(satshift[l])){
          satshift[l] = 0;
          hsv_leds[l].sat = 0;
        } else {
          hsv_leds[l].sat += satshift[l];
        }
        change = true;
      } else if (satshift[l] > 0){
        if (255 - hsv_leds[l].sat < satshift[l]) {
          satshift[l] = 0;
          hsv_leds[l].sat = 255;
        } else {
          hsv_leds[l].sat += satshift[l];
        }
        change = true;
      }

      if (valshift[l] < 0){
        if (hsv_leds[l].val <= abs(valshift[l])){
          valshift[l] = 0;
          hsv_leds[l].val = 0;
        } else {
          hsv_leds[l].val += valshift[l];
        }
        change = true;
      } else if (valshift[l] > 0){
        if (255 - hsv_leds[l].val < valshift[l]) {
          valshift[l] = 0;
          hsv_leds[l].val = 255;
        } else {
          hsv_leds[l].val += valshift[l];
        }
        change = true;
      }

      // Only do the conversion math once (it's fast, but we can cut down on it)
      if (change){
        leds[l] = hsv_leds[l];
      }
    }
  }
}
