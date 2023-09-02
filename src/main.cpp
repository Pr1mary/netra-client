#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// change value here
#define BAUDRATE 19200
#define DELAY_MS 1000
#define TIMEOUT_MS 5000
#define ANIM_MS 1000
#define LCD_COLS 16
#define LCD_ROWS 2

// using PCF8574 for LCD I2C adapter where default address is 0x27
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);

String cmd = "";
bool wait = false, isConnect = false;
// counter
int currIndex = 0, animCount = 0;
// specific use case timing
unsigned long animTimer = 0, cmdTimer = 0, timeoutTimer = 0;
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
// command from and to cli
void cmdFromCli(String cmd)
{
  if (cmd == "STATUS")
    Serial.println("ALIVE");
}

// command from service
void cmdFromService(String cmd)
{
  if (cmd == "ALIVE")
  {
    timeoutTimer = 0;
    isConnect = true;
  }
  if (cmd.indexOf("IP ") == 0)
  {
    cmd.remove(0, 3);
    writeLcd("IP ADDR:");
    writeLcd(cmd);
    Serial.println("OK");
    timeoutTimer = 0;
    isConnect = true;
  }
  if (cmd.indexOf("SEND ") == 0)
  {
    cmd.remove(0, 5);
    writeLcd(cmd);
    Serial.println("OK");
  }
  if (cmd == "CLEAR")
  {
    lcd.clear();
    Serial.println("OK");
  }
}

// command to service
void cmdToService(unsigned long delayTimeMs, unsigned long maxTimeout)
{
  if (delayTimeMs > maxTimeout)
  {
    return;
  }
  cmdTimer += _globalDeltaTime;
  timeoutTimer += _globalDeltaTime;
  if (cmdTimer > delayTimeMs)
  {
    if (isConnect)
    {
      Serial.println("WHEREIP");
    }
    else
    {
      Serial.println("STATUS");
    }
    cmdTimer = 0;
  }
  if (isConnect && timeoutTimer > maxTimeout)
  {
    animCount = 0;
    cmdTimer = 0;
    timeoutTimer = 0;
    isConnect = false;
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
  pinMode(13, OUTPUT);
  Serial.begin(BAUDRATE);
  while (!Serial)
    ;
}

void loop()
{
  updateDeltaTime();
  cmdToService(DELAY_MS, TIMEOUT_MS);

  if (!isConnect)
  {
    waitAnim(ANIM_MS);
    digitalWrite(13, LOW);
  }

  if (isConnect)
  {
    digitalWrite(13, HIGH);
  }

  if (Serial.availableForWrite() > 0)
  {
    cmd = Serial.readString();
    cmd.trim();
    if (cmd != "")
    {
      cmdFromService(cmd);
    }
  }
}
