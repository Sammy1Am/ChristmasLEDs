#include "XmasTree.h"
namespace lights {

  void XmasTree::startup() {
    XmasTree::isTwinkleOn = config::getConfig(config::SmallSetting::XT_TWINKLE_ON) > 0;
    XmasTree::starSize = config::getConfig(config::SmallSetting::XT_STAR_SIZE) < LED_NUM_LEDS ? config::getConfig(config::SmallSetting::XT_STAR_SIZE) : 0;
    XmasTree::ornamentSpacing = config::getConfig(config::SmallSetting::XT_ORNAMENT_SPACING);
    memset(hsvshift::hueshift,0,sizeof(hsvshift::hueshift)); // Clear hue shift for syncing
    XmasTree::prefillHues();
  }

  void XmasTree::tick(){

    // Do appropriate ticks
    switch(XmasTree::currentStatus){
      case TreeStatus::IDLE:
        XmasTree::idleTick();
        break;
      case TreeStatus::STAR_RUN:
        XmasTree::starRunTick();
        break;
    }

    // Make changes to LEDs
    hsvshift::applyShifts(leds);

    // Push to string!
    FastLED.show();
  }

  void XmasTree::idleTick(){
    for (int l=0;l<LED_NUM_LEDS;l++) {
      if (l>=LED_NUM_LEDS-XmasTree::starSize) {
        // STAR
        // If twinkleOn, randomly twinkle
        if (XmasTree::isTwinkleOn && random16(60) == 0) {
          XmasTree::twinkle(l);
        }
      }
      else if (XmasTree::ornamentSpacing > 1 && l % XmasTree::ornamentSpacing == 0)
      {
        // ORNAMENTS
        // For now, ornaments stay the same color
        
        // If twinkleOn, randomly twinkle
        if (XmasTree::isTwinkleOn && random16(320) == 0) {
            XmasTree::twinkle(l);
        }
      } else {
        // TREE
        // Shift value and decrease shift amount
        if (hsvshift::valshift[l] > 0){
          hsvshift::hsv_leds[l].val = min(255,hsvshift::hsv_leds[l].val+hsvshift::valshift[l]);
          hsvshift::valshift[l]--;
        } else if (hsvshift::valshift[l] < 0){
          hsvshift::hsv_leds[l].val = max(0,hsvshift::hsv_leds[l].val+hsvshift::valshift[l]);
          hsvshift::valshift[l]++;
        } else if (random16(8) == 0) {
            if (hsvshift::hsv_leds[l].val < 255) {
                hsvshift::valshift[l] = random8(1, 6);
            } else {
                hsvshift::valshift[l] = 0 - random8(1, 12);
            }
        }
      }
    }
  }

  uint8_t starRunPhase = 0;
  uint8_t starPosition = 0;
  bool fadeDone = true;

  void XmasTree::starRunTick(){
    switch(starRunPhase){
      case 0: // Fade the tree
        fadeDone = true;
        for (int l=0;l<LED_NUM_LEDS;l++) {
          // Fade the whole tree
          if (hsvshift::hsv_leds[l].val > 0) {
              hsvshift::valshift[l] = -6;
              fadeDone=false;
            } else {
              hsvshift::valshift[l] = 0;
            }
        }
        if (fadeDone) {
          starRunPhase = 1; // Move on!
        }
        break;
      case 1: // Run star to stop
        if (starPosition > LED_NUM_LEDS-XmasTree::starSize) {
          // We've reached the top of the tree!
          for (int l=LED_NUM_LEDS-XmasTree::starSize;l<LED_NUM_LEDS;l++) {
            XmasTree::twinkle(l);
          }
          starRunPhase = 2;
          break;
        }
        for (int l=0;l<LED_NUM_LEDS;l++) {
          if (l>=starPosition && l<starPosition+XmasTree::starSize) {
            // STAR
            // Make star yellow
            hsvshift::hsv_leds[l] = CHSV(50,255,255);
            leds[l] = hsvshift::hsv_leds[l]; // We're not shifting these, so they need to be copied here
            //TODO, only worry about "head" pixel

            // Refill tree behind star
            if (l==starPosition && starPosition > 0) {
              XmasTree::prefillPixel(l-1);
              leds[l-1] = hsvshift::hsv_leds[l-1]; // We're not shifting these, so they need to be copied here
            }

            // Add sparkles (TODO: Make more closer to star?)
            if (random8(3)==0){
              XmasTree::twinkle((int)random16(starPosition));
            }
          } 
          else if (XmasTree::ornamentSpacing > 1 && l % XmasTree::ornamentSpacing == 0)
          {
            // ORNAMENTS
            // For now, ornaments stay the same color
          } else if (l<LED_NUM_LEDS-XmasTree::starSize){
            // TREE (Minus Star)
            // Keep tree dark during star-run
          }
        }
        starPosition++;
        break;
      case 2: // Sparkle explosion
        if (starPosition == 0) {
          starRunPhase = 6;
          break;
        }
        for (int l=0;l<5;l++){
          XmasTree::twinkle(random16(starPosition,LED_NUM_LEDS));
        }
        starPosition = starPosition > 2 ? starPosition - 3 : 0;
        break;
      case 6:
        // We're done!
        starRunPhase = 0;
        currentStatus = TreeStatus::IDLE;
    }
  }

