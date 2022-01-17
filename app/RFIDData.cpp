// 
// 
// 

#include "RFIDData.h"

void RFIDData::setCurrentSerial(uint32_t serial_num) {

	char tmp_buf[16] = { 0, };
	type = 1;

	clearSerial();
	int cnt = 0;
	ultoa(serial_num, tmp_buf, 16);

#ifdef DBG

	Serial.println("TMP_Buffer:");
	for (int i = 0; i < 16; i++)
	{

		if (tmp_buf[i] < 20) {
			Serial.print("_");
			continue;
		}
		Serial.print(tmp_buf[i]);


	}
	Serial.println();

	Serial.print("len: ");
	Serial.println(strlen(tmp_buf));

#endif // DBG


	int startpos = 8 - strlen(tmp_buf);
	while (tmp_buf[cnt]) {
		serial[cnt + startpos] = toUpperCase(tmp_buf[cnt]);
		cnt++;
		if (cnt > 6) break;
	}

	_hasNewSerial = true;

}

void RFIDData::setCurrentSerial(byte* buffer, byte bufferSize) {

	//char tmp_buf[16];
	//memset(tmp_buf, 0, 0);
	type = 0;
	//clearSerial();

	//Serial.print("Buffer size: ");
	//Serial.println(bufferSize);


	//char test[30];
	//uint8_t code[4];
	//memset(test, 0, 30);
	//int cnt = bufferSize > 29 ? 29 : bufferSize;
	//for (int i = 0; i < cnt; i++) {
	//	sprintf(test, "%s%02X", test, reverseBuffer[i]);
	//}
	//Serial.println(test);

#ifdef USE_NEW_ALG

	//Translate by alg
	uint8_t code[4];
	uint8_t crc = 0x5A;
	uint8_t reverseBuffer[7];

	memset(reverseBuffer, 0, sizeof(uint8_t) * 7);
	for (int i = 0; i < bufferSize; i++)
	{
		if (i >= 7) break;
		//reverseBuffer[i] = buffer[bufferSize - 1 - i];
		reverseBuffer[i] = buffer[i];

	}

#ifdef ALG_TEST
	////IN: 00901B74028905
	////OUT: 6F92896A

	//reverseBuffer[6] = 0x00;
	//reverseBuffer[5] = 0x90;
	//reverseBuffer[4] = 0x1B;
	//reverseBuffer[3] = 0x74;
	//reverseBuffer[2] = 0x02;
	//reverseBuffer[1] = 0x89;
	//reverseBuffer[0] = 0x05;

	
	//IN: 00013EB95A8D05
	//OUT: 875B8D1F

	reverseBuffer[6] = 0x00;
	reverseBuffer[5] = 0x01;
	reverseBuffer[4] = 0x3E;
	reverseBuffer[3] = 0xB9;
	reverseBuffer[2] = 0x5A;
	reverseBuffer[1] = 0x8D;
	reverseBuffer[0] = 0x05;

#endif // ALG_TEST

	if (bufferSize == 7) {

		memset(serial, 0, 8);

		for (int i = 0; i < 7; i++) {
			crc = CRCTBL[crc ^ reverseBuffer[i]];
#ifdef ALG_TEST
			Serial.print("crc: ");
			Serial.print(crc);
			Serial.print("; index: ");
			Serial.print(crc ^ buffer[i]);
			Serial.print("; table val: ");
			Serial.print(CRCTBL[crc ^ buffer[i]]);
			Serial.print("; i: ");
			Serial.print(i);
			Serial.print("; buf value: ");
			Serial.println(buffer[i], HEX);
#endif // ALG_TEST

		}
		code[0] = reverseBuffer[0] ^ crc;
		code[1] = reverseBuffer[1] ^ reverseBuffer[6];
		code[2] = reverseBuffer[2] ^ reverseBuffer[5];
		code[3] = reverseBuffer[3] ^ reverseBuffer[4];
		for (int i = 4; i > 0; i--) {
			sprintf(serial, "%s%02X", serial, code[i-1]);
		}
		//Serial.println(serial);

		type = 2;
		_hasNewSerial = true;
		return;
	}

#endif // USE_NEW_ALG



	if (bufferSize > 3) {

		//for (int i = 0; i < 4; i++) {
		//	
		//	itoa(buffer[i], tmp_buf, 16);
		//	if (buffer[i] < 0x10) {
		//		
		//		serial[7 - (i * 2)] = toUpperCase(tmp_buf[0]);

		//	} else {
		//		
		//		serial[7 - (i * 2)] = toUpperCase(tmp_buf[1]);
		//		serial[7 - (i * 2) - 1] = toUpperCase(tmp_buf[0]);
		//	}

		//}

		type = 2;
		memset(serial, 0, 8);
		for (int idx = 3; idx >= 0; idx--) {
			sprintf(serial, "%s%02X", serial, buffer[idx]);
		}

		_hasNewSerial = true;
	}

}

