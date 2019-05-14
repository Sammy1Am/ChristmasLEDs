#include "Off.h"
namespace lights {
void Off::startup() {
  FastLED.clear(true); // Clear all lights
}

void Off::tick() {
  // Do nothing
}

void Off::handleMessage(uint8_t * payload, size_t length){
  // Process nothing
}
}
