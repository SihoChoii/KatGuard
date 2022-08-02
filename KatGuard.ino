
// KatGuard v1.2
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

String tagId = "None";
byte nuidPICC[4];

void setup(void)
{
    Serial.begin(9600);
    Serial.println("System initialized");
    nfc.begin();
}

void loop(void)
{
    readNFC();
    delay(100);
}

String readNFC()
{
    if (nfc.tagPresent())
    {
        NfcTag tag = nfc.read();
        Serial.println(tag.getTagType());
        Serial.print("UID: ");Serial.println(tag.getUidString());
        String tagId = tag.getUidString();

        if (tag.hasNdefMessage())
        {
            Serial.println("nDef Message Found");
            NdefMessage message = tag.NdefMessage();
            Serial.print("\nThis NFC Tag contains an NDEF Message with ");
            Serial.print(message.getRecordCount());
            Serial.print(" NDEF Record");
            Serial.print(message.getRecordCount() != 1)
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

                int payloadLength = recor.getPayloadLength();
                byte payload[payloadLength];
                record.getPayload(payload);

                Serial.print("  Payload (HEX): ");
                PrintHexChar(payload, payLoadLength);

                String payloadAsString = "";
                for (int c = 0; c < payloadLength; c++)
                {
                    payloadAsString += (char)payload[c];
                }
                Serial.print(" Payload (as String): ");
                Serial.println(payloadAsString);

                String uid = record.getId();
                if (uid != "")
                {
                    Serial.print("  ID: ");Serial.println(uid);
                }
                return payloadAsString;
            }


        }
    }
}