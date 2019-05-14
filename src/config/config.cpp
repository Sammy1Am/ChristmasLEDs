#include "config.h"

namespace config {
        void setup() {
            EEPROM.begin(256);
        }

        void setConfig(SmallSetting setting, uint8_t value) {
            EEPROM.write(setting, value);
            EEPROM.commit();
        }

        uint8_t getConfig(SmallSetting setting) {
            return EEPROM.read(setting);
        }

        void setConfig(LargeSetting setting, uint32_t value) {
            for (size_t i = 0;i<sizeof(uint32_t);i++){
                EEPROM.write(setting, value >> (24-(i*8)));  // ONLY WORKS FOR 4-byte values
            }
            EEPROM.commit();
        }

        uint32_t getConfig(LargeSetting setting) {
            return EEPROM.read(setting) << 24
                | EEPROM.read(setting+1) << 16
                | EEPROM.read(setting+2) << 8
                | EEPROM.read(setting+3);
        }
    }