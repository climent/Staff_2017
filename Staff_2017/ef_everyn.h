#ifndef EVERY_N
#define EVERY_N
#include "commonheaders.h"

// EveryN produces long spirals that can be "tighhtened" by changing N from
// around 7 to around 12 and back
// Looks nice when N is animated back and forth at around one second per shift
class everyn : public effect
{

public:

  unsigned long micspershift = ONEMIL;
  int timeTillShift = 0;
  unsigned int startIndex = 0;
  unsigned int startIndex2 = 0;
  int width = 1;

  unsigned int N=0;
  uint8_t palIndex = 0;

  int micstillNshift = 0;
  int micsperNshift = ONEMIL * 10; // every 10 seconds change N
  int Nshift = 1;

public:

  void Init(int shiftpersec, unsigned int span)
  {
    // How many mics to wait til next change
      micspershift = (unsigned long)((1.0f/(float)shiftpersec) * (float)ONEMIL);
      timeTillShift = micspershift;
      N = span;
  }

  String Identify()
  {
    return "everyn";
  }

  void Reset()
  {
    N = 0;
    width = 1;
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

  void SetSpan(int span)
  {
    N = span;
  }

  void SetWidth(int w)
  {
    effect::SetWidth(w);
    width = w;
  }

  void PrintState()
  {
    // The state of everyn uses a specific SPAN.
    effect::PrintState();
    Serial.print("    "); Serial.print("span: "); Serial.println(N);
  }

  void Animate(unsigned long mics)
  {
    timeTillShift -= mics;
    micstillNshift -= mics;

    // Move the patterns when it is time
    if (timeTillShift <= 0)
    {
      startIndex += 1;
      if (startIndex >= N) startIndex = 0;
      startIndex2 -= 1;
      if (startIndex2 < 0) startIndex2 = N;
      timeTillShift = micspershift;
    }

    if (micstillNshift <= 0)
    {
      micstillNshift = micsperNshift;
      N += Nshift;
      if (N < 7)
      {
        Nshift = 1;
      }
      if (N > 9)
      {
        Nshift = -1;
      }
    }
  }

  void Render()
  {

    FadeOrClear();

    CRGB c = finalPalette[pal][palIndex++];
    uint8_t t = palIndex+128;
    CRGB c2 = finalPalette[pal][t];

    for (int i = 0; i < NUM_LEDS / 2; i++)
    {
      int check = (i + startIndex) % N;
      int check2 = (i + startIndex2) % N;

      // Set the other side as well
      int otherSide = (NUM_LEDS - 1) - i - 5;

      // Try rendering wide patch
      // <<TODO>> Use SetPixel somehow so we can get brightness for this effect
      if (check == 0)
      {
        RenderSoftSpot(dst, i, c);
        RenderSoftSpot(dst, otherSide, c);
      }

  //    if (check2 == 0)
      if (0)
      {
        RenderSoftSpot(dst, i, c2);
        RenderSoftSpot(dst, otherSide, c2);
      }
    }
  }

} everyn;
#endif
