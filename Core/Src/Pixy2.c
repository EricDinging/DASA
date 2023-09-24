#include "Pixy2.h"
#include "stdio.h"

#define PIXY_DEBUG

Pixy2 pixy;

void printVersion() {
	char buf[64];
	sprintf(buf, "hardware ver: 0x%x firmware ver: %d.%d.%d %s\r\n", pixy.version->hardware, pixy.version->firmwareMajor, pixy.version->firmwareMinor, pixy.version->firmwareBuild, pixy.version->firmwareType);
	printf(buf);
	printf("%s\n", pixy.version->firmwareType);
}

void printBlock(int n) {

	int i, j;
	char buf[128], sig[6], d;
	bool flag;

	if ((pixy.curr_blocks + n)->m_signature > CCC_MAX_SIGNATURE) // color code! (CC)
	{
		// convert signature number to an octal string
		for (i = 12, j = 0, flag = false; i >= 0; i -= 3) {
			d = (((pixy.curr_blocks + n)->m_signature) >> i) & 0x07;
			if (d > 0 && !flag)
				flag = true;
			if (flag)
				sig[j++] = d + '0';
		}
		sig[j] = '\0';
		sprintf(buf, "CC block sig: %s (%d decimal) x: %d y: %d width: %d height: %d angle: %d index: %d age: %d\r\n", sig, (pixy.curr_blocks + n)->m_signature, (pixy.curr_blocks + n)->m_x, (pixy.curr_blocks + n)->m_y, (pixy.curr_blocks + n)->m_width, (pixy.curr_blocks + n)->m_height, (pixy.curr_blocks + n)->m_angle, (pixy.curr_blocks + n)->m_index, (pixy.curr_blocks + n)->m_age);
	} else
		// regular block.  Note, angle is always zero, so no need to print
		sprintf(buf, "sig: %d x: %d y: %d width: %d height: %d index: %d age: %d\r\n", (pixy.curr_blocks + n)->m_signature, (pixy.curr_blocks + n)->m_x, (pixy.curr_blocks + n)->m_y, (pixy.curr_blocks + n)->m_width, (pixy.curr_blocks + n)->m_height, (pixy.curr_blocks + n)->m_index, (pixy.curr_blocks + n)->m_age);
	printf(buf);
}


int16_t getSync() {
	uint8_t i, j, c, cprev;
	int16_t res;
	uint16_t start;

	// parse bytes until we find sync
	for (i = j = 0, cprev = 0; true; i++) {
		res = recv(&c, 1, NULL);
		if (res >= PIXY_RESULT_OK) {
			// since we're using little endian, previous byte is least significant byte
			start = cprev;
			// current byte is most significant byte
			start |= c << 8;
			cprev = c;
			if (start == PIXY_CHECKSUM_SYNC) {
				pixy.m_cs = true;
				return PIXY_RESULT_OK;
			}
			if (start == PIXY_NO_CHECKSUM_SYNC) {
				pixy.m_cs = false;
				return PIXY_RESULT_OK;
			}
		}
		// If we've read some bytes and no sync, then wait and try again.
		// And do that several more times before we give up.
		// Pixy guarantees to respond within 100us.
		if (i >= 4) {
			if (j >= 4) {
				return PIXY_RESULT_ERROR;
			}

			for (volatile int i = 0; i < 10; i++) asm(""); // 25 microsecond delay (9.5 => 10)

			j++;
			i = 0;
		}
	}
}

int16_t recvPacket() {
	uint16_t csCalc, csSerial;
	int16_t res;

	res = getSync();
	if (res < 0)
		return res;

	if (pixy.m_cs) {
		res = recv(pixy.m_buf, 4, NULL);
		if (res < 0)
			return res;

		pixy.m_type = pixy.m_buf[0];
		pixy.m_length = pixy.m_buf[1];

		csSerial = *(uint16_t*) &(pixy.m_buf[2]);

		res = recv(pixy.m_buf, pixy.m_length, &csCalc);
		if (res < 0)
			return res;

		if (csSerial != csCalc) {
			return PIXY_RESULT_CHECKSUM_ERROR;
		}
	} else {
		res = recv(pixy.m_buf, 2, NULL);
		if (res < 0)
			return res;

		pixy.m_type = pixy.m_buf[0];
		pixy.m_length = pixy.m_buf[1];

		res = recv(pixy.m_buf, pixy.m_length, NULL);
		if (res < 0)
			return res;
	}
	return PIXY_RESULT_OK;
}

