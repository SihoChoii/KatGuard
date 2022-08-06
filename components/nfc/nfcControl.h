#ifndef NFCCONTROLH
#define NFCCONTROLH

#include <Arduino.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

extern bool lockState;

class nfcData
{
    public:
    String payload, uid;
};

struct nfcKey
{
    String uidData, payloadData;
};

extern nfcData master;

void nfcStartup();
bool nfcTagPresent();

bool authenticate(String tagId, String payload);
struct nfcKey readNFC();

#endif