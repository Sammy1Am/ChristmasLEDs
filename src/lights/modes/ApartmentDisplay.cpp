#include "ApartmentDisplay.h"
namespace lights {

  void ApartmentDisplay::startup() {
    ApartmentDisplay::isTwinkleOn = config::getConfig(config::SmallSetting::XT_TWINKLE_ON) > 0;
    ApartmentDisplay::starSize = config::getConfig(config::SmallSetting::XT_STAR_SIZE) < LED_NUM_LEDS ? config::getConfig(config::SmallSetting::XT_STAR_SIZE) : 0;
    ApartmentDisplay::ornamentSpacing = config::getConfig(config::SmallSetting::XT_ORNAMENT_SPACING);
    memset(hsvshift::hueshift,0,sizeof(hsvshift::hueshift)); // Clear hue shift for syncing
    ApartmentDisplay::prefillHues();
  }

  void ApartmentDisplay::tick(){

    // Do appropriate ticks
    switch(ApartmentDisplay::currentStatus){
      case TreeStatus::IDLE:
        ApartmentDisplay::idleTick();
        break;
      case TreeStatus::STAR_RUN:
        ApartmentDisplay::starRunTick();
        break;
    }

    // Make changes to LEDs
    hsvshift::applyShifts(leds);

    // Push to string!
    FastLED.show();
  }

  void ApartmentDisplay::idleTick(){
    for (int l=0;l<LED_NUM_LEDS;l++) {
      if (l>=LED_NUM_LEDS-ApartmentDisplay::starSize) {
        // STAR
        // If twinkleOn, randomly twinkle
        if (ApartmentDisplay::isTwinkleOn && random16(60) == 0) {
          ApartmentDisplay::twinkle(l);
        }
      } else if (l<windowLightCount) {
          //WINDOW
          //Randomly adjust hue shifts, but only for even numbers (so we can do them in pairs)
          if (l%2==0 && random8(100) == 0) {
              uint8_t newShift = random8(1, ApartmentDisplay::hueRotationSpeed) * (random8(2) ? 1 : -1); // Between 1 and 3 degrees per tick, and randomly forward or backward
              hsvshift::hueshift[l] = newShift;
              hsvshift::hueshift[l+1] = newShift;
          }
          if (ApartmentDisplay::isTwinkleOn && random16(120) == 0) {
              ApartmentDisplay::twinkle(l);
          }
      }
      else if (ApartmentDisplay::ornamentSpacing > 1 && l % ApartmentDisplay::ornamentSpacing == 0)
      {
        // ORNAMENTS
        // For now, ornaments stay the same color
      } else {
        // TREE
        // Shift value and decrease shift amount
        if (hsvshift::valshift[l] > 0){
          hsvshift::hsv_leds[l].val = min(255,hsvshift::hsv_leds[l].val+hsvshift::valshift[l]);
          hsvshift::valshift[l]--;
        } else if (hsvshift::valshift[l] < 0){
          hsvshift::hsv_leds[l].val = max(128,hsvshift::hsv_leds[l].val+hsvshift::valshift[l]);
          hsvshift::valshift[l]++;
        } else {
          if (hsvshift::hsv_leds[l].val < 255){
            hsvshift::valshift[l] = random8(1, 3);
          } else {
            hsvshift::valshift[l] = 0 - random8(1, 6);
          }
        }
      }
    }
  }

  uint8_t aptStarRunPhase = 0;
  uint8_t aptStarPosition = 0;
  bool aptfadeDone = true;

  void ApartmentDisplay::starRunTick(){
    switch(aptStarRunPhase){
      case 0: // Fade the tree
        aptfadeDone = true;
        for (int l=0;l<LED_NUM_LEDS;l++) {
          // Fade the whole tree
          if (hsvshift::hsv_leds[l].val > 0) {
              hsvshift::valshift[l] = -6;
              aptfadeDone=false;
            } else {
              hsvshift::valshift[l] = 0;
            }
        }
        if (aptfadeDone) {
          aptStarRunPhase = 1; // Move on!
        }
        break;
      case 1: // Run star to stop
        if (aptStarPosition > LED_NUM_LEDS-ApartmentDisplay::starSize) {
          // We've reached the top of the tree!
          for (int l=LED_NUM_LEDS-ApartmentDisplay::starSize;l<LED_NUM_LEDS;l++) {
            ApartmentDisplay::twinkle(l);
          }
          aptStarRunPhase = 2;
          break;
        }
        for (int l=0;l<LED_NUM_LEDS;l++) {
          if (l>=aptStarPosition && l<aptStarPosition+ApartmentDisplay::starSize) {
            // STAR
            // Make star yellow
            hsvshift::hsv_leds[l] = CHSV(45,255,255);
            leds[l] = hsvshift::hsv_leds[l]; // We're not shifting these, so they need to be copied here
            //TODO, only worry about "head" pixel

            // Refill tree behind star
            if (l==aptStarPosition && aptStarPosition > 0) {
              ApartmentDisplay::prefillPixel(l-1);
              leds[l-1] = hsvshift::hsv_leds[l-1]; // We're not shifting these, so they need to be copied here
            }

            // Add sparkles (TODO: Make more closer to star?)
            if (random8(3)==0){
              ApartmentDisplay::twinkle((int)random16(aptStarPosition));
            }
          } 
          else if (ApartmentDisplay::ornamentSpacing > 1 && l % ApartmentDisplay::ornamentSpacing == 0)
          {
            // ORNAMENTS
            // For now, ornaments stay the same color
          } else if (l<LED_NUM_LEDS-ApartmentDisplay::starSize){
            // TREE (Minus Star)
            // Keep tree dark during star-run
          }
        }
        aptStarPosition++;
        break;
      case 2: // Sparkle explosion
        if (aptStarPosition < 3) { // Use <3 to avoid underflow issues
          aptStarRunPhase = 6;
          break;
        }
        for (int l=0;l<5;l++){
          ApartmentDisplay::twinkle(random16(aptStarPosition,LED_NUM_LEDS));
        }
        aptStarPosition-=3;
        break;
      case 6:
        // We're done!
        aptStarRunPhase = 0;
        currentStatus = TreeStatus::IDLE;
    }
  }

