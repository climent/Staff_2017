#ifndef CRACKLE_H
#define CRACKLE_H
#include "commonheaders.h"

class crackles : public effect
{
    unsigned long micsperspark = ONEMIL; // one per second by default
    long micstilspark = 0;
    unsigned char palIndex = 0;
    uint8_t numsparkstoemit = 0;
    float emitLoc = 0.0f;
    bool randDir = true;

  public:

    void Init()
    {
      // Nothing to do here, use the param setter routines below!!
    }

    String Identify()
    {
      return "crackles";
    }

    void Reset()
    {
      micsperspark = ONEMIL; // one per second by default
      micstilspark = 0;
      palIndex = 0;
    }

    void SetFrequency(float f)
    {
      effect::SetFrequency(f);
      // How many mics to wait til next spark
      micsperspark = (unsigned long)((1.0f / f) * 1000000.0f);
    }

    void Animate(unsigned long mics)
    {
      micstilspark -= mics;

      // Gen a spark when it is time
      if (micstilspark <= 0)
      {
        numsparkstoemit++;
        micstilspark = micsperspark;
      }

      // Location moves up and down the staff at about 2 seconds per sweep
      emitLoc = (float)beatsin16(15) / 65535.0f;
    }

    void Render()
    {
      FadeOrClear();

      while (numsparkstoemit > 0)
      {
        CRGB rgb = finalPalette[pal][palIndex++];

        // <<TODO>> Create interesting fronts that migrate around in a complex way
        int iLoc = (int)(emitLoc * NUM_LEDS / 2);
        if (random8() > 128) iLoc = NUM_LEDS - iLoc - 1;
        // Add random distribution +/- 8 leds (one wrap)
        int iRand = random8() % 8;
        if (randDir)
          iLoc += iRand;
        else
          iLoc -= iRand;

        randDir = !randDir;

        if (iLoc >= 0 && iLoc < NUM_LEDS)
        {
          SetPixel(iLoc, rgb);

        }
        numsparkstoemit--;
      }

    }


} crackles;
#endif
