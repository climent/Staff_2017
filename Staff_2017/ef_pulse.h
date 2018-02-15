#ifndef PULSE_H
#define PULSE_H
#include "commonheaders.h"


class pulse : public effect
{

unsigned char palIndex = 0;


unsigned long micsperflash = ONEMIL;
int timeTillFlash = 0;

bool onOff[8];

  public:
  
	void Init()
	{

	}

  String Identify()
  {
    return "pulse";
  }

  void Reset()
  {

  }

  void SetFrequency(float f)
  {
    effect::SetFrequency(f);
    // How many mics to wait til next spark
    micsperflash = (unsigned long)((1.0f/f) * 1000000.0f);
  }


	void Animate(unsigned long mics)
	{
    timeTillFlash -= mics;
    if (timeTillFlash <= 0)
    {
      timeTillFlash = micsperflash;
      Flash();
    }
	}

  void Flash()
  {
    Serial.printf("Flash!\n");

    for (int i=0; i<8; i++)
      onOff[i] = (random8()>128);

    // Basically just turn on every Nth and every Mth led, with N and M being different numbers
    palIndex+=7;

    int testBit = 0;
    for (int i=0; i<NUM_LEDS; i++)
    {
      int testBit = i%8;
        if (onOff[testBit])
          SetPixel(i,finalPalette[pal][palIndex]);
    }
  }

	void Render()
	{
    FadeOrClear();


	}

  
} pulse;
#endif
