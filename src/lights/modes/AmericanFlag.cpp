#include "AmericanFlag.h"

namespace lights {

  void AmericanFlag::doStripes() {
  for (int pixel = 0; pixel < NUM_LEDS_GARAGE; pixel++){
    hsvshift::hsv_leds[pixel].saturation = min(255.0,max(0.0,(sin(((float)pixel+AmericanFlag::flagWaveValue)/2.0f) + 0.8f)*255.0f));
    leds[pixel] = hsvshift::hsv_leds[pixel];
  }
  for (int pixel = NUM_LEDS_GARAGE + NUM_LEDS_BEDROOMS; pixel < LED_NUM_LEDS; pixel++){
    hsvshift::hsv_leds[pixel].saturation = min(255.0,max(0.0,(sin(((float)pixel+AmericanFlag::flagWaveValue)/2.0f) + 0.8f)*255.0f));
    leds[pixel] = hsvshift::hsv_leds[pixel];
  }
}

void AmericanFlag::initFlag() {
  for (int pixel = 0; pixel < LED_NUM_LEDS; pixel++) {
    if (pixel < NUM_LEDS_GARAGE || pixel >= NUM_LEDS_GARAGE + NUM_LEDS_BEDROOMS) {
      hsvshift::hsv_leds[pixel] = CHSV(HSVHue::HUE_RED,0,255); // Fill red-as-white to start
    } else {
      hsvshift::hsv_leds[pixel] = CHSV(HSVHue::HUE_BLUE,255,255);
    }

    leds[pixel] = hsvshift::hsv_leds[pixel];

    // Also make sure no shifting is happening from another mode
    hsvshift::hueshift[pixel] = 0;
    hsvshift::valshift[pixel] = 0;
  }

  AmericanFlag::doStripes(); // Set stripe saturations

    #ifdef DEBUG_MSG
      Serial.print("Flag initialized");
    #endif
  }

  void AmericanFlag::twinkle(uint8_t t_led){
    hsvshift::hsv_leds[t_led].sat = 0;
    hsvshift::satshift[t_led] = 25;
  }

  void AmericanFlag::startup(){
    FastLED.clear(true); // Clear all lights
    AmericanFlag::initFlag();
  }

  void AmericanFlag::tick(){
    AmericanFlag::flagWaveValue += (random8(100) * 0.002) * (waveUp ? 1 : -1);
    if (AmericanFlag::flagWaveValue > 2){
      AmericanFlag::waveUp = false;
    } else if (AmericanFlag::flagWaveValue < -2) {
      AmericanFlag::waveUp = true;
    }

    doStripes();


    // Randomly twinkle only in blue
    if (random16(AmericanFlag::twinkleRand) == 0){
      AmericanFlag::twinkle(random8(NUM_LEDS_GARAGE, NUM_LEDS_GARAGE+NUM_LEDS_BEDROOMS)); // Only twinkle on bedrooms
    }

    // Make changes to LEDs
    hsvshift::applyShifts(leds);

    // Push to string!
    FastLED.show();
  }

  void AmericanFlag::handleMessage(uint8_t *payload, size_t length){
    // America needs no processing!
  }
}
