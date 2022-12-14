#include "nfcControl.h"

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
byte nuidPICC[4];
bool lockState = false;

nfcData master;

void nfcStartup()
{
    nfc.begin();
}

bool nfcTagPresent()
{
    if (nfc.tagPresent())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool authenticate(String tagId, String payload)
{
    nfcData testKey = readNFC();
    if (decrypt(testKey.uid, "NEKO") == decrypt(tagId, "NEKO") && decrypt(decrypt(testKey.payload, testKey.uid), "NEK0") == decrypt(decrypt(payload, tagId), "NEK0"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

struct nfcData readNFC()
{
    struct nfcData keyData;

    NfcTag tag = nfc.read();
    Serial.println(tag.getTagType());
    Serial.print("UID: ");Serial.println(tag.getUidString());
    keyData.uid = encrypt(tag.getUidString(), "NEKO");

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
            keyData.payload = encrypt(payloadAsString, keyData.uid);

            String uid = record.getId();
            if (uid != "")
            {
                Serial.print("  ID: ");Serial.println(uid);
            }
            return keyData;
        }
    }
}

bool writeNFC(String tagId, String payload)
{
    NdefMessage message = NdefMessage();
    NdefRecord record = NdefRecord();
    record.setType("text/plain");
    record.setId("");
    record.setPayload(payload);
    message.addRecord(record);
    return nfc.write(tagId, message);
}