#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// main section
String cmd = "";
String frstLine = "";
String scndLine = "";
int currIndex = 0;

int end = 13, start = 0;

void writeLcd(String text);
void writeLcdFull(String text);
void movingText();

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.println("\nSerial connected!");
  Serial.println("Initialize LCD...");
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  Serial.println("LCD Initialized!");
}

int id = 13;

void loop() {
  // movingText();
  if (Serial.available() > 0) {
    cmd = Serial.readString();
    cmd.trim();
    if (cmd.length() > 16) {
      cmd.remove(13);
      cmd += "...";
    }
    Serial.print("Command: ");
    Serial.println(cmd);
    writeLcdFull(cmd);
    // frstLine = cmd;
  }
}

void writeLcd(String text) {
  lcd.setCursor(0, currIndex);
  if (currIndex == 0) {
    lcd.clear();
    currIndex++;
  } else {
    currIndex--;
  }
  lcd.print(text);
}

void writeLcdFull(String text) {
  if (currIndex == 0) {
    frstLine = text;
  } else {
    scndLine = text;
  }
  writeLcd(text);
}

void movingText() {
  String temp = frstLine;
  if (end == frstLine.length()) {
    end = 13;
    start = 0;
  }
  if (frstLine.length() > 16) {
    temp.trim();
    if (temp.length() > 16) {
      temp.remove(end);
      temp += "...";
    }
  }
  lcd.setCursor(0, 0);
  lcd.print(temp);
  delay(500);
  end++;
  start++;
}
