#ifndef DROPS_H
#define DROPS_H
#include "commonheaders.h"

// Most active drops at one time
#define MAX_DROPS 64
struct drop
{
  float location; // 0.0 to 1.0, convert to index i during render
  float intensity; // 0.0 to 1.0, convert to 255 based value during render
};

class drops : public effect
{


unsigned long micsperspark = 1000000; // one per second by default
unsigned long nextsparktime = 0;

// This array is the locations and intensities of the drops. They move and decrease in intensity during animation
drop droparray[MAX_DROPS];

unsigned char palIndex = 0;

  public:
  
	void Init()
	{
	}

  String Identify()
  {
    return "drops";
  }

  void Reset()
  {
    micsperspark = ONEMIL;
    nextsparktime = 0;
    palIndex = 0;
  }

  void SetFrequency(float f)
  {
    effect::SetFrequency(f);
      // How many mics to wait til next spark
      micsperspark = (unsigned long)((1.0f/f) * 1000000.0f);
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

      float fadeamount = deltaseconds / fadetime; // use a fade rate
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
        droparray[whichDrop].location = frand(0.4f,0.6f);
      }
      nextsparktime = curmics + micsperspark;
    }
	}

	void Render()
	{

    FadeOrClear();

    for (int i=0; i<MAX_DROPS; i++)
    {
      if (droparray[i].intensity > 0.0f)
      {
        int whichLed = (int)(droparray[i].location * NUM_LEDS/2);
        if (whichLed >= 0 && whichLed < NUM_LEDS)
        {
          CRGB rgb = finalPalette[pal][palIndex++];
          CHSV fadedColor = rgb2hsv(rgb);
          fadedColor.v = (int)(droparray[i].intensity * 255.0f);
          //dst[whichLed] = fadedColor;
          SetPixel(whichLed,fadedColor);
        }
      }
    }  
	}

  
} drops;
#endif
