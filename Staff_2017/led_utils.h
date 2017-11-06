
#ifndef LEDUTILS_H
#define LEDUTILS_H


void Overwrite (CRGB* src, CRGB* dst)
{
  for (int i = 0; i < NUM_LEDS; i++)
    dst[i] = src[i];
}

void BlendHalf (CRGB* src, CRGB* dst)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    dst[i] = blend( dst[i], src[i], 128 ); // <<TODO>> Make a version that does a variable blend!
  }

}

void WriteOneChannel (CRGB* src, CRGB* dst, int channel)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    if (channel == 0)
      dst[i].r = src[i].r;
    else if (channel == 1)
      dst[i].g = src[i].g;
    else if (channel == 2)
      dst[i].b = src[i].b;
  }

}
void WriteIfNonBlack (CRGB* src, CRGB* dst, int channel)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    if (src[i].r > 12 || src[i].g > 12 || src[i].b > 12)
      dst[i] = src[i];
  }

}

void BlendIfBrighter (CRGB* src, CRGB* dst)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    dst[i].r = MAX(src[i].r, dst[i].r);
    dst[i].g = MAX(src[i].g, dst[i].b);
    dst[i].b = MAX(src[i].b, dst[i].b);
  }
}

void ChooseBrightest(CRGB* src, CRGB* dst)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    int s = src[i].r + src[i].g + src[i].b;
    int d = dst[i].r + dst[i].g + dst[i].b;
    if (s >= d)
      dst[i] = src[i];
  }
}

void OverwriteIfEven (CRGB* src, CRGB* dst)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    if (i % 2 == 0)
    {
      dst[i] = src[i];
    }
  }
}

void OverwriteIfBrighter (CRGB* src, CRGB* dst, int level)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    if (src[i].r > level || src[i].g > level || src[i].b > level)
    {
      dst[i] = src[i];
    }
  }
}

// Hacky soft pixel writer
void RenderSoftSpot(CRGB* dst, int i, CRGB c)
{
  dst[i] = c;
  if ((i - 1) >= 0)
  {
    dst[i - 1] = c;
    dst[i - 1] /= 16;
  }
  if ((i + 1) < NUM_LEDS)
  {
    dst[i + 1] = c;
    dst[i + 1] /= 16;
  }
}

void Blackout(CRGB* dst)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    dst[i] = CRGB(0, 0, 0);
  }
}

void SetAll(CRGB* dst, CRGB c)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    dst[i] = c;
  }
}

void Crawl(CRGB color, CRGB* dst)
{
  Blackout(dst);
  for (int i = 0; i < NUM_LEDS; i++)
  {
    dst[i] = color;
    FastLED.show();
    //delay(1);
  }
}

int OppositeSide(int i)
{
  return (NUM_LEDS - i - 1);
}

void Test(CRGB* dst)
{
  Blackout(dst);
  // Always do a full connectivity test at the beginning
  Crawl(CRGB(255, 0, 0), dst);
  Crawl(CRGB(0, 255, 0), dst);
  Crawl(CRGB(0, 0, 255), dst);
  Crawl(CRGB(255, 255, 0), dst);
  Crawl(CRGB(255, 0, 255), dst);
  Crawl(CRGB(0, 255, 255), dst);
  Blackout(dst);
}


void SafeSetLed(CRGB* dst, int i, CRGB c)
{
  if (i >= 0 && i < NUM_LEDS)
    dst[i] = c;
}


void SafeShow()
{
  show_at_max_brightness_for_power();
}

// Some utils to get normed values of height and angle (around staff) based on led index i

float GetHeight(int i)
{
  float p = (float)(i) / (float)(NUM_LEDS);
  if (p <= 0.5f)
  {
    return (p * 2.001f);
  }
  else
  {
    return (1.0f - p) * 2.001f;
  }
}

// 0.0 - 1.0 not radians, or degrees, just normed coordinate around the circle
float GetAngle(int i)
{
  float numLedsPerThread = 8.0f;

  float p = 0.0f;
  if (i >= NUM_LEDS / 2)
  {
    i = NUM_LEDS - i;
    p = (float)(i) / numLedsPerThread;
    p += 0.5f; // other  side
  }
  else
  {
    p = (float)(i) / numLedsPerThread;
  }

  while (p > 1.0f)
    p -= 1.0f;
  return p;

}


#endif
