#include "Heartbeat.h"
namespace lights {
  void Heartbeat::startup() {
    FastLED.clear(true); // Clear all lights
    for (int i=0;i<LED_NUM_LEDS;i++){
          hsvshift::hsv_leds[i] = CHSV(HSVHue::HUE_RED,255,0); // Red, fully saturated, zero value
          hsvshift::hueshift[i] = 0;
          hsvshift::valshift[i] = 0;
        }
  }

  void lub(){
    for (int i = NUM_LEDS_GARAGE+NUM_LEDS_BEDROOMS; i < LED_NUM_LEDS; i++){
      hsvshift::hsv_leds[i].val = 255;
      hsvshift::valshift[i] = -12;
      leds[i] = hsvshift::hsv_leds[i];
    }
    #ifdef DEBUG_MSG
      Serial.write("lub");
    #endif
  }

  void dub(){
    for (int i = 0; i < NUM_LEDS_GARAGE+NUM_LEDS_BEDROOMS; i++){
      hsvshift::hsv_leds[i].val = 255;
      hsvshift::valshift[i] = -12;
      leds[i] = hsvshift::hsv_leds[i];
    }
    #ifdef DEBUG_MSG
      Serial.write("dub");
    #endif
  }

  void Heartbeat::tick() {

    if (Heartbeat::tickCount == Heartbeat::lubTicks){
      lub();
    }

    if (Heartbeat::tickCount > Heartbeat::dubTicks){
      dub();
      Heartbeat::tickCount = 0;
    }

    // Make changes to LEDs
    hsvshift::applyShifts(leds);

    // Push to string!
    FastLED.show();

    // Increment tickCount
    Heartbeat::tickCount++;
  }

  void Heartbeat::handleMessage(uint8_t *payload, size_t length){
    switch (payload[0]){
      case config::SmallSetting::HB_BPM:
        if (payload[1] > 0){ // Don't set heartrate to zero!
          Heartbeat::lubTicks = (60000000 / TICK_MICROS) / (int)payload[1]; // BPM to tick-period
          Heartbeat::dubTicks = lubTicks * 1.15;
        }
        break;
    }
  }
}
