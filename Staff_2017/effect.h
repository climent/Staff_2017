#ifndef EFFECT_H
#define EFFECT_H
#include "color_utils.h"
#include "led_utils.h"
class effect
{
  public:

    enum ClearMode
    {
      kClear = 0,
      kFade = 1
    };

    enum MirrorMode
    {
      kNone = 0,
      kMirror,
      kFlipAndMirror
    };

    // Some basic controls for most effects
    float frequency;
    float speed;
    float width;
    float fadetime;
    uint8_t fadeunits; // for use with nscale8 to fade the effect over time
    uint8_t pal; // pal index 0, 1 or 2
    CHSV color;
    ClearMode clearmode;
    MirrorMode mirrormode;
    uint8_t brightness;

    CRGB* dst = NULL;

    bool running = false;

    effect()
    {
      SetFrequency(1.0f);
      SetSpeed(1.0f);
      SetWidth(1.0f);
      SetFadeTime(1.0f);
      SetPal(0);
      SetBrightness(255);
      SetClearMode(kClear);
      SetMirrorMode(kNone);
    }

    virtual void SetBuffer(CRGB* dest) {
      dst = dest;
    }; // Must have a destination to render
    virtual CRGB* GetBuffer() {
      return dst;
    }; // Get the working buffer for this effect
    virtual void Animate(unsigned long micros);
    virtual void Render();
    virtual void SetClearMode(ClearMode mode) {
      clearmode = mode; // Clear to black before rendering each frame, or fade old buffer contents out over time (leave trails)
    }
    virtual void SetMirrorMode(MirrorMode mode) {
      mirrormode = mode;
    }

    // These all have default impls so that we can safely call them on effects that don't use them
    // NOTE: Make sure these routines can be called safely while an effect is active!!
    virtual void SetFrequency(float f) {
      frequency = f;
    }; // Usually in leds/sec or launches/sec etc
    virtual void SetSpeed(float s) {
      speed = s;
    }; // Usually in leds/sec or normalized param/sec
    virtual void SetWidth(float w) {
      width = w;
    }; // Usually in leds, or norm param
    virtual void SetBrightness(uint8_t b) {
      brightness = b;
    }

    virtual void SetFadeTime(float t)
    {
      fadetime = t;
      // Every second we will fade this amount
      float fadepersec = 1.0f / t;
      float fadeperframe = fadepersec / 60.0f;
      fadeunits = (int)(255.0f * (1.0f - fadeperframe));

    }; // The time a newly lit led should take to fade to black

    // Some funcs used by only a few effects
    virtual void SetSpan(int N) {}; // Set some kind of span or interval (N for mod N effects for example)
    virtual void SetPal(uint8_t p) {
      pal = p;
    };

    virtual String Identify() = 0; // Every effect must name itself
    virtual void Start() {
      running = true;
    };
    virtual void Stop() {
      running = false;
    };
    virtual void Reset() = 0; // Every effect must support Reset so we can start setting params and restart

    inline void FadeOrClear()
    {
      if (clearmode == kClear)
        Blackout(dst);
      else
        for (int i = 0; i < NUM_LEDS; i++)
        {
          dst[i].nscale8(fadeunits);
        }
    }

    // This common pixel setter allows us to tweak color as it is being rendered by the effects. In this first case, brightness
    inline void SetPixel(int whichLed, CRGB c)
    {
      dst[whichLed] = c;
      dst[whichLed].nscale8(brightness);
    }

    void ApplyMirror(CRGB* src, effect::MirrorMode mode)
    {
      switch (mode)
      {
        case effect::MirrorMode::kNone:
          break;
        case effect::MirrorMode::kMirror:
          {
            for (int i = 0; i < NUM_LEDS / 4; i++)
            {
              int invi = NUM_LEDS - i;
              src[NUM_LEDS / 2 - i] = src[i];
              src[NUM_LEDS / 2 + i] = src[invi];
            }
            break;
          }
      }
    }

    void PrintState()
    {
      Serial.print(Identify()); Serial.println(": ");
      Serial.print("    "); Serial.print("frequency: "); Serial.println(frequency);
      Serial.print("    "); Serial.print("speed: "); Serial.println(speed);
      Serial.print("    "); Serial.print("width: "); Serial.println(width);
      Serial.print("    "); Serial.print("fadetime: "); Serial.println(fadetime);
      Serial.print("    "); Serial.print("fadeunits: "); Serial.println(fadeunits);
      Serial.print("    "); Serial.print("pal: "); Serial.println(pal);
      Serial.print("    "); Serial.print("brghtness: "); Serial.println(brightness);
      Serial.print("    "); Serial.print("clearmode: "); Serial.println(clearmode);
      Serial.print("    "); Serial.print("mirrormode: "); Serial.println(mirrormode);

    }

};
#endif
