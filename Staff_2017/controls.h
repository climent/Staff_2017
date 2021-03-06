#ifndef CONTROLS_H
#define CONTROLS_H

#include "commonheaders.h"

// Mode "off" means don't render it
enum MixMode
{
  kOverwrite = 0,
  kBlendHalf = 1,
  kWriteIfNonBlack = 2,
  kBlendIfBrighter = 3,
  kOverwriteIfEven = 4,
  kChooseBrightest = 5,
  kOverwriteIfBrighter = 6,
  kOff
};

// Render buffers 0-1 and final mixdown buffer 2
#define kMixDownBuffer 2
CRGB leds[3][NUM_LEDS];

// Effects are rendered into the working buffers 0-2
effect* currenteffects[2] = {NULL, NULL};

//MixMode blendmodes[3] = {kOverwrite,kBlendHalf,kBlendIfBrighter};
MixMode blendmodes[2] = {kOverwrite, kOff};

// Create some "boards" to do image and fluid and flame effects into
static const int kMatrixWidth = 8;
static const int kMatrixHeight = 70;
unsigned char board[kMatrixHeight][kMatrixWidth];
unsigned char work[kMatrixHeight][kMatrixWidth];

// For checking the mapping of leds to board locations. With 8x70 and a slight
// adjustment to GetHeight(), we have a very close mapping
// It is TOTALLY not orthogonal and even. It massively skewed, due to the
// winding nature of the leds strips. BUT, still useful for mapping 2D effects
// (noise etc) to the staff
unsigned char test[kMatrixHeight][kMatrixWidth];

bool enableBaseChange = false;

// The effect classes
#include "ef_crackles.h"
#include "ef_drops.h"
#include "ef_modchase.h"
#include "ef_pulse.h"
#include "ef_pools.h"
#include "ef_everyn.h"
#include "ef_throb.h"
#include "ef_pmarch.h"
#include "ef_fluid.h"
#include "ef_sparks.h"
#include "ef_crawl.h"
#include "ef_flame.h"
#include "ef_image.h"
#include "ef_null.h"
#include "ef_sand.h"

//static const int numEffects = 14;

effect* effectTable[] =
{
  &crackles,&drops, &modchase, &pulse, &pools, &everyn, &throb, &pmarch,
  &fluid, &crawl, &flame, &image, &sand, &sparks
};
const byte numEffects = (sizeof(effectTable) / sizeof(effectTable[0]));

effect* baseEffects[] =
{
  &pulse, &pools, &throb, &pmarch, &fluid, &image, &sand, &null
};
const byte numBaseEffects = (sizeof(baseEffects) / sizeof(baseEffects[0]));

effect* layerEffects[] =
{
  &crackles, &drops, &modchase, &everyn, &crawl, &flame, &sparks, &null
};
const byte numLayerEffects = (sizeof(layerEffects) / sizeof(layerEffects[0]));

class controller : public effect
{

  public:
    // Palette animation timer
    // 10 seconds between pal changes (change to random later)
    static const unsigned long micsPerPalchange = 1000000 * 10;
    long micsTilPalChange = micsPerPalchange;
    uint8_t curPal = 0;

    // 10 seconds between base effect changes (change to random later)
    static const unsigned long micsPerBasechange = 1000000 * 27;
    long micsTilBaseChange = micsPerBasechange;

