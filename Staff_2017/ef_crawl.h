#ifndef CRAWL_H
#define CRAWL_H
#include "commonheaders.h"


// EveryN produces long spirals that can be "tighhtened" by changing N from around 7 to around 12 and back
// Looks nice when N is animated back and forth at around one second per shift
class crawl : public effect
{

public:

  static const int numCursors = 3;

  uint8_t direction[numCursors] = {0}; // 0 = up, 1 = down, 2 = right, 3 = left
  int cursor[numCursors] = {0};
  int numMovedInDir[numCursors] = {0};

unsigned long micspershift = ONEMIL / 12; // 12 moves per second
int timeTillShift = 0;

unsigned long micsperdirshift = ONEMIL / 3; // 3 direction shifts per second
int timeTillDirShift = 0;

uint8_t palIndex = 0;


public:
  
void Init()
{
}

String Identify()
{
  return "crawl";
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
  timeTillDirShift -= mics;

  // Change the direction when it is time
  if (timeTillDirShift <= 0)
  {
    for (int i=0; i<numCursors; i++)
    {
      direction[i] = random(4);
      if (direction[i] == 1) direction[i] = 0; // only up for now
    }
  }

  // Move the cursor when it is time
  if (timeTillShift <= 0)
  {
//    timeTillShift = micspershift;
  }
}


void MoveCursors()
{
  for (int i=0; i<numCursors; i++)
  {
  // Rules are:
  // If going forward, will go at least 4 steps, then 50% chance of breakout
  // If going right or left, will go at least 2 steps. Then 50% chance of breakout
  // If going up and breakout, then choose right or left
  // If going right or left and breakout, then choose up only

    if (direction[i] == 0 && numMovedInDir[i] > 4)
    {
      int r = random(255);
      if (r > 128)
      {
        if (r > 192) 
          direction[i] = 2;
        else
          direction[i] = 3;
        numMovedInDir[i] = 0;
      }
    }
    else if ((direction[i] == 2 || direction[i] == 3) && numMovedInDir[i] > 2)
    {
      int r = random(255);
      if (r > 128)
      {
        direction[i] = 0;
        numMovedInDir[i] = 0;
      }
    }

    int newcursor = 0;
    if (direction[i] == 0)
    {
      newcursor = OppositeSide(cursor[i]);
      if (newcursor >= NUM_LEDS/2)
      {
        newcursor -= 4;
        // Wrap at end means reset to 0
        if (newcursor < NUM_LEDS/2)
          newcursor = 0;
      }
      else
      {
        newcursor += 4;
        // Wrap at end means reset to 0
        if (newcursor > NUM_LEDS/2)
          newcursor = 0;
      }
    }
    else if (direction[i] == 2)
    {
      if (cursor[i] >= NUM_LEDS/2)
        newcursor = cursor[i] + 1;
      else
        newcursor = cursor[i] - 1;
    }
    else
    {
      if (cursor[i] >= NUM_LEDS/2)
        newcursor = cursor[i] - 1;
      else
        newcursor = cursor[i] + 1;
    }  

    cursor[i] = newcursor;
    numMovedInDir[i]++;
  }
}

void Render()
{
  FadeOrClear();


  MoveCursors(); // 60 fps movement for 3 traces

  for (int i=0; i<numCursors; i++)
  {
    CRGB c = finalPalette[pal][palIndex++];
    if (cursor[i] < NUM_LEDS && cursor[i] >= 0)
    {
      //dst[cursor[i]] = c;
      SetPixel(cursor[i],c);
    }
    else
      cursor[i] = 0;
  }



  // Move in a semi-random way. Moving up or down the staff means other side and shift. Moving to the side means same side, just +/- 1

}

  
} crawl;
#endif
