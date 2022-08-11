#include "displayControl.h"
#include "displayIcons.cpp"

// LCD
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void lcdAnimation(String status)
{
    if (status == "startup")
    {
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("^._.^");
        delay(300);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("^._.^");
        lcd.setCursor(0,1);
        lcd.print("|   |");
        delay(300);
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("^._.^ K");
        delay(50);
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("^._.^ KA");
        delay(50);
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("^._.^ KAT");
        delay(50);
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("^._.^ KAT G");
        delay(50);
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("|   | KAT GU");
        lcd.setCursor(0,0);
        lcd.print("^._.^");
        delay(50);
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("|   | KAT GUA");
        lcd.setCursor(0,0);
        lcd.print("^._.^");
        delay(50);
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("|   | KAT GUAR");
        lcd.setCursor(0,0);
        lcd.print("^._.^");
        delay(50);
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("^._.^ KAT GUARD");
    }
}

void printState(char armTittle[], int iconState)
{
  // int = i;
  lcd.setCursor(0,0);
  lcd.write(iconState);
  lcd.print(" ");
  lcd.print(armTittle);
  int blankSpace = 13 - strlen(armTittle);
  for(int i = 0; i < blankSpace; i++)
  {
    lcd.print(" ");
  }
} 

void lcdUpdate(String status)
{
    if (status == "startup")
    {
        lcd.begin();
        lcd.backlight();
        lcd.createChar(1, lockedIcon);
        lcd.createChar(0, unlockedIcon);
        lcdAnimation(status);
    }
    else if (status == "locked")
    {
        printState("Locked", lockedIcon);
    }
    else if (status == "unlocked")
    {
        printState("Unlocked", unlockedIcon);
    }
}

