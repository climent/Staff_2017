#ifndef MODCHASE_H
#define MODCHASE_H
#include "commonheaders.h"


class modchase : public effect
{

public:
unsigned long micspershift = ONEMIL;
int timeTillShift = 0;

unsigned long micsperlaunch = ONEMIL;
int timeTillLaunch = 0;

unsigned long micsperpalchange = ONEMIL;
int timeTillPalChange = 0;

unsigned char palIndex = 0;
int numToShift = 0;
int numToEmit = 0;

  public:
  
	void Init(int palchangespersec)
	{
    micsperpalchange = (unsigned long)((1.0f/(float)palchangespersec) * (float)ONEMIL);
    timeTillPalChange = micsperpalchange;
  }

  String Identify()
  {
    return "modchase";
  }

  void Reset()
  {
    micspershift = ONEMIL;
    timeTillShift = 0;

    micsperlaunch = ONEMIL;
    timeTillLaunch = 0;

    micsperpalchange = ONEMIL;
    timeTillPalChange = 0;

    palIndex = 0;
    numToShift = 0;
    numToEmit = 0;    
  }

  void SetSpeed(float ledspersec)
  {
    effect::SetSpeed(ledspersec);
     // How many mics to wait til next change
      micspershift = (unsigned long)((1.0f/ledspersec) * (float)ONEMIL);
      timeTillShift = micspershift;
  }

  void SetFrequency(float f)
  {
    effect::SetFrequency(f);
     // How many mics to wait til next launch
      micsperlaunch = (unsigned long)((1.0f/f) * (float)ONEMIL);
      timeTillLaunch = micsperlaunch;
  }

	void Animate(unsigned long mics)
	{
    timeTillShift -= mics;
    timeTillPalChange -= mics;
    timeTillLaunch -= mics;

    // Move the patterns when it is time
    if (timeTillShift <= 0)
    {
      numToShift++;
      timeTillShift = micspershift;
    }

    // Launch when it is time
    if (timeTillLaunch <= 0)
    {
      numToEmit = (int)width;
      timeTillLaunch = micsperlaunch;
    }

    if (timeTillPalChange <= 0)
    {
      palIndex += 1;
      timeTillPalChange = micsperpalchange;
    }
	}

// Have the two spirals shift past one another
uint8_t extra = 0;

	void Render()
	{
    //FadeOrClear();
    extra++;

    // Scroll
    while (numToShift > 0)
    {
      for (int i=NUM_LEDS/2-1; i>=0; i--)
      {
        int from = i-1;
        if (from >= 0)
          dst[i] = dst[from];
        else
          dst[i] = CRGB::Black;
      }

      for (int i=NUM_LEDS/2; i<NUM_LEDS; i++)
      {
        int from = i+1;
        if (from < NUM_LEDS)
          dst[i] = dst[from];
        else
          dst[i] = CRGB::Black;
      }

      numToShift--;

      // Emit new stuff
      if (numToEmit > 0)
      {
        // Use palette later
        //dst[0] = finalPalette[pal][palIndex++]; // Just march the palette for now
        SetPixel(0,finalPalette[pal][palIndex++]);
        SetPixel(NUM_LEDS-1,finalPalette[pal][palIndex+64]);
        numToEmit--;
      }

    }


	}

  
} modchase;
#endif