    void Init()
    {
      // A "blank" effect for simplicity and to avoid loads of NULL checks
      // everywhere
      null.Init();
      null.SetClearMode(kFade);
      null.SetFadeTime(4.0f);

      //=======================
      // LAYER effects
      //=======================
      sparks.Init();
      sparks.SetWidth(12.0);
      sparks.SetFrequency(0.5f);
      sparks.SetMirrorMode(kMirror);
      sparks.SetClearMode(kFade);
      sparks.SetPal(1);
      sparks.SetSpeed(30);

      // Redesign this to do lots of fireworks kinda stuff like the umbrealla
      crackles.Init();
      crackles.SetFrequency(60.0f);
      crackles.SetFadeTime(2.0f);
      crackles.SetPal(1);
      crackles.SetClearMode(kFade);

      // This effect sucks right now. quick fix or ditch
      drops.Init();
      drops.SetFrequency(2.0f); // Launches per second
      drops.SetFadeTime(2.0f);
      drops.SetSpeed(0.2f); // In lengths per second
      drops.SetPal(1);

      modchase.Init(10.0f);
      modchase.SetWidth(200.0f);
      modchase.SetFrequency(1.0f / 10.0f);
      modchase.SetSpeed(30.0f);
      modchase.SetPal(1);

      everyn.SetSpeed(12.0f);
      everyn.SetSpan(10);
      everyn.SetFadeTime(0.25f);
      everyn.SetClearMode(kFade);
      everyn.SetPal(1);

      // This is cool, add more concurrent traces
      crawl.Init();
      crawl.SetClearMode(kFade);
      crawl.SetPal(1);

      // ef_sand
      sand.Init();
      sand.SetPal(1);
      sand.SetClearMode(kFade);

      //=======================
      // BASE effects
      //=======================

      // This is awesome, leave it be!
      flame.Init(10.0f);
      flame.SetClearMode(kFade);
      flame.SetFadeTime(0.1f);
      flame.SetPal(0);
      flame.SetBuffer(leds[0]);
      flame.SetFrequency(2.0f);

      // This is now a layer effect
      pulse.Init();
      //pulse.SetFadeTime(0.5f);
      pulse.SetPal(0);
      pulse.SetFrequency(4.0f);
      pulse.SetFadeTime(0.1f);
      pulse.SetBuffer(leds[0]);
      pulse.SetClearMode(kFade);

      pools.Init();
      pools.SetBrightness(32);
      pools.SetPal(0);
      pools.SetBuffer(leds[0]);

      pmarch.Init(30.0f);
      pmarch.SetFrequency(4.0f);
      pmarch.SetSpeed(160);
      pmarch.SetBrightness(32);
      pmarch.SetPal(0);
      pmarch.SetBuffer(leds[0]);

      throb.Init();
      throb.SetBrightness(32);
      throb.SetPal(0);
      throb.SetBuffer(leds[0]);

      fluid.Init(10.0f);
      fluid.SetBrightness(32);
      fluid.SetPal(0);
      fluid.SetBuffer(leds[0]);

      // Copy this and make a good noise effect
      image.Init(10.0f);
      image.SetClearMode(kFade);
      image.SetFadeTime(0.1f);
      image.SetPal(0);
      image.SetBuffer(leds[0]);


      currenteffects[0] = &fluid;
      currenteffects[0]->SetBuffer(leds[0]);
      blendmodes[0] = kOverwrite;

      currenteffects[1] = &crawl;
      currenteffects[1]->SetBuffer(leds[1]);
      blendmodes[1] = kWriteIfNonBlack; // Basically overlay with any lit pixels
    }

    String Identify()
    {
      return "controller";
    }

    void Reset()
    {
      // <<TODO>> whatever we need to do to reset basically everything!
    }

    void SetBaseEffect(effect* e)
    {
      currenteffects[0] = e;
      currenteffects[0]->SetBuffer(leds[0]);
    }

    void SetLayerEffect(effect* e)
    {
      currenteffects[1] = e;
      currenteffects[1]->SetBuffer(leds[1]);
    }

