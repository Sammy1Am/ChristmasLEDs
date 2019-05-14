#include <Arduino.h>
#include <Ticker.h>
#include "net/network.h"
#include "net/xmas_server.h"
#include "lights/control.h"

// ESP8266 has confusing hardware timers, so we're using this software timer instead
Ticker ledTicker = Ticker(lights::controlTick, TICK_MILLIS, 0, MILLIS);
Ticker activityTicker = Ticker(lights::controlStartActivity, ACTIVITY_MILLIS, 0, MILLIS);

void setup() {
  // Start the Serial communication to send messages to the computer
  Serial.begin(115200);         
  delay(10);
  Serial.println('\n');

  // Setup various components
  net::wifiSetup();
  net::otaSetup();
  
  net::serverSetup(lights::handleMessage);

  lights::controlSetup();

  ledTicker.start(); // Start the timer after everything else is setup
  activityTicker.start();
}

void loop() {
  // Loop components that need looping
  net::otaLoop();
  ledTicker.update(); // Poke the timer to see if a tick needs to happen
  activityTicker.update();
  yield();
}