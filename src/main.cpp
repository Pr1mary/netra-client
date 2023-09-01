#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// change value here
#define BAUDRATE 19200
#define DELAY_MS 1000
#define TIMEOUT_MS 5000
#define ANIM_MS 1000

// using PCF8574 for LCD I2C adapter where default address is 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

String cmd = "";
bool wait = false, isConnect = false;
// counter
int currIndex = 0, animCount = 0;
// specific use case timing
unsigned long animTimer = 0, cmdTimer = 0;
// used for timer
unsigned long _globalLastTime = 0, _globalDeltaTime = 0;

void updateDeltaTime()
{
  unsigned long currTime = millis();
  _globalDeltaTime = currTime - _globalLastTime;
  _globalLastTime = currTime;
}

void writeLcd(String text)
{
  String in_txt = text;
  if (in_txt.length() > 16)
  {
    in_txt.remove(13);
    in_txt += "...";
  }
  lcd.setCursor(0, currIndex);
  if (currIndex == 0)
  {
    lcd.clear();
    currIndex++;
  }
  else
  {
    currIndex--;
  }
  lcd.print(in_txt);
}
// command from cli
void cmdFromCli(String cmd)
{
  if (cmd == "STATUS")
    Serial.println("OK");
}

// command from service
void cmdFromService(String cmd)
{
  if (cmd != "")
  {
    if (cmd == "OK")
    {
      cmdTimer = 0;
    }
    else if (cmd.indexOf("IP ") == 0)
    {
      cmd.remove(0, 3);
      writeLcd("IP ADDR:");
      writeLcd(cmd);
    }
    else if (cmd.indexOf("SEND ") == 0)
    {
      cmd.remove(0, 5);
      writeLcd(cmd);
    }
    else if (cmd == "CLEAR")
    {
      lcd.clear();
    }
    isConnect = true;
  }
}

void timeoutProcess(HardwareSerial Serial, unsigned long delayTimeMs, unsigned long maxTimeout)
{
  cmdTimer += _globalDeltaTime;
  if (isConnect)
  {
    if (cmdTimer > delayTimeMs)
    {
      Serial.println("STATUS");
      cmdTimer = 0;
    }
    if (cmdTimer > maxTimeout)
    {
      animCount = 0;
      cmdTimer = 0;
      isConnect = false;
    }
  }
  else
  {
    cmdTimer = 0;
  }
}

void waitAnim(unsigned long animTimerMax)
{
  animTimer += _globalDeltaTime;
  if (animTimer > animTimerMax)
  {
    switch (animCount)
    {
    case 0:
      writeLcd("Waiting");
      break;
    case 1:
      writeLcd("Waiting.");
      break;
    case 2:
      writeLcd("Waiting..");
      break;
    case 3:
      writeLcd("Waiting...");
      break;
    default:
      break;
    }
    writeLcd("System not ready");
    animCount++;
    animTimer = 0;
  }
  if (animCount > 3)
    animCount = 0;
}

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.clear();
  Serial.begin(BAUDRATE);
  while (!Serial)
    ;
}

void loop()
{
  updateDeltaTime();

  if (!Serial.available() && !isConnect)
    waitAnim(ANIM_MS);

  if (!isConnect)
    Serial.println("UP");

  if (Serial.availableForWrite() > 0)
  {
    cmd = Serial.readString();
    cmd.trim();
    cmdFromService(cmd);
    timeoutProcess(Serial, DELAY_MS, TIMEOUT_MS);
  }
}