int16_t sendPacket() {
	// write header info at beginnig of buffer
	pixy.m_buf[0] = PIXY_NO_CHECKSUM_SYNC & 0xff;
	pixy.m_buf[1] = PIXY_NO_CHECKSUM_SYNC >> 8;
	pixy.m_buf[2] = pixy.m_type;
	pixy.m_buf[3] = pixy.m_length;
	// send whole thing -- header and data in one call
	return send(pixy.m_buf, pixy.m_length + PIXY_SEND_HEADER_SIZE);
}

int8_t getVersion() {
	pixy.m_length = 0;
	pixy.m_type = PIXY_TYPE_REQUEST_VERSION;
	sendPacket();
	if (recvPacket() == 0) {
		if (pixy.m_type == PIXY_TYPE_RESPONSE_VERSION) {
			pixy.version = (Version*) pixy.m_buf;
			return pixy.m_length;
		} else if (pixy.m_type == PIXY_TYPE_RESPONSE_ERROR)
			return PIXY_RESULT_BUSY;
	}
	return PIXY_RESULT_ERROR;  // some kind of bitstream error
}

int8_t getResolution() {
	pixy.m_length = 1;
	pixy.m_bufPayload[0] = 0; // for future types of queries
	pixy.m_type = PIXY_TYPE_REQUEST_RESOLUTION;
	sendPacket();
	if (recvPacket() == 0) {
		if (pixy.m_type == PIXY_TYPE_RESPONSE_RESOLUTION) {
			pixy.frameWidth = *(uint16_t*)pixy.m_buf;
			pixy.frameHeight = *(uint16_t*)(pixy.m_buf + sizeof(uint16_t));
			return PIXY_RESULT_OK; // success
		} else
			return PIXY_RESULT_ERROR;
	} else
		return PIXY_RESULT_ERROR;  // some kind of bitstream error
}

int8_t getBlocks(bool wait, uint8_t sigmap, uint8_t maxBlocks) {
	pixy.curr_blocks = NULL;
	pixy.numBlocks = 0;

	while (1) {
		// fill in request data
		pixy.m_bufPayload[0] = sigmap;
		pixy.m_bufPayload[1] = maxBlocks;
		pixy.m_length = 2;
		pixy.m_type = CCC_REQUEST_BLOCKS;

		// send request
		sendPacket();
		if (recvPacket() == 0) {
			if (pixy.m_type == CCC_RESPONSE_BLOCKS) {
				pixy.curr_blocks = (Block*) pixy.m_buf;
				pixy.numBlocks = pixy.m_length / sizeof(Block);

				return pixy.numBlocks;
			}
			// deal with busy and program changing states from Pixy (we'll wait)
			else if (pixy.m_type == PIXY_TYPE_RESPONSE_ERROR) {
				if ((int8_t) pixy.m_buf[0] == PIXY_RESULT_BUSY) {
					if (!wait)
						return PIXY_RESULT_BUSY; // new data not available yet
				} else if ((int8_t) pixy.m_buf[0] != PIXY_RESULT_PROG_CHANGING)
					return pixy.m_buf[0];
			}
		} else
			return PIXY_RESULT_ERROR;  // some kind of bitstream error

		// If we're waiting for frame data, don't thrash Pixy with requests.
		// We can give up half a millisecond of latency (worst case)
		for (volatile int i = 0; i < 190; i++) asm("");
	}
}

int8_t pixy2_init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *port, uint16_t pin) {
	// allocate buffer space for send/receive
	pixy.m_buf = (uint8_t*) malloc(PIXY_BUFFERSIZE);
	// shifted buffer is used for sending, so we have space to write header information
	pixy.m_bufPayload = pixy.m_buf + PIXY_SEND_HEADER_SIZE;
	pixy.frameWidth = pixy.frameHeight = 0;
	pixy.version=NULL;
	pixy.m_cs = false;
	pixy.curr_blocks = NULL;
	pixy.numBlocks = 0;

	int8_t res;

	res = open(hspi, port, pin);
	if (res < 0)
		return res;

	// wait for pixy to be ready -- that is, Pixy takes a second or 2 boot up
	// getVersion is an effective "ping".  We timeout after 5s.

	for (int millis = HAL_GetTick(); HAL_GetTick() - millis < 5000;) {
		if (getVersion() >= 0) { // successful version get -> pixy is ready
			getResolution(); // get resolution so we have it
			return PIXY_RESULT_OK;
		}
		HAL_Delay(5); // might have to change
	}
	// timeout
	return PIXY_RESULT_TIMEOUT;
}
