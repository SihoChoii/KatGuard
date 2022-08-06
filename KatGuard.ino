
// KatGuard v1.2
#include <Wire.h>

// NFC
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
byte nuidPICC[4];
String masterPayload, masterUid;
bool lockState = false;

struct nfcKey
{
    String uidData, payloadData;
};

// LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Custom Characters
#define lockIcon 1
#define unlockIcon 0
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

// Light Sensor
const int lightSensor = A0;

// Buzzer
#include "pitches.h"
const int buz1 = 3;

void setup(void)
{
    Serial.begin(9600);
    Serial.println("System initialized");
    nfc.begin();
    lcdUpdate("startup");
    tone(buz1, NOTE_CS6, 100);
    lcdUpdate("unlocked");
}

void loop(void)
{
    if (!lockState)
    {
        if (nfc.tagPresent())
        {
            nfcKey nfcKeyData = readNFC();
            masterUid = nfcKeyData.uidData;
            masterPayload = nfcKeyData.payloadData;
            Serial.print("Master UID")
            lockState = true;
            lcdUpdate("locked");
            tone(buz1, NOTE_CS5, 100);
            tone(buz1, NOTE_CS6, 100);
        }
    }
    else
    {
        if (nfc.tagPresent())
        {
            if (authenticate(masterUid, masterPayload))
            {
                lockState = false;
                lcdUpdate("unlocked");
                masterUid = "NEKONEKONEKO";
                masterPayload = "NEKONEKONEKO";
                tone(buz1, NOTE_CS5, 100);
                tone(buz1, NOTE_CS6, 100);
            }
        }
        // else (tarpStatus() == LOW)
        // {
        //     alarmSystem(HIGH);
        //     while (lockState == true)
        //     {
        //         // Maybe move to function?
        //         if (nfc.tagPresent())
        //         {
        //             if (authenticate(masterUid, masterPayload))
        //             {
        //                 lockState = false;
        //                 lcdUpdate("unlocked");
        //             }
        //         }
        //     }
        //     alarmSystem(LOW);
        // }
    }
    delay(100);
}

void lcdUpdate(String status)
{
    if (status == "startup")
    {
        lcd.begin();
        lcd.backlight();
        lcd.createChar(1, locked);
        lcd.createChar(0, unlocked);
        lcdAnimation(status);
    }
    else if (status == "locked")
    {
        printState("Locked", lockIcon);
    }
    else if (status == "unlocked")
    {
        printState("Unlocked", unlockIcon);
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

void alarmSystem(int status)
{
    // Alarm Sequence
}

int tarpStatus()
{
    // Tarp Detection System
    return HIGH;
    return LOW;
}

bool authenticate(String tagId, String payload)
{
    nfcKey nfcKeyData = readNFC();
    if (nfcKeyData.uidData == tagId && nfcKeyData.payloadData == payload)
    {
        return true;
    }
    else
    {
        return false;
    }
}

struct nfcKey readNFC()
{
    // String tagId = "None";
    struct nfcKey nfcKeyData;

    NfcTag tag = nfc.read();
    Serial.println(tag.getTagType());
    Serial.print("UID: ");Serial.println(tag.getUidString());
    String tagId = tag.getUidString();
    nfcKeyData.uidData = tagId;

    if (tag.hasNdefMessage())
    {
        Serial.println("nDef Message Found");
        NdefMessage message = tag.getNdefMessage();
        Serial.print("\nThis NFC Tag contains an NDEF Message with ");
        Serial.print(message.getRecordCount());
        Serial.print(" NDEF Record");
        Serial.print(message.getRecordCount() != 1);
        {
            Serial.print("s");
        }
        Serial.println(".");

        int recordCount = message.getRecordCount();
        for (int i = 0; i < recordCount; i++)
        {
            Serial.print("\n NDEF Record ");Serial.println(i+1);
            NdefRecord record = message.getRecord(i);

            Serial.print("  TNF: ");Serial.println(record.getTnf());
            Serial.print("  Type: ");Serial.println(record.getType());

            int payloadLength = record.getPayloadLength();
            byte payload[payloadLength];
            record.getPayload(payload);

            Serial.print("  Payload (HEX): ");
            PrintHexChar(payload, payloadLength);

            String payloadAsString = "";
            for (int c = 0; c < payloadLength; c++)
            {
                payloadAsString += (char)payload[c];
            }
            Serial.print(" Payload (as String): ");
            Serial.println(payloadAsString);
            nfcKeyData.payloadData = payloadAsString;

            String uid = record.getId();
            if (uid != "")
            {
                Serial.print("  ID: ");Serial.println(uid);
            }
            return nfcKeyData;
        }
    }
}