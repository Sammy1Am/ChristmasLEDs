#include <Arduino.h>

#define FASTLED_ALLOW_INTERRUPTS 0 // Do this to avoid glitching on ESP8266

#include <FastLED.h>
#include "config/config.h"
#include "modes.h"

#ifndef CHRISTMASLIGHTS_CONTROL_H
#define CHRISTMASLIGHTS_CONTROL_H

namespace lights {

    extern CRGB leds[LED_NUM_LEDS]; // Actual FastLED array

    void controlSetup();
    void controlTick();
    void controlStartActivity();

    void handleMessage(uint8_t * payload, size_t length);

    void changeMode(uint8_t mode, bool writeConfig);
    void changeBrightness(uint8_t brightness, bool writeConfig);
}

#endif