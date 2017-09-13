#ifndef POOLS_H
#define POOLS_H
#include "commonheaders.h"


class pools : public effect
{
  public:
  float nodes[9]; // units are frontpalette space (0-255)
  float vels[9];  // in frontpalette units per second
  int locs[9]; // the node locations in strip space
  int m_length;
  
  void Init()
  {
    m_length = NUM_LEDS/2;
    // For now, random vels
    for (int i=0; i<9; i++)
    {
      int r = random(10,255);
      vels[i] = (float)(r);
      locs[i] = i * (m_length/8);
      nodes[i] = 0;
    }
    locs[8] = m_length-1; // put last node at the end
    
  }


  String Identify()
  {
    return "pools";
  }

  void Reset()
  {
  	// Nothing for now, but soon...
  }

  
  void SlowVels()
  {
    // For now, random vels
    for (int i=0; i<9; i++)
    {
      int r = random(50,200);
      vels[i] = (float)(r);
    }
  }
  
  void FastVels()
  {
    // For now, random vels
    for (int i=0; i<9; i++)
    {
      int r = random(100,400);
      vels[i] = (float)(r);
    }
  }
  
  void Rythmic()
  {
    // For now, random vels
    int v[9] = {255,1024,0,255,128,255,0,1024,255};
    for (int i=0; i<9; i++)
    {
      int n;
      if (i%2)
      {
        n = 0;
      }
      else
      {
        n = 255;
      }
      vels[i] = (float)(v[i]);
      nodes[i] = (float)(n);
    }
    
  }
 
  void Animate(unsigned long mics)
  { 
  	float dt = (float)(mics)/(float)ONEMIL;

    for (int i=0; i<9; i++)
    {
      nodes[i] += vels[i] * dt;
      if (nodes[i] < 0)
     {
       nodes[i] = -nodes[i]; // bounce
       vels[i] = -vels[i];
     }
      if (nodes[i] > (float)(255))
     {
       nodes[i] = (float)(511) - nodes[i]; // bounce
       vels[i] = -vels[i];
     }
    }
  }
  
  void Render()
  {
    FadeOrClear();

    for (int i=0; i<8; i++)
    {
      // interpolate from node i to i+1 in frontpalette space
      float startp = nodes[i];
      float endp = nodes[i+1];
      float pdelta = (endp-startp) / (float)(locs[i+1]-locs[i]);
      float curp = startp;
      for (int l=locs[i]; l<=locs[i+1]; l++)
      {
        int palindex = (int)(curp);
        if (palindex < 0) palindex = 0;
        if (palindex > 255) palindex = 255;
        //dst[l] = finalPalette[pal][palindex];
        SetPixel(l,finalPalette[pal][palindex]);
        curp += pdelta;
      }
    }
    // copy this out to the rest of the buffer legs
    for (int i=NUM_LEDS-1; i>=NUM_LEDS/2; i--)
    {
    	dst[i] = dst[NUM_LEDS-i];
    }
  }
  
} pools;
#endif