    void Animate(unsigned long mics)
    {
      float dt = (float)(mics) / 1000000.0f;

      // Periodically change the palette
      micsTilPalChange -= mics;
      if (micsTilPalChange <= 0)
      {
        // For now change all palettes
        //Serial.printf("Changing palettes...\n");
        int newpal = random(0, kNumPalettes);
        // one second fade to next palette
        palmixer.SetNewPalette(0, newpal, 4.0f);
        newpal = random(0, kNumPalettes);
        // one second fade to next palette
        palmixer.SetNewPalette(1, newpal, 4.0f);
        newpal = random(0, kNumPalettes);
        // one second fade to next palette
        palmixer.SetNewPalette(2, newpal, 4.0f);

        micsTilPalChange = micsPerPalchange;
      }

      // periodically change the base layer effect
      micsTilBaseChange -= mics;
      if (micsTilBaseChange <= 0 && enableBaseChange)
      {

        int newBase = random(0, 7);
        switch (newBase)
        {
          case 0:
            // Just do flame
            SetBaseEffect(&flame);
            SetLayerEffect(&null);
            micsTilBaseChange = 1000000 * 60;
            break;

          case 1:
            // Do fluid (noise) with crawl on top
            SetBaseEffect(&fluid);
            SetLayerEffect(&crawl);
            micsTilBaseChange = 1000000 * 27;
            break;

          case 2:
            // Do fluid (noise) with crackle
            SetBaseEffect(&fluid);
            SetLayerEffect(&crackles);
            micsTilBaseChange = 1000000 * 27;
            break;

          case 3:
            // Do pools with crackle
            SetBaseEffect(&pools);
            SetLayerEffect(&everyn);
            micsTilBaseChange = 1000000 * 60;
            break;

          case 4:
            // Do fluid (fluid) with everyn
            SetBaseEffect(&fluid);
            SetLayerEffect(&everyn);
            micsTilBaseChange = 1000000 * 60;
            break;

          case 5:
            SetBaseEffect(&null);
            SetLayerEffect(&crawl);
            micsTilBaseChange = 1000000 * 20;
            break;

          case 6:
            // Just do fluid
            SetBaseEffect(&fluid);
            SetLayerEffect(&null);
            micsTilBaseChange = 1000000 * 60;
            break;

          case 7:
            SetBaseEffect(baseEffects[random(0, 7)]);
            SetLayerEffect(layerEffects[random(0, 7)]);
            micsTilBaseChange = 1000000 * 60;
        }

        micsTilBaseChange = micsPerBasechange;
      }

      palmixer.Animate(dt);

      if (currenteffects[0] != NULL)
        currenteffects[0]->Animate(mics);
      if (currenteffects[1] != NULL)
        currenteffects[1]->Animate(mics);

    }

    void Render()
    {
      currenteffects[0]->Render();
      currenteffects[1]->Render();


      // Clear the mixdown target buffer
      // NOTE: The base effects are always in overwrite mode, so this is not
      // necessary
      memset(leds[kMixDownBuffer], 0, NUM_LEDS * 3);

      // Do the mixdown
      for (int i = 0; i < 2; i++)
      {
        if (currenteffects[i] == NULL) continue;

        switch (blendmodes[i])
        {
          case kOverwrite:
            Overwrite(currenteffects[i]->GetBuffer(), leds[kMixDownBuffer]);
            break;
          case kBlendHalf:
            BlendHalf(currenteffects[i]->GetBuffer(), leds[kMixDownBuffer]);
            break;
          case kBlendIfBrighter:
            BlendIfBrighter(currenteffects[i]->GetBuffer(), leds[kMixDownBuffer]);
            break;
          case kWriteIfNonBlack:
            // <<TODO>> Use variable channels
            WriteIfNonBlack(currenteffects[i]->GetBuffer(), leds[kMixDownBuffer], 0);
            break;
          case kOverwriteIfEven:
            OverwriteIfEven(currenteffects[i]->GetBuffer(), leds[kMixDownBuffer]);
            break;
          case kChooseBrightest:
            ChooseBrightest(currenteffects[i]->GetBuffer(), leds[kMixDownBuffer]);
            break;
          case kOverwriteIfBrighter:
            OverwriteIfBrighter(
              currenteffects[i]->GetBuffer(), leds[kMixDownBuffer], 8);
            break;
          case kOff:
          default:
            // Do nothing
            break;
        }
      }
    }

} controller;

#endif
