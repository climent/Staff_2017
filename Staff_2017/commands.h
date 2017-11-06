#ifndef COMMANDS_H
#define COMMANDS_H
#include "commonheaders.h"
#include "controls.h"
#include "SerialFlash.h"

static const int MAX_BUF = 64;
static const int MAX_ARGS = 12;
int numInput = 0;
char inputBuffer[MAX_BUF];
char *command;
char *args[MAX_ARGS];
int numArgs = 0;


char scriptBuf[1024]; // Maximum script length of 1K for now
int scriptBufIndex = 0;
int scriptBufSize = 0;

long micsToPause = 0; // A master pause control that can serve as a "delay" command during script execution


effect* selectedEffect = NULL;


effect* EffectByName(const char* name)
{
  Serial.print("Looking up effect: "); Serial.println(name);

  if (strcmp(name, "crackles") == 0) return &crackles;
  else if (strcmp(name, "drops") == 0) return &drops;
  else if (strcmp(name, "modchase") == 0) return &modchase;
  else if (strcmp(name, "pulse") == 0) return &pulse;
  else if (strcmp(name, "pools") == 0) return &pools;
  else if (strcmp(name, "everyn") == 0) return &everyn;
  else if (strcmp(name, "throb") == 0) return &throb;
  else if (strcmp(name, "pmarch") == 0) return &pmarch;
  else if (strcmp(name, "fluid") == 0) return &fluid;
  else if (strcmp(name, "crawl") == 0) return &crawl;
  else if (strcmp(name, "flame") == 0) return &flame;
  else if (strcmp(name, "null") == 0) return &null;
  else if (strcmp(name, "random") == 0)
  {
    int selection = random(0, numEffects);
    return effectTable[selection];
  }
  else
  {
    Serial.println("Unrecognized effect!");
    return &null;
  }

}

void SelectEffect(const char* name)
{
  selectedEffect = EffectByName(name);
}

// Set an effect to a slot, and also select it for further manipulation
void SetEffect(int slot, const char *name)
{
  if (slot < 0 || slot > 1) return;
  selectedEffect = EffectByName(name);
  if (selectedEffect != NULL)
    currenteffects[slot] = selectedEffect;
}

void SetBlendMode(int slot, int mode)
{
  if (slot < 0 || slot > 1) return;
  if (mode < 0 || mode > kOff) return;
  blendmodes[slot] = static_cast<MixMode>(mode);
}

void SetClearMode(int slot, int mode)
{
  if (slot < 0 || slot > 1) return;
  if (mode < 0 || mode > effect::kFade) return;
  if (currenteffects[slot] != NULL)
  {
    Serial.print("Setting clearmode: "); Serial.println(mode);
    currenteffects[slot]->SetClearMode(static_cast<effect::ClearMode>(mode));
  }
}

void SetFadeTime(int slot, float fadetime)
{
  if (slot < 0 || slot > 1) return;
  if (currenteffects[slot] != NULL)
  {
    Serial.print("Setting fadetime: "); Serial.println(fadetime);
    currenteffects[slot]->SetFadeTime(fadetime);
  }
}


void SetSpeed(int slot, float speed)
{
  if (slot < 0 || slot > 1) return;
  if (currenteffects[slot] != NULL)
  {
    Serial.print("Setting speed: "); Serial.println(speed);
    currenteffects[slot]->SetSpeed(speed);
  }
}

void SetFrequency(int slot, float freq)
{
  if (slot < 0 || slot > 1) return;
  if (currenteffects[slot] != NULL)
  {
    Serial.print("Setting freq: "); Serial.println(freq);
    currenteffects[slot]->SetFrequency(freq);
  }
}


void SetWidth(int slot, float w)
{
  if (slot < 0 || slot > 1) return;
  if (currenteffects[slot] != NULL)
  {
    Serial.print("Setting width: "); Serial.println(w);
    currenteffects[slot]->SetWidth(w);
  }
}


void SplitCommand()
{

  numArgs = 0;
  command = strtok (inputBuffer, "  ,");
  //  Serial.print("command is: "); Serial.println(command);
  if (command == NULL) return;

  do {
    args[numArgs] = strtok (NULL, " ,");
    numArgs++;
  } while (args[numArgs - 1] != NULL);
  numArgs--;
  Serial.print("numargs is: "); Serial.println(numArgs);
}

