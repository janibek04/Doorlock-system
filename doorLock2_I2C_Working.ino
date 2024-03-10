#include <Wire.h>
#include <Adafruit_PN532.h>
#include <Servo.h>

// Declare servo
Servo lockServo;

// Declare shield IO hardware
#define blueLed 8
#define redLed 7
// Declare buzzer
int buzzer = 10;

// Declaring frequency of each tone
#define note_DD5 622
#define note_AA5 932

#define note_Bflat 466
#define note_A 440
#define note_Aflat 415
#define note_G 392

// Using I2C for PN532
#define PN532_IRQ   2
#define PN532_RESET 3

// Configure NFC shield device by creating a new instance of Adafruit_PN532 called nfc
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);



/// Replace these lines
// uint8_t card1[] = { 0x04, 0xDD, 0xD4, 0x12, 0xFF, 0x38, 0x80 };
// uint8_t card2[] = { 0x93, 0xD5, 0x15, 0xC1, 0, 0, 0 };
// uint8_t card3[] = { 0x94, 0xBB, 0x17, 0xC5, 0, 0, 0 };

// With your card definitions
uint8_t card1[] = { 0xF3, 0xEA, 0x06, 0xF5, 0, 0, 0 };
uint8_t card2[] = { 0xDC, 0xC6, 0x97, 0x63, 0, 0, 0 };


void setup() {
  pinMode(blueLed, OUTPUT);
  digitalWrite(blueLed, LOW);
  pinMode(redLed, OUTPUT);
  digitalWrite(redLed, LOW);

  // Open serial port for debugging
  Serial.begin(115200);
  Serial.print("Start");

  nfc.begin();

  // Shamelessly borrowed from Adafruit lib ***********************
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1);  // halt
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behavior of the PN532.
  //nfc.setPassiveActivationRetries(0xFF); 
  // Above, it is from original code, but it made the code stop, 
  // so I just commented it

  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for an ISO14443A card");

  // Attach the servo to pin 9
  lockServo.attach(9);
  lockServo.write(0);  // Initialize the servo to the locked position
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID

  uint8_t uidLength;  // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  unsigned long readID = 0;

  // Wait for an ISO14443A type card (Mifare, etc.). When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  Serial.println("Before reading card");
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  Serial.println("After reading card");

  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Value: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x");
      Serial.print(uid[i], HEX);
      readID += uid[i];
      if (i < (uidLength - 1)) {
        readID = readID << 8;
      }
    }

    int i = 0;
    int cardmatches = 0;

    for (i = 0; i < 7; i++) {
      if (uid[i] == card1[i] || uid[i] == card2[i]) {
        cardmatches++;
      }
    }

    if (cardmatches == 7) {
      Unlock();
    } else {
      keepLock();
    }

    delay(1000);
  } else {
    // PN532 probably timed out waiting for a card
    Serial.println("Timed out waiting for a card");
  }
}

void keepLock() {
  Serial.println("");
  Serial.println("Unsuccessful! Wrong tag!");
  digitalWrite(redLed, HIGH);
  // "failure" buzzer
  tone(buzzer, note_Bflat);
  delay(200);
  noTone(buzzer);
  delay(100);
  tone(buzzer, note_A);
  delay(200);
  noTone(buzzer);
  delay(100);
  tone(buzzer, note_Aflat);
  delay(200);
  noTone(buzzer);
  delay(100);
  tone(buzzer, note_G);
  delay(500);
  noTone(buzzer);

  digitalWrite(redLed, LOW);
}

void Unlock() {
  Serial.println("");
  Serial.println("Unlocking!!");
  // Move the servo to the unlocked position (adjust the angle as needed)
  lockServo.write(90);
  digitalWrite(blueLed, HIGH);
  // ringtone starts from the buzzer
  tone(buzzer, note_DD5);
  delay(200);
  noTone(buzzer);
  delay(200);
  tone(buzzer, note_DD5);
  delay(200);
  tone(buzzer, note_DD5);
  delay(190);
  noTone(buzzer);
  delay(20);
  tone(buzzer, note_AA5);
  delay(190);
  noTone(buzzer);
  delay(1000);
  // Move the servo back to the locked position
  lockServo.write(0);
  digitalWrite(blueLed, LOW);
}
