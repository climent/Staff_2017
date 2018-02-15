#ifndef PMARCH_N
#define PMARCH_N
#include "commonheaders.h"

class pmarch : public effect
{

// This effect just shows a palette along the length of the staff and slowly shifts it
public:

unsigned long micspershift = ONEMIL;
int timeTillShift = 0;
uint8_t startIndex = 0; // this is where in the palette we start, cycle this slowly to 255 and wrap around 


  public:
  
	void Init(int shiftpersec)
	{
    // How many mics to wait til next change
      micspershift = (unsigned long)((1.0f/(float)shiftpersec) * (float)ONEMIL);
      timeTillShift = micspershift;

	}

  String Identify()
  {
    return "pmarch";
  }

  void Reset()
  {
    micspershift = ONEMIL;
    timeTillShift = 0;
}


  void SetSpeed(float ledspersec)
  {
     effect::SetSpeed(ledspersec);

     // How many mics to wait til next change
      micspershift = (unsigned long)((1.0f/ledspersec) * (float)ONEMIL);
      timeTillShift = micspershift;
  }

	void Animate(unsigned long mics)
	{
    timeTillShift -= mics;

 //   float nF = (float)(beatsin16(20))/65535.0f;
 //   nF = nF * 3.0f + 1.0f;
 //   SetFrequency(nF);

    // Move the patterns when it is time
    if (timeTillShift <= 0)
    {
      startIndex += 1;
      timeTillShift = micspershift;
    }
	}

	void Render()
	{

    FadeOrClear();

    float cur = (float)startIndex;
    for (int i=0; i<NUM_LEDS/2; i++)
    {
      uint8_t pindex = ((int)(i*frequency) + startIndex)%256;
      CRGB c = finalPalette[0][pindex];
      CRGB c2 = finalPalette[1][pindex];
      // Set the other side as well 
      int otherSide = (NUM_LEDS-1) - i;
      SetPixel(i,c);
      //dst[i] = c;
      //dst[i+NUM_LEDS/2] = c;
      SetPixel(i+NUM_LEDS/2,c);
    }
	}

  
} pmarch;
#endif
