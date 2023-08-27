#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// change value here
#define BAUDRATE 19200
#define DELAY_MS 100
#define MAX_TIMEOUT 1

// using PCF8574 for LCD I2C adapter where default address is 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

String cmd = "";
bool wait = false, isConnect = false;
int currIndex = 0, animCount = 0, timeout = 0;

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
      timeout = 0;
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

void timeoutProcess(int delayTimeMs, int maxFail)
{
  delay(delayTimeMs);
  if (isConnect)
  {
    Serial.println("STATUS");
    timeout++;
    if (timeout > maxFail)
    {
      animCount = 0;
      timeout = 0;
      isConnect = false;
    }
  }
}

int waitAnim(int counter)
{
  switch (counter)
  {
  case 0:
    writeLcd("Waiting.");
    break;
  case 1:
    writeLcd("Waiting..");
    break;
  case 2:
    writeLcd("Waiting...");
    break;
  default:
    break;
  }
  writeLcd("System not ready");
  counter++;
  if (counter > 2)
    return 0;
  return counter;
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
  if (!Serial.available() && !isConnect)
    animCount = waitAnim(animCount);

  if (!isConnect)
    Serial.println("UP");

  if (Serial.availableForWrite() > 0)
  {
    cmd = Serial.readString();
    cmd.trim();
    cmdFromService(cmd);
  }

  timeoutProcess(DELAY_MS, MAX_TIMEOUT);
}
