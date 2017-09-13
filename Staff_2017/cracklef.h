#ifndef DROPS_H
#define DROPS_H
#include "effect.h"
#include "led_utils.h"
#include "color_utils.h"
#include "motion.h"

// Most active drops at one time
#define MAX_DROPS 64
typedef struct drop
{
  float location; // 0.0 to 1.0, convert to index i during render
  float intensity; // 0.0 to 1.0, convert to 255 based value during render
};

class drops : public effect
{


unsigned long micsperspark = 1000000; // one per second by default
unsigned long nextsparktime = 0;
float speed = 0.0f;

// This array is the locations and intensities of the drops. They move and decrease in intensity during animation
drop droparray[MAX_DROPS];


CHSV sparkcolor;

  public:
  
	void Init(int numpersec, float velocity, CRGB color)
	{
    // How many mics to wait til next spark
      micsperspark = (unsigned long)((1.0f/(float)numpersec) * 1000000.0f);
      sparkcolor = rgb2hsv(color);
      speed = velocity;
	}

  int FindFreeDrop()
  {
    for (int i=0; i<MAX_DROPS; i++)
    {
      if (droparray[i].intensity <= 0.0f)
        return i;
    }
    return -1;
  }

	void Animate(unsigned long mics)
	{
    unsigned long curmics = micros();
    float deltaseconds = (float)(mics)/1000000.0f;
    float deltapos = deltaseconds * speed;

    // Move the existing drops
    for (int i=0; i<MAX_DROPS; i++)
    {
      // Move the drops down
      if (droparray[i].location < 0.5f)
      {
        droparray[i].location -= deltapos;
        if (droparray[i].location <= 0.0f)
          droparray[i].intensity = 0.0f;
      }
      else
      {
        droparray[i].location += deltapos;
        if (droparray[i].location >= 1.0f)
          droparray[i].intensity = 0.0f;
      }

      // Fade them also over period of one second (make this adjustable later)
      float fadeamount = deltaseconds; // later use a fade rate
      droparray[i].intensity -= fadeamount;

    }

    // Gen a drop when it is time
    if (curmics > nextsparktime)
    {
      int whichDrop = FindFreeDrop();
      if (whichDrop >= 0)
      {
        // Set up a new drop
        droparray[whichDrop].intensity = 1.0f;
        droparray[whichDrop].location = frand(0.0f,1.0f);
      }
      nextsparktime = curmics + micsperspark;
    }
	}

	void Render(CRGB* dst)
	{
    // Always be fading the entire rendered effect
    for (int i=0; i<NUM_LEDS; i++)
    {
      leds[i].nscale8(232);
    }

    for (int i=0; i<MAX_DROPS; i++)
    {
      if (droparray[i].intensity > 0.0f)
      {
        int whichLed = (int)(droparray[i].location * NUM_LEDS)
        if (whichLed >= 0 and whichLed < NUM_LEDS)
        {
          CHSV fadedColor = sparkcolor;
          fadedColor.v = (int)(droparray[i].intensity * 255.0f)
          leds[whichLed] = fadedColor;
        }
      }
    }

    // Clear the spark array
    for (int i=0; i<NUM_LEDS; i++)
      sparkarray[i] = 0;
  
	}

  
} drops;
#endif
