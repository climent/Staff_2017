#ifndef NUMBERS_H
#define NUMBERS_H

static const int ONEMIL = 1000000;

float frand(float LO, float HI)
{
  float r3 = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
  return r3;
}
#endif
