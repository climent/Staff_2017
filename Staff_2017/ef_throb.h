#ifndef THROB_H
#define THROB_H
#include "commonheaders.h"

class throb : public effect
{

public:

  uint8_t palIndex = 0;

	void Init()
	{

	}

  String Identify()
  {
    return "throb";
  }

  void Reset()
  {

  }

	void Animate(unsigned long mics)
	{
    palIndex++;

    float lfoVal = beat16(20) / 65535.0f;
    uint8_t index = (uint8_t)(lfoVal*255.0f);

    for (int i = 0; i < NUM_LEDS; i++)
    {
      CHSV hsv = CHSV(palIndex, 255, 32);
      //dst[i] = hsv;
      //Serial.println(palIndex);
      SetPixel(i, CHSV(palIndex, 255, random8(64, 255)));
    }
	}

	void Render()
	{
    // FadeOrClear();
	}

} throb;
#endif
