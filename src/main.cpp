#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <EEPROM.h>

int outOneUSeconds = 750;
int outTwoUSeconds = 750;
int outThreeUSeconds = 750;
bool onToggle = true;
//array with high pin number
bool on[D8];
bool waitToPersist = false;
// [key][values for pin D5, D6, D7]
int persistetValues[4][3];
uint64_t lastPressed;

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
const long KEY_ENCORE = 18446744073709551615;

IRrecv irRecv(irRecPin);
decode_results irResults;

void processIRCommand(uint64_t cmdInt);
void performLEDPWM();
void toggleOnOff();
void increasePWM(int *var, int val, int pin);
void decreasePWM(int *var, int val, int pin);
void persist(uint64_t pin);
void load(uint64_t pin);

struct State
{
  int red;
  int green;
  int blue;
};

void setup()
{
  Serial.begin(115200);
  irRecv.enableIRIn();

  //D5= RED!!
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);

  on[D5] = true;
  on[D6] = true;
  on[D7] = true;
  Serial.println("Starting...");
}

void loop()
{
  if (irRecv.decode(&irResults))
  {
    if (irResults.value == KEY_ENCORE && lastPressed != 0)
    {
      processIRCommand(lastPressed);
    }
    else
    {
      processIRCommand(irResults.value);
    }

    irRecv.resume();
  }
  delay(1);

  performLEDPWM();
}

void processIRCommand(uint64_t cmdInt)
{
  //If not waiting to persist, remove the flag
  if (waitToPersist && !(cmdInt == KEY_SEVEN || cmdInt == KEY_EIGHT || cmdInt == KEY_NINE || cmdInt == KEY_ZERO || cmdInt == KEY_ENCORE))
  {
    waitToPersist = false;
  }

  switch (cmdInt)
  {
  case KEY_POWER:
    toggleOnOff();
    break;

  case KEY_DOWN:
    decreasePWM(&outOneUSeconds, 50, D5);
    decreasePWM(&outTwoUSeconds, 50, D6);
    decreasePWM(&outThreeUSeconds, 50, D7);
    lastPressed = cmdInt;
    break;

  case KEY_UP:
    increasePWM(&outOneUSeconds, 50, D5);
    increasePWM(&outTwoUSeconds, 50, D6);
    increasePWM(&outThreeUSeconds, 50, D7);
    lastPressed = cmdInt;
    break;

  case KEY_ONE:
    Serial.printf("KEY_ONE: %d \n", outOneUSeconds);
    increasePWM(&outOneUSeconds, 50, D5);
    lastPressed = cmdInt;
    break;

  case KEY_TWO:
    Serial.printf("KEY_TWO: %d \n", outTwoUSeconds);
    increasePWM(&outTwoUSeconds, 50, D6);
    lastPressed = cmdInt;
    break;

  case KEY_THREE:
    Serial.printf("KEY_THREE: %d \n", outThreeUSeconds);
    increasePWM(&outThreeUSeconds, 50, D7);
    lastPressed = cmdInt;
    break;
  case KEY_FOUR:
    Serial.printf("KEY_FOUR: %d \n", outOneUSeconds);
    decreasePWM(&outOneUSeconds, 50, D5);
    lastPressed = cmdInt;
    break;

  case KEY_FIVE:
    Serial.printf("KEY_FIVE: %d \n", outTwoUSeconds);
    decreasePWM(&outTwoUSeconds, 50, D6);
    lastPressed = cmdInt;
    break;
  case KEY_SIX:
    Serial.printf("KEY_SIX: %d \n", outThreeUSeconds);
    decreasePWM(&outThreeUSeconds, 50, D7);
    lastPressed = cmdInt;
    break;
  case KEY_SEVEN:
  case KEY_EIGHT:
  case KEY_NINE:
  case KEY_ZERO:
    if (waitToPersist)
    {
      persist(cmdInt);
      waitToPersist = false;
    }
    else
    {
      load(cmdInt);
    }
    lastPressed = 0;
    break;
  case KEY_ENTER:
    waitToPersist = true;
    lastPressed = 0;
    break;
  default:
    Serial.println(cmdInt);
    lastPressed = 0;
    break;
  }
}

void persist(uint64_t key)
{
  int keyStore;

  switch (key)
  {
  case KEY_SEVEN:
    keyStore = 7;
    break;
  case KEY_EIGHT:
    keyStore = 8;
    break;
  case KEY_NINE:
    keyStore = 9;
    break;
  case KEY_ZERO:
    keyStore = 0;
    break;

  default:
    return;
    break;
  }

  State state = {outOneUSeconds, outTwoUSeconds, outThreeUSeconds};
  Serial.printf("Writing to EEPROM: key %d, red %d, green %d, blue %d\n", keyStore, outOneUSeconds, outTwoUSeconds, outThreeUSeconds);
  EEPROM.begin(512);
  EEPROM.put(keyStore, state);
  EEPROM.commit();
  EEPROM.end();
}

void load(uint64_t key)
{
int keyStore;

  switch (key)
  {
  case KEY_SEVEN:
    keyStore = 7;
    break;
  case KEY_EIGHT:
    keyStore = 8;
    break;
  case KEY_NINE:
    keyStore = 9;
    break;
  case KEY_ZERO:
    keyStore = 0;
    break;

  default:
    return;
    break;
  }

  State state;
  EEPROM.begin(512);
  EEPROM.get(keyStore, state);
  EEPROM.end();
  Serial.printf("Read from EEPROM: key %d, red %d, green %d, blue %d\n", keyStore, state.red, state.green, state.blue);
  /* if(state == NULL) {
  } else {
    Serial.println("Could not read from EEPROM!");
  } */
}

void performLEDPWM()
{
  if (!onToggle)
  {
    return;
  }

  for (int i = 0; i < 1000; i++)
  {

    if (on[D5] && outOneUSeconds >= i)
    {
      digitalWrite(D5, HIGH);
    }
    else
    {
      digitalWrite(D5, LOW);
    }

    if (on[D6] && outTwoUSeconds >= i)
    {
      digitalWrite(D6, HIGH);
    }
    else
    {
      digitalWrite(D6, LOW);
    }

    if (on[D7] && outThreeUSeconds >= i)
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
  if (onToggle)
  {
    Serial.println("Toggle off");
    onToggle = false;
    on[D5] = false;
    on[D6] = false;
    on[D7] = false;
    digitalWrite(D5, LOW);
    digitalWrite(D6, LOW);
    digitalWrite(D7, LOW);
  }
  else
  {
    Serial.println("Toggle on");
    on[D5] = true;
    on[D6] = true;
    on[D7] = true;
    onToggle = true;
  }
}

void increasePWM(int *var, int val, int pin)
{
  on[pin] = true;
  int maxVal = 850;

  if (pin == D5)
  {
    maxVal = 1000;
  }

  if (on[pin] && *var < maxVal)
  {
    *var += val;
    Serial.printf("increasePWM, pin: %d, val: %d\n", pin, *var);
  }
}

void decreasePWM(int *var, int val, int pin)
{
  if (on[pin] && *var > 0)
  {
    digitalWrite(pin, HIGH);
    *var -= val;
    Serial.printf("decreasePWM, pin: %d, val: %d\n", pin, *var);
  }
  else
  {
    digitalWrite(pin, LOW);
    on[pin] = false;
  }
}