void PrintState()
{
  Serial.print("slot0: ");
  if (currenteffects[0] != NULL)
  {
    currenteffects[0]->PrintState();
  }
  else Serial.println("NULL");

  Serial.print("slot1: ");
  if (currenteffects[1] != NULL)
  {
    currenteffects[1]->PrintState();
  }
  else Serial.println("NULL");


  Serial.print("selectedEffect: ");
  if (selectedEffect != NULL)
  {
    Serial.println(selectedEffect->Identify());
  }
  else Serial.println("NULL");


  Serial.println("blendmodes: ");
  Serial.println(blendmodes[0]);
  Serial.println(blendmodes[1]);

}

void PrintHelp()
{
  Serial.printf("Commandlist:\n");
  Serial.println("========================");

  Serial.printf("Global state commands:\n");
  Serial.printf("amps <milliamps>\n");
  Serial.printf("globalbright <brightness 0-255>\n");
  Serial.println();

  Serial.printf("Ways to select an effect for modification:\n");
  Serial.printf("select <effectname>\n");
  Serial.printf("selectslot <slot>\n");
  Serial.println();

  Serial.printf("Ways to modify selected effect:\n");
  Serial.printf("pal <pal 0-2>\n");
  Serial.printf("speed <speed>\n");
  Serial.printf("span <span>\n");
  Serial.printf("width <width>\n");
  Serial.printf("freq <Hz>\n");
  Serial.printf("fadetime <seconds>\n");
  Serial.println();

  Serial.printf("Ways to modify the slots:\n");
  Serial.printf("seteffect <slot> <effectname>\n");
  Serial.printf("blendmode <slot> <mode>\n");
  Serial.printf("clearmode <slot> <mode>\n");
  Serial.println();
}

void DoCommand()
{
  // Split the string into command and args
  SplitCommand();

  Serial.print("Command: "); Serial.println(command);
  for (int i = 0; i < numArgs; i++)
  {
    Serial.print("Arg: "); Serial.println(args[i]);
  }


  if (strcmp(command, "amps") == 0)
  {
    uint32_t A = atoi(args[0]);
    Serial.println("doing amps < milliamps> command");
    Serial.println(A);
    // limit my draw to A amps at 5v of power draw
    set_max_power_in_volts_and_milliamps(5, A);
  }
  else if (strcmp(command, "speed") == 0)
  {
    float S = atof(args[0]);
    Serial.println("doing speed <speed> command");
    Serial.println(S);
    if (selectedEffect)
      selectedEffect->SetSpeed(S);
  }
  else if (strcmp(command, "span") == 0)
  {
    int S = atoi(args[0]);
    Serial.println("doing span <span> command");
    Serial.println(S);
    if (selectedEffect)
      selectedEffect->SetSpan(S);
  }
  else if (strcmp(command, "width") == 0)
  {
    float W = atof(args[0]);
    Serial.println("doing width <width> command");
    Serial.println(W);
    if (selectedEffect)
      selectedEffect->SetWidth(W);
  }
  else if (strcmp(command, "bright") == 0)
  {
    float B = atoi(args[0]);
    Serial.println("doing bright <bright 0-255> command");
    Serial.println(B);
    if (selectedEffect)
      selectedEffect->SetBrightness(B);
  }
  else if (strcmp(command, "pal") == 0)
  {
    float B = atoi(args[0]);
    Serial.println("doing pal <pal 0-2> command");
    Serial.println(B);
    if (selectedEffect)
      selectedEffect->SetPal(B);
  }
  else if (strcmp(command, "freq") == 0)
  {
    float F = atof(args[0]);
    Serial.println("doing freq <persecond> command");
    Serial.println(F);
    if (selectedEffect)
      selectedEffect->SetFrequency(F);
  }
  else if (strcmp(command, "fadetime") == 0)
  {
    float F = atof(args[0]);
    Serial.println("doing fadetime  <seconds> command");
    Serial.println(F);
    if (selectedEffect)
      selectedEffect->SetFadeTime(F);
  }
  else if (strcmp(command, "select") == 0)
  {
    Serial.println("doing select <effect> command");
    SelectEffect(args[0]);
  }
  else if (strcmp(command, "selectslot") == 0)
  {
    Serial.println("doing selectslot <slot> command");
    int slot = atoi(args[0]);
    if (slot >= 0 && slot < 3)
      selectedEffect = currenteffects[slot];
  }
  else if (strcmp(command, "seteffect") == 0)
  {
    int slot = atoi(args[0]);
    if (slot < 0 || slot > 2) goto ERR;
    Serial.println("doing seteffect <slot> <effect> command");
    SetEffect(slot, args[1]);
    if (currenteffects[slot] != NULL)
      currenteffects[slot]->SetBuffer(leds[slot]);
  }
  else if (strcmp(command, "blendmode") == 0)
  {
    Serial.println("doing blendmode <slot> <mode> command");
    int slot = atoi(args[0]);
    int mode = atoi(args[1]);
    SetBlendMode(slot, mode);
  }
  else if (strcmp(command, "clearmode") == 0)
  {
    Serial.println("doing clearmode <slot> <mode> command");
    int slot = atoi(args[0]);
    int mode = atoi(args[1]);
    SetClearMode(slot, mode);
  }
  else if (strcmp(command, "globalbright") == 0)
  {
    Serial.println("doing globalbright command");
    int bright = atoi(args[0]);
    FastLED.setBrightness(bright);
  }
  else if (strcmp(command, "delay") == 0)
  {
    Serial.println("doing delay <seconds> command");
    float delay = atof(args[0]);

    micsToPause = (long)(delay * 1000000.0f);
  }
  else if (strcmp(command, "state") == 0)
  {
    Serial.println("doing print state command");
    PrintState();
  }
  else if (strcmp(command, "help") == 0)
  {
    Serial.println("doing print help command");
    PrintHelp();
  }

  goto DONE;

ERR:
  Serial.println("Syntax error in command processor!");

DONE:

  // Reset the command collectors
  numInput = 0;
  numArgs = 0;
  for (int i = 0; i < MAX_ARGS; i++) args[i] = NULL;
  command = NULL;
}

