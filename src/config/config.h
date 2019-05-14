#include <Arduino.h>
#include <EEPROM.h>

// FastLED Settings
#define LED_DATA_PIN 13
#define LED_NUM_LEDS 200

#define TICK_MILLIS 50 // How often to tick the light controller
#define ACTIVITY_MILLIS 300000 // How often to start an activity

#define DEBUG_MSG // Leave uncommented to print debug statements to serial

#ifndef CHRISTMASLIGHTS_CONFIG_H
#define CHRISTMASLIGHTS_CONFIG_H

namespace config {
    // Enum that represents the memory location of various single-byte settings.  These values are
    // also the "command" bytes in the message to indicate what they'll be setting (0-127 are reserved for 
    // non-memory commands, and that memory is reserved for other settings)
    enum SmallSetting {
        MODE = 128, // 1 byte
        GLOBAL_BRIGHTNESS = 129, // 1 byte

        // These settings are for RandomFade
        RF_TWINKLE_ON = 130,
        RF_BAND_SIZE = 131,

        // These settings are for XmasTree
        XT_TWINKLE_ON = 132,
        XT_STAR_SIZE = 133,
        XT_ORNAMENT_SPACING = 134,

        // These settings are for Heartbeat
        HB_BPM = 135,
    };

    // Enum that represents the memory location of various 4-byte settings
    enum LargeSetting {
        //NUM_LEDS = 0x2, // 4 bytes
    };

    void setup();
    void setConfig(SmallSetting setting, uint8_t value);
    uint8_t getConfig(SmallSetting setting);
    void setConfig(LargeSetting setting, uint32_t value);
    uint32_t getConfig(LargeSetting setting);
}

#endif