  void ApartmentDisplay::handleMessage(uint8_t * payload, size_t length){
    if (length < 2 ) { return; }

    switch (payload[0]) {
    case config::SmallSetting::XT_TWINKLE_ON:
      ApartmentDisplay::changeTwinkle(payload[1] > 0, true);
      break;
    case config::SmallSetting::XT_STAR_SIZE:
      ApartmentDisplay::changeStarSize(payload[1], true);
      break;
    case config::SmallSetting::XT_ORNAMENT_SPACING:
      ApartmentDisplay::changeOrnamentSpacing(payload[1], true);
      break;
    case 12:
      ApartmentDisplay::startActivity(0);
      break;
    }
  }

  void ApartmentDisplay::startActivity(uint8_t activityNum) {
    aptStarRunPhase = 0;
    ApartmentDisplay::currentStatus = TreeStatus::STAR_RUN;
  }

  void ApartmentDisplay::prefillHues() {
    for (int l=0;l<LED_NUM_LEDS;l++) {
      ApartmentDisplay::prefillPixel(l);
    }
    #ifdef DEBUG_MSG
      Serial.print("Hue fill complete\n");
    #endif
  }

  void ApartmentDisplay::prefillPixel(int pixelIndex) {
    if (pixelIndex>=LED_NUM_LEDS-ApartmentDisplay::starSize) {
        // STAR
        hsvshift::hsv_leds[pixelIndex] = CHSV(45,255,255);
      } 
      else if (pixelIndex < windowLightCount) {
        if (pixelIndex%2==0) {
            hsvshift::hsv_leds[pixelIndex].hue = random8();
            hsvshift::hsv_leds[pixelIndex].sat = 255;
            hsvshift::hsv_leds[pixelIndex].val = 255;
            lights::leds[pixelIndex] = hsvshift::hsv_leds[pixelIndex];

            hsvshift::hsv_leds[pixelIndex + 1].hue = hsvshift::hsv_leds[pixelIndex].hue; // Set matching pairs to the same hue
            hsvshift::hsv_leds[pixelIndex+1].sat = 255;
            hsvshift::hsv_leds[pixelIndex+1].val = 255;
            lights::leds[pixelIndex+1] = hsvshift::hsv_leds[pixelIndex+1];
        }
      }
      else if (ApartmentDisplay::ornamentSpacing > 1 && pixelIndex % ApartmentDisplay::ornamentSpacing == 0)
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
        //hsvshift::hsv_leds[pixelIndex] = CHSV(HSVHue::HUE_GREEN,255,255);
        hsvshift::hsv_leds[pixelIndex] = CHSV(45, 45, 255);
      }
  }

  void ApartmentDisplay::twinkle(int t_led){
    hsvshift::hsv_leds[t_led].sat = 0;
    hsvshift::satshift[t_led] = 25;
  }

  void ApartmentDisplay::changeTwinkle(bool doTwinkle, bool setConfig) {
    #ifdef DEBUG_MSG
            Serial.printf("Setting twinkle to %d\n", doTwinkle);
        #endif
        ApartmentDisplay::isTwinkleOn = doTwinkle;
        if (setConfig) {
            config::setConfig(config::SmallSetting::XT_TWINKLE_ON, doTwinkle);
        }
  }

  void ApartmentDisplay::changeStarSize(uint8_t starSize, bool setConfig) {
    #ifdef DEBUG_MSG
            Serial.printf("Setting starSize to %d\n", starSize);
        #endif
        
        ApartmentDisplay::starSize = starSize;
        memset(hsvshift::hueshift,0,sizeof(hsvshift::hueshift)); // Clear hue shift for syncing
        ApartmentDisplay::prefillHues(); // Refill leds to sync colors

        if (setConfig) {
            config::setConfig(config::SmallSetting::XT_STAR_SIZE, starSize);
        }
  }

  void ApartmentDisplay::changeOrnamentSpacing(uint8_t ornamentSpacing, bool setConfig) {
    #ifdef DEBUG_MSG
            Serial.printf("Setting ornamentSpacing to %d\n", ornamentSpacing);
        #endif
        
        ApartmentDisplay::ornamentSpacing = ornamentSpacing;
        memset(hsvshift::hueshift,0,sizeof(hsvshift::hueshift)); // Clear hue shift for syncing
        ApartmentDisplay::prefillHues(); // Refill leds to sync colors

        if (setConfig) {
            config::setConfig(config::SmallSetting::XT_ORNAMENT_SPACING, ornamentSpacing);
        }
  }
}
