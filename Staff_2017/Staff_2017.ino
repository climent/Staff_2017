#include "FastLED.h"
#include "power_mgt.h"

#include <SerialFlash.h>
#include <SD.h>
#include <SPI.h>
const int SDchipSelect = 4;    // Audio Shield has SD card CS on pin 10
const int FlashChipSelect = 6; // digital pin for flash chip CS pin
//const int FlashChipSelect = 21; // Arduino 101 built-in SPI Flash


// Global aggresive var
bool aggressive = true;

// Done as two spirals, down and back
#define NUM_LEDS 304
#define DATA_PIN 7
//#define CLOCK_PIN 13

// motion stuff
float roll, pitch, heading; // Euler orientation
float x, y, z; // vector orientation

bool timeToExit = false;

unsigned long lastmillis = 0;
unsigned long lastmics = 0;

int timeTillPrint = 1000; // Print diagnostics once per second
int timeTillRender = 16; // 60Hz rendering
int timeTillOrientation = 16; // Let' stry 60hz for motion updates as well

// Bump command vars
int numBumps = 0;
int gotBumpedGate = 0;
int bumpCounterTimeout = 0;

// Includes of other modules
#include "led_utils.h"
#include "effect.h"
#include "motion.h"


// Command processing and effect sequence control to drive the whole show
// command sequences can come from the Serial line or from a file or data
#include "controls.h"
#include "commands.h"

void setup ()
{
  Serial.begin(115200);
  delay(1000); // Let the serial monitor come up

  FastLED.addLeds<WS2812B, DATA_PIN, BGR>(leds[2], NUM_LEDS);
  FastLED.setDither(0);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);  // enable access to LEDs

  int val = analogRead(0); // read random value;
  Serial.printf("Random seed is: %d\n", val);
  randomSeed(val);

  // Limit to 2 amps to begin with
  set_max_power_in_volts_and_milliamps(5, 2000);

  // Motion sensors init
  imu.begin();
  filter.begin(100);

  // Run a color check and strip test
//  Test(leds[3]);

  // Create a bunch of palettes to use
  GenerateGlobalPalettes();

  // Init all the effects and the controller that drives/animates them
  controller.Init();

  if (!SerialFlash.begin(FlashChipSelect)) {
    Serial.println("Unable to access SPI Flash chip");
  }

  //  RunScript();
}

int frameCount = 0;
int renderCount = 0;

void loop ()
{
  frameCount++;

  // Do timing work
  unsigned long curmics = micros();

  // Handle rollover
  //  if (curmics < lastmics)
  //    lastmics = curmics;

  unsigned long deltamics = curmics - lastmics;
  lastmics = curmics;

  unsigned long curmillis = millis();
  //  if (curmillis < lastmillis)
  //    lastmillis = curmillis;

  unsigned long deltamillis = curmillis - lastmillis;
  lastmillis = curmillis;

  // Decrement our timers
  timeTillPrint -= deltamillis;
  timeTillRender -= deltamillis;
  timeTillOrientation -= deltamillis;

  // Update the sensors and the fusion filter
  UpdateMotion(deltamics);

  if (gotFlipped)
  {
    gotFlipped = false;
    if (rightSideUp)
    {
      // Brighten things up
      FastLED.setBrightness(255);

      // Set a global to increase speeds and frequencies etc
      aggressive = true;
    }
    else
    {
      // Dim things down
      FastLED.setBrightness(64);

      // Set a global to reduce speeds and frequencies
      aggressive = false;
    }
  }

  // Gated bump counter
  if (gotBumpedGate > 0)
    gotBumpedGate -= deltamillis;
  if (bumpCounterTimeout > 0)
    bumpCounterTimeout -= deltamillis;

  if (gotBumped && gotBumpedGate <= 0)
  {
    numBumps++;
    gotBumped = false;
    gotBumpedGate = 600; // 600 ms until we can get another bump
    bumpCounterTimeout = 2000; // two second after last bump we will accept the command

    Serial.println("gotBumped!");
    //    Serial.print("maxAx: "); Serial.println(maxAx);
    //    Serial.print("maxAy: "); Serial.println(maxAy);
    //    Serial.print("maxAz: "); Serial.println(maxAz);

    //    Serial.print("minAx: "); Serial.println(minAx);
    //    Serial.print("minAy: "); Serial.println(minAy);
    //    Serial.print("minAz: "); Serial.println(minAz);

    maxAx = 0.0;
    maxAy = 0.0;
    maxAz = 0.0;
    minAx = 0.0;
    minAy = 0.0;
    minAz = 0.0;
  }

  if (bumpCounterTimeout <= 0 && numBumps > 0)
  {
    Serial.printf("Got bump command: %d bumps\n",numBumps);
    numBumps = 0;
  }

  // Handle inputs
  ProcessInput(deltamics);

  if (timeTillPrint <= 0)
  {
    timeTillPrint = 10000;

    Serial.print("frames per sec: "); Serial.println(frameCount / 10);
    frameCount = 0;
    Serial.print("rendered frames: "); Serial.println(renderCount / 10);
    renderCount = 0;
    Serial.print("deltamics: "); Serial.println(deltamics);
    Serial.print("micsToPause: ");Serial.println(micsToPause);
    Serial.print("Serial available: ");Serial.println(Serial.available());

    //Serial.print("x: "); Serial.println(x);
    //Serial.print("y: "); Serial.println(y);
    //Serial.print("z: "); Serial.println(z);

#if 0
    Serial.print("Orientation: ");
    Serial.print(heading);
    Serial.print(" ");
    Serial.print(pitch);
    Serial.print(" ");
    Serial.println(roll);
#endif
  }

  if (timeTillOrientation <= 0)
  {
    // Update the orientation
    getOrientation(&roll,&pitch,&heading,&x,&y,&z);
    timeTillOrientation = 16;
  }

  // Animate the controller. This will animate any active effects, process animated palettes, lfo's, and do random or scripted switching of effects and animation of parameters
  controller.Animate(deltamics);

  // Render all active buffers and mixdown, then show with power limits applied
  if (timeTillRender <= 0)
  {
    renderCount++;
    timeTillRender = 8;
    controller.Render();
    show_at_max_brightness_for_power();
  }

}
