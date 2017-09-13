#ifndef SPARKS_H
#define SPARKS_H
#include "commonheaders.h"
 #include "particle.h"


class sparks : public effect
{


  public:
unsigned long micsperspark = ONEMIL; // one per second by default
unsigned long nextsparktime = 0;

  static const int numsparks = 12;
  particle sparks[numsparks];

  uint8_t palIndex = 0;

  int FindFreeSpark()
  {
    for (int i=0; i<numsparks; i++)
      if (!sparks[i].active)
        return i;
    return -1;
  }


void RenderSpark(float location, float width, int pal, CRGB* dst)
{
  // render the spark as a span of pixels starting with the first color in the palette and covering the entire palette over the width of the span
  // Don't allow rendering to wrap around the end of the staff. Clamp at 0, NUM_LEDS/2 and NUM_LEDS
  // Later, add some methods for specifying render direction and mirroring options
  float palincrement = 256.0f/width;
  float pallocation = 0.0f;
  int curLed = (int)location;
  int finalLed = (int)(location - width);
  for (int l = curLed; l>finalLed; l--)
  {
    // Fix this based on velocity etc?
    if (l < 0) continue;
    if (l > NUM_LEDS/2) continue;

    uint8_t pindex = (uint8_t)pallocation;
    pallocation += palincrement;
    CRGB c = finalPalette[pal][pindex];
    //dst[l] = c;
    SetPixel(l,c);
    //dst[NUM_LEDS-l] = c;
    SetPixel(NUM_LEDS-l,c);
  }
}  
  
	void Init()
	{
    micsperspark = ONEMIL; // one per second by default
    nextsparktime = 0;
	}

  String Identify()
  {
    return "sparks";
  }

  void Reset()
  {
    micsperspark = ONEMIL; // one per second by default
    nextsparktime = 0;
  }

  void SetFrequency(float f)
  {
    effect::SetFrequency(f);
    // How many mics to wait til next spark
    micsperspark = (unsigned long)((1.0f/f) * 1000000.0f);
  }
	void Animate(unsigned long mics)
	{
    unsigned long curmics = micros();

    for (int i=0; i<numsparks; i++)
      sparks[i].Animate(mics);

    // Gen a spark when it is time
    if (curmics > nextsparktime)
    {
      int sparknum = FindFreeSpark();
      if (sparknum >= 0)
      {
        sparks[sparknum].Launch(0.0f, speed, 0.0f, 6.0f);
      }
      nextsparktime = curmics + micsperspark;
    }
  }

	void Render()
	{
    FadeOrClear();

    // Render the sparks using some simple params like location, width, palette
    // Play with various ways of rendering. Write utils in led_utils for rendering these
    // so they can be reused by anything location based elsewhere
    for (int i=0; i<numsparks; i++)
    {
      if (sparks[i].active)
      {
        RenderSpark(sparks[i].location, width, pal, dst);
      }
    }

	}

  
} sparks;
#endif
