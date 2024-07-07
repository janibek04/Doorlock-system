#include <Wire.h>
#include <Adafruit_PN532.h>

// Assign the Arduino pin numbers:
#define SDA_PIN 20
#define SCL_PIN 21
#define IRQ_PIN 2  // Interrupt pin connected to the IRQ pin of PN532
#define RST_PIN 3  // Not used in this example

Adafruit_PN532 nfc(IRQ_PIN, RST_PIN);

volatile bool cardPresent = false;

void setup(void) {
  
  Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN532 board");
    while (1)
      ;  // halt
  }

  nfc.SAMConfig();
  nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);

  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), checkForNFC, FALLING);
  Serial.println("Waiting for an NFC Tag...");
}

void loop(void) {
  if (cardPresent) {
    cardPresent = false;
    readNfcTag();
  }
  //delay(100);
  // Your code to handle other tasks, e.g., polling a key matrix, can go here
}

void checkForNFC() {
  cardPresent = true;
}

void readNfcTag() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  success = nfc.readDetectedPassiveTargetID(uid, &uidLength);

  if (success) {
    Serial.println("Found an NFC tag!");
    Serial.print("UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x");
      Serial.print(uid[i], HEX);
    }
    Serial.println("");
    nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);  // Restart detection
  }
}
