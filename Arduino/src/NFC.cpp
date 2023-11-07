#include <MFRC522.h>
#include "Events.h"

#define SS_PIN 5
#define RST_PIN 15

extern enum Event event;

class NFC
{
private:
  MFRC522 rfid;

  byte keychainID[4] = {26, 164, 251, 176};
  byte cardID[4] = {195, 226, 203, 169};

public:
  NFC()
  {
    this->rfid = MFRC522(SS_PIN, RST_PIN);
  }

  void Setup()
  {
    SPI.begin();
    this->rfid.PCD_Init();
  }

  bool CheckStatus()
  {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    {
      return false;
    }

    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
    {
      event = Event::NFCIncorrecto;
    }
    else if (memcmp(rfid.uid.uidByte, cardID, 4) == 0)
    {
      event = Event::DesbloqueoManual;
    }
    else
    {
      event = Event::NFCIncorrecto;
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

    return true;
  }
};