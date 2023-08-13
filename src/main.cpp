#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

String cmd = "";
bool wait = false, isConnect = false;
int currIndex = 0, animCount = 0;

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

void cmdProcessor(String cmd)
{
  if (cmd == "STATUS")
  {
    Serial.println("UP");
    isConnect = true;
    return;
  }
  if (cmd.indexOf("IP ") == 0)
  {
    cmd.remove(0, 3);
    writeLcd("IP ADDR:");
    writeLcd(cmd);
    isConnect = true;
    return;
  }
  if (cmd.indexOf("SEND ") == 0)
  {
    cmd.remove(0, 5);
    writeLcd(cmd);
    isConnect = true;
    return;
  }
}

void setup()
{
  // pinMode(6, INPUT);
  // pinMode(13, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  Serial.begin(19200);
  while (!Serial)
    ;
}

void loop()
{
  if (!Serial.available())
  {
    if (!isConnect)
    {
      switch (animCount)
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
      Serial.println("UP");
      animCount++;
      if (animCount > 2)
        animCount = 0;
      delay(250);
    }
  }

  if (Serial.availableForWrite() > 0)
  {
    cmd = Serial.readString();
    cmd.trim();
    cmdProcessor(cmd);
  }
}
