#include <MFRC522.h>
#include "Events.h"

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
        this->rfid = MFRC522(10, 9);
    }

    void setup()
    {
        SPI.begin();
        rfid.PCD_Init();
    }

    bool checkStatus()
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
            event = Event::AperturaNFC;
        }
        else
        {
            event = Event::NFCIncorrecto;
        }
        // Halt PICC

        rfid.PICC_HaltA();
        // Stop encryption on PCD
        rfid.PCD_StopCrypto1();

        return true;
    }
};