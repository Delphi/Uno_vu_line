/*
 * Standby: Twinkling lights of random colours
 */

void twinkle() {
  delay(10);
  
  if (random(25) == 1) {
    uint16_t i = random(N_PIXELS);
    ledsLeft[i] = CRGB(random(256), random(256), random(256));
  }
  fadeToBlackBy(ledsLeft, N_PIXELS, FADE_RATE);

  
  #ifdef TWO_STRIPS
    if (random(25) == 1) {
      uint16_t i = random(N_PIXELS);
      ledsRight[i] = CRGB(random(256), random(256), random(256));
    }
    fadeToBlackBy(ledsRight, N_PIXELS, FADE_RATE);
  #endif
  
  //FastLED.show();
  LEDShowEnable=true;
  //delay(10); //moved to top.
}
