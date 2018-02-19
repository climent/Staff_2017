#ifndef EF_SAND_H
#define EF_SAND_H
#include "commonheaders.h"

class sand : public effect
{

public:

  int timeTillNext = 100000;
  uint16_t index = 0;

  void Init()
  {
    // Nothing to do here
  }

  String Identify()
  {
    return "sand";
  }

  void Reset()
  {

  }

  void Animate(unsigned long mics)
  {
    timeTillNext -= mics;
    if (timeTillNext <= 0)
    {
      index++;
      timeTillNext = 100000;
      FadeOrClear();
      if (index == NUM_LEDS)
      {
        index = 0;
      }
    }
    // Nothing
  }

  void Render()
  {
    SetPixel(index, CRGB::Red);
  }

} sand;
#endif
