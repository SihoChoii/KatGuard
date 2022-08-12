#ifndef NFCCONTROLH
#define NFCCONTROLH

#include <Arduino.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include </Systems/Crypto/encrypt.h>

extern bool lockState;

struct nfcData
{
    String payload, uid;
};

extern nfcData master;

void nfcStartup();
bool nfcTagPresent();

bool authenticate(String tagId, String payload);
struct nfcData readNFC();

#endif