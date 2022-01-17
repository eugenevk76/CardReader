/*
 * A simple example to interface with rdm6300 rfid reader.
 *
 * Connect the rdm6300 to VCC=5V, GND=GND, TX=any GPIO (this case GPIO-04)
 * Note that the rdm6300's TX line is 3.3V level,
 * so it's save to use with both AVR* and ESP* microcontrollers.
 *
 * Arad Eizen (https://github.com/arduino12).
 */

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <string.h>
#include "rdm6300_1.h"
#include "RFIDData.h"

#define PIN_RED 4
#define PIN_GREEN 2
#define PIN_BLUE 3
#define PIN_BUZZER 5

#define PN532_SCK  (15)
#define PN532_MOSI (16)
#define PN532_SS   (10)
#define PN532_MISO (14)


// Init array that will store new NUID 
byte nuidPICC[4];

Rdm6300 rdm6300;
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

uint8_t prev_uid[] = { 0, 0, 0, 0, 0, 0, 0 };

char serial[8] = { "" };

RFIDData currentData;

void setup()
{
	
	Serial.begin(115200);
	//while (!Serial) delay(10);
	
	pinMode(PIN_RED, OUTPUT);
	pinMode(PIN_GREEN, OUTPUT);
	pinMode(PIN_BLUE, OUTPUT);
	pinMode(PIN_BUZZER, OUTPUT);
	
	

	nfc.begin();

	uint32_t versiondata = nfc.getFirmwareVersion();
	if (!versiondata) {
		Serial.print("Didn't find PN53x board");
		for (int i = 0; i < 3; i++)
		{
			digitalWrite(PIN_RED, HIGH);
			digitalWrite(PIN_BUZZER, HIGH);
			delay(100);
			digitalWrite(PIN_RED, LOW);
			digitalWrite(PIN_BUZZER, LOW);
			delay(100);
		}
		while (1); // halt
	}

	// configure board to read RFID tags
	nfc.SAMConfig();

	//SPI.begin(); // Init SPI bus
	delay(100);
	rdm6300.begin();
	
	//Serial.println("\nPlace RFID tag near the rdm6300...");
	//Serial.println(F("This code scan the MIFARE Classsic NUID."));
	//Serial.print(F("Using the following key:"));
	//printHex(key.keyByte, MFRC522::MF_KEY_SIZE);


	digitalWrite(PIN_RED, HIGH);
	digitalWrite(PIN_BUZZER, HIGH);
	delay(100);
	digitalWrite(PIN_RED, LOW);
	digitalWrite(PIN_BUZZER, LOW);
	digitalWrite(PIN_GREEN, HIGH);
	delay(100);
	digitalWrite(PIN_GREEN, LOW);
	digitalWrite(PIN_BUZZER, HIGH);
	digitalWrite(PIN_BLUE, HIGH);
	delay(100);
	digitalWrite(PIN_BLUE, LOW);
	digitalWrite(PIN_BUZZER, LOW);
	digitalWrite(PIN_RED, HIGH);
}

void loop()
{
	//mifare
	readMF();

	//EM Marin
	readEM();

	if (currentData.hasNewSerial()) {

		//Serial.print("Type: ");
		//Serial.println(currentData.getType());
		//Serial.print("Serial: ");
		//Serial.println(currentData.getSerial());

		Serial.print(currentData.getType());
		Serial.print("#");
		Serial.println(currentData.getSerial());

		//delay(1000);

	}
	
	delay(100);
}

void readEM() {

	/* if non-zero tag_id, update() returns true- a new tag is near! */
	if (rdm6300.update()) {

		digitalWrite(PIN_RED, LOW);
		digitalWrite(PIN_BLUE, HIGH);
		digitalWrite(PIN_BUZZER, HIGH);
		delay(200);
		digitalWrite(PIN_BLUE, LOW);
		digitalWrite(PIN_BUZZER, LOW);
		digitalWrite(PIN_RED, HIGH);

		currentData.setCurrentSerial(rdm6300.get_tag_id());

		//Serial.println(rdm6300.get_tag_id(), HEX);
	}

}

void readMF() {


	uint8_t success;
	uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
	uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

	// Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
	// 'uid' will be populated with the UID, and uidLength will indicate
	// if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
	success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);



	if (success) {

		int i;

		if (memcmp(prev_uid, uid, 7) == 0) return;

		memcpy(prev_uid, uid, 7);

		//Info
		digitalWrite(PIN_RED, LOW);
		digitalWrite(PIN_GREEN, HIGH);
		digitalWrite(PIN_BUZZER, HIGH);
		delay(200);
		digitalWrite(PIN_GREEN, LOW);
		digitalWrite(PIN_BUZZER, LOW);
		digitalWrite(PIN_RED, HIGH);


		// Display some basic information about the card
		//Serial.println("Found an ISO14443A card");
		//Serial.print("  UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
		//Serial.print("  UID Value: ");
		//nfc.PrintHex(uid, uidLength);
		//Serial.println("");

		currentData.setCurrentSerial(uid, uidLength);

	}
	else {
		
		memset(prev_uid, 0, 7);
	
	}





	//// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	//if (!rfid.PICC_IsNewCardPresent())
	//	return;

	//// Verify if the NUID has been readed
	//if (!rfid.PICC_ReadCardSerial())
	//	return;

	//digitalWrite(PIN_RED, LOW);
	//digitalWrite(PIN_GREEN, HIGH);
	//digitalWrite(PIN_BUZZER, HIGH);
	//delay(200);
	//digitalWrite(PIN_GREEN, LOW);
	//digitalWrite(PIN_BUZZER, LOW);
	//digitalWrite(PIN_RED, HIGH);

	//currentData.setCurrentSerial(rfid.uid.uidByte, rfid.uid.size);

	////char uid[40];
	////memset(uid, 0, 39);

	////if (rfid.uid.size >= 4) {
	////	for (int idx = 3; idx >= 0; idx--) {
	////		sprintf(uid, "%s%02X", uid, rfid.uid.uidByte[idx]);
	////	}
	////	Serial.print("Algo SN: ");
	////	Serial.println(uid);
	////}

	////Serial.print(F("PICC type: "));
	////MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
	////Serial.println(rfid.PICC_GetTypeName(piccType));

	////Serial.print(F("Card UID:"));
	////dump_byte_array(rfid.uid.uidByte, rfid.uid.size);
	////Serial.println();

	//// Halt PICC
	//rfid.PICC_HaltA();

	//// Stop encryption on PCD
	//rfid.PCD_StopCrypto1();

	////delay(1000);

}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void printHex(byte* buffer, byte bufferSize) {
	for (byte i = 0; i < bufferSize; i++) {
		Serial.print(buffer[i] < 0x10 ? " 0" : " ");
		Serial.print(buffer[i], HEX);
		if (i < 4) {

			//serial[]

		}
	}
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte* buffer, byte bufferSize) {
	for (byte i = 0; i < bufferSize; i++) {
		Serial.print(buffer[i] < 0x10 ? " 0" : " ");
		Serial.print(buffer[i], DEC);
	}
}

void dump_byte_array(byte* buffer, byte bufferSize) {
	for (byte i = 0; i < bufferSize; i++) {
		Serial.print(buffer[i] < 0x10 ? " 0" : " ");
		Serial.print(buffer[i], HEX);
	}
}