bool ReadOneScriptLine()
{
  if (scriptBufIndex >= scriptBufSize) return false;

  while (scriptBufIndex < scriptBufSize)
  {
    char c = scriptBuf[scriptBufIndex++];
    if (c == '\n')
    {
      inputBuffer[numInput++] = 0;
      Serial.println("command entered!");
      DoCommand();
      return true;
    }
    else if (numInput < MAX_BUF)
    {
      inputBuffer[numInput++] = c;
    }
    else
    {
      Serial.println("Input buffer overflow!");
      numInput = 0;
      numArgs = 0;
      for (int i = 0; i < MAX_ARGS; i++) args[i] = NULL;
      command = NULL;
      return false;
    }
  }
  return false; // Must be done with data
}

void RunScript()
{

  Serial.println("Attempting to open flash file...");

  SerialFlashFile file;
  file = SerialFlash.open("scripts.txt");
  if (file) {
    Serial.println("Couldn't find the file");
  } else {
    Serial.println("Found the file");
  }

  scriptBufSize = file.size();
  Serial.print("File size is: "); Serial.println(scriptBufSize);
  Serial.println("Reading file");
  file.read(scriptBuf, scriptBufSize);
  while (ReadOneScriptLine())
  {
    Serial.println("Processed one script line");
  }
}


bool ProcessInput(unsigned long mics)
{
  if (micsToPause > 0)
  {
    micsToPause -= mics;
    if (micsToPause > 0) return false;
  }

  while (Serial.available() > 0) {
    // read the incoming byte:
    char c = Serial.read();
    if (c == '\n')
    {
      inputBuffer[numInput++] = 0;
      Serial.println("command entered!");
      DoCommand();
      return true;
    }
    else if (numInput < MAX_BUF)
    {
      inputBuffer[numInput++] = c;
    }
    else
    {
      Serial.println("Input buffer overflow!");
      numInput = 0;
    }
  }
  return false;
}

#endif
