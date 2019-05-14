#include "control.h"

namespace lights {

    CRGB leds[LED_NUM_LEDS]; // Actual FastLED array

    Mode *modes[NUM_MODES] = {new Off(), new BlinkTest(), new RandomFade(), new XmasTree(), new AmericanFlag(), new Heartbeat()}; // Array of mode options
    Mode *currentMode = modes[0]; // Start current mode as "Off"

    void controlSetup() {
        config::setup();

        FastLED.addLeds<WS2811, LED_DATA_PIN, RGB>(leds, LED_NUM_LEDS);

        changeBrightness(config::getConfig(config::SmallSetting::GLOBAL_BRIGHTNESS), false);
        changeMode(config::getConfig(config::SmallSetting::MODE), false);
    }

    void controlTick() {
        currentMode->tick();
    }

    void controlStartActivity() {
        currentMode->startActivity(0);
    }

    void handleMessage(uint8_t * payload, size_t length) {
        if (length < 2 ) { return; }

        switch (payload[0]) {
        case config::SmallSetting::MODE:
            changeMode(payload[1], true);
            break;
        case config::SmallSetting::GLOBAL_BRIGHTNESS:
            changeBrightness(payload[1], true);
            break;
        default:
            // If this message wasn't handled by control, pass it on to the mode
            currentMode->handleMessage(payload, length);
        }
    }

    void changeMode(uint8_t mode, bool setConfig) {
        if (mode < NUM_MODES) {
            #ifdef DEBUG_MSG
                Serial.printf("Changing to mode %d\n", mode);
            #endif

            currentMode = modes[0]; // Switch mode to Off so ticks don't do anything
            modes[mode]->startup(); // Startup the new mode
            currentMode = modes[mode]; // Switch current mode to begin ticking

            if (setConfig) {
                config::setConfig(config::SmallSetting::MODE, mode);
            }
        } else {
            #ifdef DEBUG_MSG
                Serial.printf("Mode %d is invalid!\n", mode);
            #endif
        }
    }

    void changeBrightness(uint8_t brightness, bool setConfig) {
        #ifdef DEBUG_MSG
            Serial.printf("Setting global brightness to %d\n", brightness);
        #endif
        FastLED.setBrightness(brightness);
        if (setConfig) {
            config::setConfig(config::SmallSetting::GLOBAL_BRIGHTNESS, brightness);
        }
    }
}