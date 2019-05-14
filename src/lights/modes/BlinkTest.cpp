#include "BlinkTest.h"
namespace lights {
  bool blinkOn = true;
  int blinkTick = 0;
  void BlinkTest::startup() {
    FastLED.clear(true); // Clear all lights
    FastLED.showColor(CRGB::Blue);
  }

  void BlinkTest::tick() {
    if (blinkTick++ > 100) {
      if (blinkOn=!blinkOn) {
        FastLED.showColor(CRGB::Orange);
      } else {
        FastLED.clear(true);
      }
      blinkTick = 0;
    }
    // Do nothing
  }

  void BlinkTest::handleMessage(uint8_t * payload, size_t length){
    // Process nothing
  }
}
