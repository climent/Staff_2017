#ifndef NULL_H
#define NULL_H
#include "commonheaders.h"

class null : public effect
{

// This is just a blank effect that does nothing
public:
 
	void Init()
	{
  }

  String Identify()
  {
    return "null effect";
  }

  void Reset()
  {
  }

	void Animate(unsigned long mics)
	{
	}

	void Render()
	{
    FadeOrClear();
  }
 
} null;
#endif
