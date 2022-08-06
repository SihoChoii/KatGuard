// KatGuard v2.1.1-aplha.1
#include "KatGuard.h"

void setup(void)
{
    Serial.begin(9600);
    Serial.println("System initialized");
    nfcStartup();
    lcdUpdate("startup");
    tone(buz1, NOTE_CS6, 100);
    lcdUpdate("unlocked");
}

void loop(void)
{
    if (!lockState)
    {
        if (nfcTagPresent())
        {
            nfcKey nfcKeyData = readNFC();
            master.uid = nfcKeyData.uidData;
            master.payload = nfcKeyData.payloadData;
            Serial.print("Master UID");
            lockState = true;
            lcdUpdate("locked");
            tone(buz1, NOTE_CS5, 100);
            tone(buz1, NOTE_CS6, 100);
        }
    }
    else
    {
        if (nfcTagPresent())
        {
            if (authenticate(master.uid, master.payload))
            {
                lockState = false;
                lcdUpdate("unlocked");
                master.uid = "NEKONEKONEKO";
                master.payload = "NEKONEKONEKO";
                tone(buz1, NOTE_CS5, 100);
                tone(buz1, NOTE_CS6, 100);
            }
        }
    }
    delay(100);
}

// void alarmSystem(int status)
// {
//     // Alarm Sequence
// }

// int tarpStatus()
// {
//     // Tarp Detection System
//     return HIGH;
//     return LOW;
// }