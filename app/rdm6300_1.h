/*
 * A simple library to interface with rdm6300 rfid reader.
 *
 * Arad Eizen (https://github.com/arduino12).
 */

#ifndef _RDM6300_h_
#define _RDM6300_h_

#include <Arduino.h>

#define RDM6300_BAUDRATE		9600
#define RDM6300_PACKET_SIZE		14
#define RDM6300_PACKET_BEGIN	0x02
#define RDM6300_PACKET_END		0x03
#define RDM6300_NEXT_READ_MS	800 //220
#define RDM6300_READ_TIMEOUT	20

class Rdm6300
{
	public:
		void begin();
		void end(void);
		bool update(void);
		uint32_t get_tag_id(void);
		bool is_tag_near(void);
	private:
		uint32_t _tag_id = 0;
		uint32_t _last_tag_id = 0;
		uint32_t _last_read_ms = 0;
};

#endif
