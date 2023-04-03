#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

String cmd = "";
int currIndex = 0;

void writeLcd(String text) {
  String in_txt = text;
  if (in_txt.length() > 16) {
    in_txt.remove(13);
    in_txt += "...";
  }
  lcd.setCursor(0, currIndex);
  if (currIndex == 0) {
    lcd.clear();
    currIndex++;
  } else {
    currIndex--;
  }
  lcd.print(in_txt);
}

void cmdProcessor(String cmd){
  if(cmd == "STATUS"){
    Serial.println("UP");
    return;
  }
  if(cmd.indexOf("SEND ") == 0){
    cmd.remove(0, 5);
    writeLcd(cmd);
  }
  return;
}

void setup() {
  // pinMode(6, INPUT);
  // pinMode(13, OUTPUT);
  Serial.begin(19200);
  while (!Serial) {
    ;
  }
  Serial.println("UP");
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  if (Serial.availableForWrite() > 0) {
    cmd = Serial.readString();
    cmd.trim();
    cmdProcessor(cmd);
  }
}