  void XmasTree::handleMessage(uint8_t * payload, size_t length){
    if (length < 2 ) { return; }

    switch (payload[0]) {
    case config::SmallSetting::XT_TWINKLE_ON:
      XmasTree::changeTwinkle(payload[1] > 0, true);
      break;
    case config::SmallSetting::XT_STAR_SIZE:
      XmasTree::changeStarSize(payload[1], true);
      break;
    case config::SmallSetting::XT_ORNAMENT_SPACING:
      XmasTree::changeOrnamentSpacing(payload[1], true);
      break;
    case 12:
      XmasTree::startActivity(0);
      break;
    }
  }

  void XmasTree::startActivity(uint8_t activityNum) {
    starRunPhase = 0;
    XmasTree::currentStatus = TreeStatus::STAR_RUN;
  }

  void XmasTree::prefillHues() {
    for (int l=0;l<LED_NUM_LEDS;l++) {
      XmasTree::prefillPixel(l);
    }
    #ifdef DEBUG_MSG
      Serial.print("Hue fill complete\n");
    #endif
  }

  void XmasTree::prefillPixel(int pixelIndex) {
    if (pixelIndex>=LED_NUM_LEDS-XmasTree::starSize) {
        // STAR
        hsvshift::hsv_leds[pixelIndex] = CHSV(50,255,255);
      } 
      else if (XmasTree::ornamentSpacing > 1 && pixelIndex % XmasTree::ornamentSpacing == 0)
      {
        // ORNAMENTS
        hsvshift::hsv_leds[pixelIndex].hue = random8();
        hsvshift::hsv_leds[pixelIndex].sat = 255;
        hsvshift::hsv_leds[pixelIndex].val = 255;
        lights::leds[pixelIndex] = hsvshift::hsv_leds[pixelIndex];

        // Also make sure no shifting is happening from another mode
        hsvshift::hueshift[pixelIndex] = 0;
        hsvshift::valshift[pixelIndex] = 0;
      } else {
        // TREE
        hsvshift::hsv_leds[pixelIndex] = CHSV(HSVHue::HUE_GREEN,255,255);
      }
  }

  void XmasTree::twinkle(int t_led){
    hsvshift::hsv_leds[t_led].sat = 0;
    hsvshift::satshift[t_led] = 25;
  }

  void XmasTree::changeTwinkle(bool doTwinkle, bool setConfig) {
    #ifdef DEBUG_MSG
            Serial.printf("Setting twinkle to %d\n", doTwinkle);
        #endif
        XmasTree::isTwinkleOn = doTwinkle;
        if (setConfig) {
            config::setConfig(config::SmallSetting::XT_TWINKLE_ON, doTwinkle);
        }
  }

  void XmasTree::changeStarSize(uint8_t starSize, bool setConfig) {
    #ifdef DEBUG_MSG
            Serial.printf("Setting starSize to %d\n", starSize);
        #endif
        
        XmasTree::starSize = starSize;
        memset(hsvshift::hueshift,0,sizeof(hsvshift::hueshift)); // Clear hue shift for syncing
        XmasTree::prefillHues(); // Refill leds to sync colors

        if (setConfig) {
            config::setConfig(config::SmallSetting::XT_STAR_SIZE, starSize);
        }
  }

  void XmasTree::changeOrnamentSpacing(uint8_t ornamentSpacing, bool setConfig) {
    #ifdef DEBUG_MSG
            Serial.printf("Setting ornamentSpacing to %d\n", ornamentSpacing);
        #endif
        
        XmasTree::ornamentSpacing = ornamentSpacing;
        memset(hsvshift::hueshift,0,sizeof(hsvshift::hueshift)); // Clear hue shift for syncing
        XmasTree::prefillHues(); // Refill leds to sync colors

        if (setConfig) {
            config::setConfig(config::SmallSetting::XT_ORNAMENT_SPACING, ornamentSpacing);
        }
  }
}
