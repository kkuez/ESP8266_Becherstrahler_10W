#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

int outOneUSeconds = 1000;
int outTwoUSeconds = 1000;
int outThreeUSeconds = 1000;
bool on = true;

const int irRecPin = D3;
const uint64_t KEY_POWER = 3974070404;
const uint64_t KEY_DOWN = 2140684872;
const uint64_t KEY_UP = 3321326852;
const uint64_t KEY_ONE = 825816032;
const uint64_t KEY_TWO = 3207847040;
const uint64_t KEY_THREE = 2880933152;
const uint64_t KEY_FOUR = 1250339008;
const uint64_t KEY_FIVE = 3222488448;
const uint64_t KEY_SIX = 3121764036;
const uint64_t KEY_SEVEN = 1386404388;
const uint64_t KEY_EIGHT = 3075245088;
const uint64_t KEY_NINE = 3809946656;
const uint64_t KEY_ZERO = 798946180;
const uint64_t KEY_VOL_UP = 4129915364;
const uint64_t KEY_VOL_DOWN = 2537318624;
const uint64_t KEY_CHAN_UP = 2625382880;
const uint64_t KEY_CHAN_DOWN = 457390884;
const uint64_t KEY_ENTER = 1008447872;

IRrecv irRecv(irRecPin);
decode_results irResults;

void processIRCommand(uint64_t cmdInt);
void performLEDPWM();
void toggleOnOff();
void increasePWM(int *var, int val);
void decreasePWM(int *var, int val);

void setup()
{
  Serial.begin(115200);
  irRecv.enableIRIn();

  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);

  Serial.println("Starting...");
}

void loop()
{
  if (irRecv.decode(&irResults))
  {
    processIRCommand(irResults.value);
    irRecv.resume();
  }
  delay(1);

  performLEDPWM();
}

void processIRCommand(uint64_t cmdInt)
{
  switch (cmdInt)
  {
  case KEY_POWER:
    toggleOnOff();
    break;

  case KEY_DOWN:
    decreasePWM(&outOneUSeconds, 50);
    decreasePWM(&outTwoUSeconds, 50);
    decreasePWM(&outThreeUSeconds, 50);
    break;

  case KEY_UP:
    increasePWM(&outOneUSeconds, 50);
    increasePWM(&outTwoUSeconds, 50);
    increasePWM(&outThreeUSeconds, 50);
    break;

  case KEY_ONE:
    Serial.printf("KEY_ONE: %d \n", outOneUSeconds);
    increasePWM(&outOneUSeconds, 50);
    break;

  case KEY_TWO:
    Serial.printf("KEY_TWO: %d \n", outTwoUSeconds);
    increasePWM(&outTwoUSeconds, 50);
    break;

  case KEY_THREE:
    Serial.printf("KEY_THREE: %d \n", outThreeUSeconds);
    increasePWM(&outThreeUSeconds, 50);
    break;
  case KEY_FOUR:
    Serial.printf("KEY_FOUR: %d \n", outOneUSeconds);
    decreasePWM(&outOneUSeconds, 50);
    break;

  case KEY_FIVE:
    Serial.printf("KEY_FIVE: %d \n", outTwoUSeconds);
    decreasePWM(&outTwoUSeconds, 50);
    break;
  case KEY_SIX:
    Serial.printf("KEY_SIX: %d \n", outThreeUSeconds);
    decreasePWM(&outThreeUSeconds, 50);
    break;
  default:
    break;
  }
}

void performLEDPWM()
{
  if (!on)
  {
    return;
  }

  for (int i = 0; i < 1000; i++)
  {

    if (outOneUSeconds >= i)
    {
      digitalWrite(D5, HIGH);
    }
    else
    {
      digitalWrite(D5, LOW);
    }

    if (outTwoUSeconds >= i)
    {
      digitalWrite(D6, HIGH);
    }
    else
    {
      digitalWrite(D6, LOW);
    }

    if (outThreeUSeconds >= i)
    {
      digitalWrite(D7, HIGH);
    }
    else
    {
      digitalWrite(D7, LOW);
    }
    delayMicroseconds(1);
  }
}

void toggleOnOff()
{
  if (on)
  {
    digitalWrite(D5, LOW);
    digitalWrite(D6, LOW);
    digitalWrite(D7, LOW);
    on = false;
  }
  else
  {
    on = true;
  }
}

void increasePWM(int *var, int val)
{
  if (on && *var < 1000)
  {
    *var += val;
  }
}

void decreasePWM(int *var, int val)
{
  if (on && *var > 0)
  {
    *var -= val;
  }
}
