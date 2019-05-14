#include <FastLED.h>
#include "config/config.h"

#ifndef HSV_SHIFTING_H
#define HSV_SHIFTING_H

namespace hsvshift {

    extern CHSV hsv_leds[LED_NUM_LEDS]; // CHSV array for easy blending and hueshifting
    extern int8_t hueshift[LED_NUM_LEDS]; // Amount to shift each led's hue value every tick
    extern int8_t satshift[LED_NUM_LEDS]; // Amount to shift each led's saturation value every tick
    extern int8_t valshift[LED_NUM_LEDS]; // Amount to shift each led's value value every tick

    void applyShifts(CRGB leds[LED_NUM_LEDS]); 
}

#endif
