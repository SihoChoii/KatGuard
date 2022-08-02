#include <Wire.h> 

// Display 1
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

/*
char[16] lcdBufferOne = " ";
sprintf(lcdBufferOne, "%c%c%c %s", '[', stateChar, ']', armState);
*/

// Custom Characters
byte locked[] = {
  B01110,
  B10001,
  B10001,
  B11111,
  B11011,
  B11011,
  B11111,
  B00000
};

byte unlocked[] = {
  B01110,
  B10010,
  B10000,
  B11111,
  B11011,
  B11011,
  B11111,
  B00000
};

// NFC
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c); 
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID

// Buzzer
#include "pitches.h"
const int buz1 = 3;

// Light Sensor
const int lightSensor = A0;

// Global Vars
int lockState = LOW, pairState = LOW, writeState = LOW;
String keyData;

void setup()
{
  Serial.begin(115200);
  
  // NFC Setup
  nfc.begin();
  nfcStartup();

  //LCD Startup
  lcd.begin();
  lcd.backlight();
  lcd.createChar(1, locked);
  lcd.createChar(0, unlocked);
  katStartup();
  
  tone(buz1, NOTE_CS6, 100);
}

void loop()
{
  //cleanNFC();
  //writeNFC("MEOW");
  //readNFC();

  if(pairState == LOW)
  {
    pairKey();
  }
  else if(pairState == HIGH)
  {
    int lightSensor = analogRead(A0);
    
    
    // Light Sensor Readings
    Serial.print("Light Sensor: ");
    Serial.println(lightSensor);   // the raw analog reading

    if(lightSensor<=40)
    {
      printState("ACTIVATED", 1);
      while(findKey()==false)
      {
        tone(buz1, NOTE_C5);
        delay(100);
        noTone(buz1);
        tone(buz1, NOTE_CS6);
        delay(100);
        noTone(buz1);
        tone(buz1, NOTE_C5);
        delay(100);
        noTone(buz1);
        tone(buz1, NOTE_CS6);
        delay(100);
        noTone(buz1);
        tone(buz1, NOTE_C5);
        delay(100);
        noTone(buz1);
        tone(buz1, NOTE_CS6);
        delay(100);
        noTone(buz1);
      }
    }
    findKey();
  }
  delay(100);
}

String converter(uint8_t *str){
    return String((char *)str);
}

void nfcStartup()
{
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) 
  {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A Card ...");
}

void pairKey()
{
  uint8_t success;
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) 
  {
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    tone(buz1, NOTE_CS6, 100);
    
    if (uidLength == 4)
    { 
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);  
      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];
        
        success = nfc.mifareclassic_ReadDataBlock(4, data);
        if (success)
        {
          Serial.println("Reading Block 4:");
          nfc.PrintHexChar(data, 16);
          Serial.println("");
          keyData = data;
          pairState = HIGH;
          Serial.print("Pair State: ");
          Serial.println(pairState);
          Serial.print("Key Data: ");
          Serial.println(keyData);
          Serial.print("UID: ");
          Serial.println(converter(uid));
          printState("Locked", 1);
        }
      }
    }
  }
}

bool findKey()
{
  uint8_t success;
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t tempUid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  String tempKeyData;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, tempUid, &uidLength);
  if (success) 
  {
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(tempUid, uidLength);
    Serial.println("");
    tone(buz1, NOTE_CS6, 100);
    
    if (uidLength == 4)
    { 
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);  
      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];
        
        success = nfc.mifareclassic_ReadDataBlock(4, data);
        if (success)
        {
          Serial.println("Reading Block 4:");
          nfc.PrintHexChar(data, 16);
          Serial.println("");
          tempKeyData = data;
          Serial.print("Pair State: ");
          Serial.println(pairState);
          Serial.print("Key Data: ");
          Serial.println(tempKeyData);
          Serial.print("UID: ");
          Serial.println(converter(tempUid));
          if(tempKeyData == keyData)
          {
            Serial.println("UID & Key Data Match");
            Serial.print("Locker UID: ");
            Serial.println();
            tone(buz1, NOTE_E6, 100);
            printState("Unlocked", 0);
            pairState = LOW;
            return true;
          }
          else
          {
            tempKeyData = "a";
            tone(buz1, NOTE_B5, 100);
            Serial.println("UID or Key Data Mismatch");
            return false;
          }
        }
      }
    }
  }
}

void nfcWrite()
{
  uint8_t success;
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) 
  {
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    tone(buz1, NOTE_CS6, 100);
    
    if (uidLength == 4)
    { 
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);  
      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];

        char dataa[] = { 'K', 'A', 'T', 'G', 'U', 'A', 'R', 'D', '0', '0', '0', '1', 0, 0, 0, 0};
        success = nfc.mifareclassic_WriteDataBlock (4, dataa);

        if(success)
        {
          Serial.print("Wrote: ");
          nfc.PrintHexChar(data, 16);
          Serial.print(" to block 4 ");
          writeState = HIGH;
        }
        else
        {
          Serial.print("Failed to write to card");
        }
      }
    }
  }
}

void katStartup()
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
  printState("Unlocked", 0);
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
