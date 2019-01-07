// --------------------------------
// -- VU Meter - Scott's version --
// --------------------------------

#include <FastLED.h>
#include <EEPROM.h>
#include "Button.h"

# define LEFT_OUT_PIN 6         // Left channel data out pin to LEDs
# define RIGHT_OUT_PIN 5        // Right channel data out pin to LEDs
# define LEFT_IN_PIN A5         // Left aux in signal
# define RIGHT_IN_PIN A4        // Right aux in signal
# define BTN_PIN 3              // Push button on this pin
# define DEBOUNCE_MS 20         // Number of ms to debounce the button 20 is default
# define LONG_PRESS 500         // Number of ms to hold the button to count as long press
# define N_PIXELS 18            // Number of pixels in each string
# define COLOR_ORDER GRB        // Try mixing up the letters (RGB, GBR, BRG, etc) for a whole new world of color combinations
# define BRIGHTNESS 200         // 0-255, higher number is brighter.
# define LED_TYPE WS2812B       // Probably WS2812B
# define MAX_LED_POWER 2000     // Limit power consumption of the LED strip in milliAmps. 
# define DC_OFFSET 0            // DC offset in aux signal - if unusure, leave 0
# define NOISE 10               // Noise/hum/interference in aux signal
# define SAMPLES 32             // Length of buffer for dynamic level adjustment
# define TOP (N_PIXELS + 2)     // Allow dot to go slightly off scale
# define PEAK_FALL 20           // Rate of peak falling dot
# define N_PIXELS_HALF (N_PIXELS / 2)
# define PATTERN_TIME 10         // Seconds to show eaach pattern on auto
# define TWO_STRIPS             // Comment out this line if only using one strip attached to the left channel
//# define TWO_STRIPS_REVERSE      // Comment out this line if 2 strips are attached but you wish to mirror the left channel on the right.

bool LEDShowEnable = false;     //Use by LED pattens to enable/disable LED updates.

uint8_t volCountLeft = 0;       // Frame counter for storing past volume data
int volLeft[SAMPLES];           // Collection of prior volume samples
int lvlLeft = 10;               // Current "dampened" audio level
int minLvlAvgLeft = 0;          // For dynamic adjustment of graph low & high
int maxLvlAvgLeft = 512;

uint8_t volCountRight = 0;      // Frame counter for storing past volume data
int volRight[SAMPLES];          // Collection of prior volume samples
int lvlRight = 10;              // Current "dampened" audio level
int minLvlAvgRight = 0;         // For dynamic adjustment of graph low & high
int maxLvlAvgRight = 512;

CRGB ledsLeft[N_PIXELS];
#ifdef TWO_STRIPS
  CRGB ledsRight[N_PIXELS];
#elif TWO_STRIPS_REVERSE
  CRGB ledsLeftReverse[N_PIXELS];
#endif

uint8_t myhue = 0;

void vu4(bool is_centered, uint8_t channel);
void vu5(bool is_centered, uint8_t channel);
void vu6(bool is_centered, uint8_t channel);
void vu7(bool show_background);
void vu8();
void vu9();
void vu10();
void balls();
void fire();
void juggle();
void ripple(boolean show_background);
void sinelon();
void twinkle();
void rainbow(uint8_t rate);

// --------------------
// --- Button Stuff ---
// --------------------
uint8_t state = 0;
int buttonPushCounter = 0;
bool autoChangeVisuals = false;
Button modeBtn(BTN_PIN, DEBOUNCE_MS);


void incrementButtonPushCounter() {
  buttonPushCounter = ++buttonPushCounter %17;
  EEPROM.write(0, buttonPushCounter);
}

void setup() {
  delay(1000); // power-up safety delay

  FastLED.addLeds < LED_TYPE, LEFT_OUT_PIN, COLOR_ORDER > (ledsLeft, N_PIXELS).setCorrection(TypicalLEDStrip);
  #ifdef TWO_STRIPS
    FastLED.addLeds < LED_TYPE, RIGHT_OUT_PIN, COLOR_ORDER > (ledsRight, N_PIXELS).setCorrection(TypicalLEDStrip);
  #elif TWO_STRIPS_REVERSE
    FastLED.addLeds < LED_TYPE, RIGHT_OUT_PIN, COLOR_ORDER > (ledsLeftReverse, N_PIXELS).setCorrection(TypicalLEDStrip);
  #endif
  FastLED.setBrightness(BRIGHTNESS);

  Serial.begin(57600);

  buttonPushCounter = (int)EEPROM.read(0); // load previous setting
  Serial.print("Starting pattern ");
  Serial.println(buttonPushCounter);
}

