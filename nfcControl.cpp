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