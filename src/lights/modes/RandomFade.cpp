#include "RandomFade.h"
namespace lights {
  void RandomFade::startup() {
    RandomFade::isTwinkleOn = config::getConfig(config::SmallSetting::RF_TWINKLE_ON) > 0;
    RandomFade::colorBandSize = config::getConfig(config::SmallSetting::RF_BAND_SIZE) > 0 ? config::getConfig(config::SmallSetting::RF_BAND_SIZE) : 1;
    memset(hsvshift::hueshift,0,sizeof(hsvshift::hueshift)); // Clear hue shift for syncing
    RandomFade::prefillRandom();
  }

  void RandomFade::tick(){
    if (RandomFade::hueRotationSpeed > 0){
      for (int grp = 0; grp < LED_NUM_LEDS; grp+=RandomFade::colorBandSize) {
        //Randomly adjust hue shifts
        if (random8(100) == 0) {
          uint8_t newShift = random8(1, RandomFade::hueRotationSpeed) * (random8(2) ? 1 : -1); // Between 1 and 3 degrees per tick, and randomly forward or backward
          for (int i = grp; i < grp+RandomFade::colorBandSize && i < LED_NUM_LEDS; i++){
            hsvshift::hueshift[i] = newShift;
          }
        }
      }
    }

    // If twinkleOn, randomly twinkle
    if (RandomFade::isTwinkleOn && random16(RandomFade::twinkleRand) == 0){
      RandomFade::twinkle(random8(LED_NUM_LEDS));
    }

    // Make changes to LEDs
    hsvshift::applyShifts(leds);

    // Push to string!
    FastLED.show();
  }

  void RandomFade::handleMessage(uint8_t * payload, size_t length){
    if (length < 2 ) { return; }

    switch (payload[0]) {
    case config::SmallSetting::RF_TWINKLE_ON:
        RandomFade::changeTwinkle(payload[1] > 0, true);
        break;
    case config::SmallSetting::RF_BAND_SIZE:
        RandomFade::changeColorBandSize(payload[1], true);
        break;
    }
  }

  void RandomFade::prefillRandom() {
    for (int grp = 0; grp < LED_NUM_LEDS; grp+=RandomFade::colorBandSize) {
      uint8_t randomHue = random8();
      for (int i = grp; i<grp+RandomFade::colorBandSize && i < LED_NUM_LEDS; i++){
        hsvshift::hsv_leds[i].hue = randomHue;
        hsvshift::hsv_leds[i].sat = 255;
        hsvshift::hsv_leds[i].val = 255;
        lights::leds[i] = hsvshift::hsv_leds[i];

        // Also make sure no shifting is happening from another mode
        hsvshift::hueshift[i] = 0;
        hsvshift::valshift[i] = 0;
      }
    }
    #ifdef DEBUG_MSG
      Serial.print("Random fill complete");
    #endif
  }

  void RandomFade::twinkle(int t_led){
    hsvshift::hsv_leds[t_led].sat = 0;
    hsvshift::satshift[t_led] = 25;
  }

  void RandomFade::changeTwinkle(bool doTwinkle, bool setConfig) {
    #ifdef DEBUG_MSG
            Serial.printf("Setting twinkle to %d\n", doTwinkle);
        #endif
        RandomFade::isTwinkleOn = doTwinkle;
        if (setConfig) {
            config::setConfig(config::SmallSetting::RF_TWINKLE_ON, doTwinkle);
        }
  }

  void RandomFade::changeColorBandSize(uint8_t bandSize, bool setConfig) {
    #ifdef DEBUG_MSG
            Serial.printf("Setting bandSize to %d\n", bandSize);
        #endif
        
        RandomFade::colorBandSize = bandSize;
        memset(hsvshift::hueshift,0,sizeof(hsvshift::hueshift)); // Clear hue shift for syncing
        RandomFade::prefillRandom(); // Refill leds to sync colors

        if (setConfig) {
            config::setConfig(config::SmallSetting::RF_BAND_SIZE, bandSize);
        }
  }
}