void loop() {
  
  // Read button
  modeBtn.read(); 
  switch (state) {
    case 0:                
      if (modeBtn.wasReleased()) {
        Serial.print("Short press, pattern ");
        Serial.println(buttonPushCounter);
        incrementButtonPushCounter();
        autoChangeVisuals = false;
      }
      else if (modeBtn.pressedFor(LONG_PRESS))
        state = 1;
      break;
    
    case 1:
      if (modeBtn.wasReleased()) {
        state = 0;
        Serial.print("Long press, auto, pattern ");
        Serial.println(buttonPushCounter);
        autoChangeVisuals = true;
      }
      break;
  }
  
  // Switch pattern if on auto
  if(autoChangeVisuals){
    EVERY_N_SECONDS(PATTERN_TIME) { 
      incrementButtonPushCounter();
      Serial.print("Auto, pattern ");
      Serial.println(buttonPushCounter); 
    }
  }

  // Run selected pattern
  switch (buttonPushCounter) {
    case 0:
      vu4(false, 0);
      #ifdef TWO_STRIPS
        vu4(false, 1);
      #endif
      break;
  
    case 1:
      vu4(true, 0);
      #ifdef TWO_STRIPS
        vu4(true, 1);
      #endif
      break;
  
    case 2:
      vu5(false, 0);
      #ifdef TWO_STRIPS
        vu5(false, 1);
      #endif
      break;
  
    case 3:
      vu5(true, 0);
      #ifdef TWO_STRIPS
        vu5(true, 1);
      #endif
      break;
  
    case 4:
      vu6(false, 0);
      #ifdef TWO_STRIPS
        vu6(false, 1);
      #endif
      break;
      
    case 5:
      vu7(true);
      break;
  
    case 6:
      vu8();
      break;
  
    case 7:
      vu9();
      break;
  
    case 8:
      vu10();
      break;
    
    case 9:
      vu7(false);
      break;
  
    case 10:
      twinkle();
      break;
  
    case 11:
      sinelon();
      break;
  
    case 12:
      balls();
      break;
  
    case 13:
      juggle();
      break;
  
    case 14:
      fire();
      break;
  
    case 15:
      ripple(false);
      break;
  
    case 16:
      rainbow(10);
      break;
  }
  
  #ifdef TWO_STRIPS_REVERSE
    ReverseCopyLeftStripOnToRightStrip();
  #endif
  if(LEDShowEnable==true) {
    FastLED.Show();
    LEDShowEnable=false;
    fadeToBlackBy(ledsRight, N_PIXELS, 255);
  }
}

// ------------------
// -- VU functions --
// ------------------

uint16_t auxReading(uint8_t channel) {

  int n = 0;
  uint16_t height = 0;

  if(channel == 0) {
    int n = analogRead(LEFT_IN_PIN); // Raw reading from left line in
    n = abs(n - 512 - DC_OFFSET); // Center on zero
    n = (n <= NOISE) ? 0 : (n - NOISE); // Remove noise/hum
    lvlLeft = ((lvlLeft * 7) + n) >> 3; // "Dampened" reading (else looks twitchy)
    volLeft[volCountLeft] = n; // Save sample for dynamic leveling
    volCountLeft = ++volCountLeft % SAMPLES;
    // Calculate bar height based on dynamic min/max levels (fixed point):
    height = TOP * (lvlLeft - minLvlAvgLeft) / (long)(maxLvlAvgLeft - minLvlAvgLeft);
  }
  
  else {
    int n = analogRead(RIGHT_IN_PIN); // Raw reading from mic
    n = abs(n - 512 - DC_OFFSET); // Center on zero
    n = (n <= NOISE) ? 0 : (n - NOISE); // Remove noise/hum
    lvlRight = ((lvlRight * 7) + n) >> 3; // "Dampened" reading (else looks twitchy)
    volRight[volCountRight] = n; // Save sample for dynamic leveling
    volCountRight = ++volCountRight % SAMPLES;
    // Calculate bar height based on dynamic min/max levels (fixed point):
    height = TOP * (lvlRight - minLvlAvgRight) / (long)(maxLvlAvgRight - minLvlAvgRight);
  }

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = constrain(height, 0, TOP);

  return height;
}

/*
 * Function for dropping the peak
 */
uint8_t peakLeft, peakRight;
void dropPeak(uint8_t channel) {
  
  static uint8_t dotCountLeft, dotCountRight;
 
  if(channel == 0) {
    if(++dotCountLeft >= PEAK_FALL) { //fall rate 
      if(peakLeft > 0) peakLeft--;
      dotCountLeft = 0;
    }
  } else {
    if(++dotCountRight >= PEAK_FALL) { //fall rate 
      if(peakRight > 0) peakRight--;
      dotCountRight = 0;
    }
  }
}

/*
 * Function for averaging the sample readings
 */
void averageReadings(uint8_t channel) {

  uint16_t minLvl, maxLvl;

  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if(channel == 0) {
    minLvl = maxLvl = volLeft[0];
    for (int i = 1; i < SAMPLES; i++) {
      if (volLeft[i] < minLvl) minLvl = volLeft[i];
      else if (volLeft[i] > maxLvl) maxLvl = volLeft[i];
    }
    if ((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
    
    minLvlAvgLeft = (minLvlAvgLeft * 63 + minLvl) >> 6; // Dampen min/max levels
    maxLvlAvgLeft = (maxLvlAvgLeft * 63 + maxLvl) >> 6; // (fake rolling average)
  }

  else {
    minLvl = maxLvl = volRight[0];
    for (int i = 1; i < SAMPLES; i++) {
      if (volRight[i] < minLvl) minLvl = volRight[i];
      else if (volRight[i] > maxLvl) maxLvl = volRight[i];
    }
    if ((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
    minLvlAvgRight = (minLvlAvgLeft * 63 + minLvl) >> 6; // Dampen min/max levels
    maxLvlAvgRight = (maxLvlAvgLeft * 63 + maxLvl) >> 6; // (fake rolling average)
  }
}

//function to copy the left strip onto the right strip in reverse order.
void ReverseCopyLeftStripOnToRightStrip() {
  #ifdef TWO_STRIPS_REVERSE
    for(int i=0; i<N_PIXELS; i++) {
      ledsLeftReverse[N_PIXELS-i] = ledsLeft[i];
    }
  #endif
}
