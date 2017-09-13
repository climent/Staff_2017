#ifndef FLAME_H
#define FLAME_H
#include "commonheaders.h"

class flame : public effect
{

// Let's turn this into a "life" like simulation
public:



uint8_t palIndex = 255;

unsigned long micsperemit = ONEMIL;
int timeTillEmit = 0;
uint8_t startIndex = 0; // this is where in the palette we start, cycle this slowly to 255 and wrap around 
bool emit = false;

  public:

  
	void Init(int shiftpersec)
	{
    Serial.printf("flame Init()...\n");
    Serial.flush();
    // How many mics to wait til next change
      micsperemit = (unsigned long)((1.0f/(float)shiftpersec) * (float)ONEMIL);
      timeTillEmit = micsperemit;

      SetupFlameFront();

#if 0


    Serial.printf("About to init the test matrix\n");
    Serial.flush();

    for (int i=0; i<NUM_LEDS; i++)
    {
      // Each led has a height and width that we can use to find the best render value
      int y = (int)(GetHeight(i) * kMatrixHeight);
      int x = (int)(GetAngle(i) * kMatrixWidth);
      test[y][x] += 1;
  	}


    Serial.println("matrix coverage:");
    for (int y=0; y<kMatrixHeight; y++)
    {
      for (int x=0; x<kMatrixWidth; x++)
      {
        Serial.printf("%d ",test[y][x]);
      }
      Serial.println();
    }
#endif

  }

  String Identify()
  {
    return "image";
  }

  void Reset()
  {
    micsperemit = ONEMIL;
    timeTillEmit = 0;

  }


  void SetSpeed(float ledspersec)
  {
     effect::SetSpeed(ledspersec);
  }

  void SetFrequency(float freq)
  {
     // How many mics to wait til next change
      micsperemit = (unsigned long)((1.0f/freq) * (float)ONEMIL);
      timeTillEmit = micsperemit;
  }

	void Animate(unsigned long mics)
	{
    timeTillEmit -= mics;

    // Move the patterns when it is time
    if (timeTillEmit <= 0)
    {
      timeTillEmit = micsperemit;
      DoFlameFront();
      emit = true;
    }
	}

  void SetupFlameFront()
  {
     for (uint16_t x = 0; x < kMatrixWidth; x++)
     {
      for (uint16_t y = 0; y< kMatrixHeight; y++)
       {
         board[y][x] = 255;
         work[y][x] = 255;
         test[y][x] = 0;
       }
     }

     // If aggressive, set 4 chunks on fire randomly distributed along the staff
     if (aggressive)
     {
      board[random(kMatrixHeight)][random(kMatrixWidth)] = 128; // set one chunk on fire!
      board[random(kMatrixHeight)][random(kMatrixWidth)] = 128; // set one chunk on fire!
      board[random(kMatrixHeight)][random(kMatrixWidth)] = 128; // set one chunk on fire!
      board[random(kMatrixHeight)][random(kMatrixWidth)] = 128; // set one chunk on fire!
     }
     else
     {
     // else just two chunks at roughly one third along each
       board[random(kMatrixHeight)][random(kMatrixWidth)] = 128; // set one chunk on fire!
       // else just two chunks at roughly one third along each
       board[random(kMatrixHeight)][random(kMatrixWidth)] = 128; // set one chunk on fire!

     }

  }

  void DoFlameFront()
  {
    bool stillBurning = false;

     for (uint16_t x = 0; x < kMatrixWidth; x++)
     {
      for (uint16_t y = 0; y< kMatrixHeight; y++)
       {
        bool canPass = false;

        // if this entry is on fire, look to nearby entries, and with varying probability, set them alight as well
        // if the entry is dead/burned out, skip it
        if (board[y][x] == 0) continue;



        if (board[y][x] == 128)
        {
          stillBurning = true;
          // This one is burning. look to light neighbors, and if one or more lights

          // Check up
          if (y<kMatrixHeight-1)
            if (board[y+1][x] == 255 ) // if unburned
            {
              canPass = true;
              if (random8() > 210)
              {
                board[y+1][x] = 128; // on fire now
                //board[y][x] = 127;// passed on the fire, and ready to flame out after render
              }
            }

          // Check down
          if (y>0)
            if (board[y-1][x] == 255) // if unburned
            {
              canPass = true;
              if (random8() > 210)
              {
                board[y-1][x] = 128; // on fire now
                //board[y][x] = 127;// passed on the fire, and ready to flame out after render
              }
            }

          // Check right with wrap
          uint8_t wx = x+1;
          if (wx == kMatrixWidth) wx = 0;
            if (board[y][wx] == 255) // if unburned
            {
              canPass = true;
              if (random8() > 240)
              {
                board[y][wx] = 128; // on fire now
                //board[y][x] = 127;// passed on the fire, and ready to flame out after render
              }
            }

          // Check left with wrap
          wx = x-1;
          if (x == 0) wx = kMatrixWidth-1;
            if (board[y][wx] == 255) // if unburned
            {
              canPass = true;
              if (random8() > 245)
              {
                board[y][wx] = 128; // on fire now
                //board[y][x] = 127;// passed on the fire, and ready to flame out after render
              }
            }

          if (!canPass)
            board[y][x] = 127; // Flame out if a lonely spark with nowhere to go :)
        }
      }
    }      

    if (!stillBurning)
      SetupFlameFront(); // start again!
  }


	void Render()
	{
    FadeOrClear();
  for (int i=0; i<NUM_LEDS; i++)
  {
    // Each led has a height and width that we can use to find the best render value
    int y = (int)(GetHeight(i) * kMatrixHeight);
    int x = (int)(GetAngle(i) * kMatrixWidth);
    // get the burning value
    uint8_t bv = board[y][x];

    if (bv == 128 || bv == 127) // burning 
    {
      //dst[i] = finalPalette[0][work[y][x]--];
      SetPixel(i,finalPalette[0][work[y][x]--]);
      if (work[y][x] == 0) // passed the fire on to others, can flame out now
      {
        board[y][x] = 0; // burn out
      }
    }

    
  }    

  }
  
} flame;
#endif
