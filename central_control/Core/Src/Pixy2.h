#ifndef PIXY2_PIXY2_H_
#define PIXY2_PIXY2_H_

#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stm32l4xx_hal.h"
#include "Pixy2SPI_SS.h"

// Below defines are from TPixy2.h
#define PIXY_DEFAULT_ARGVAL					 0x80000000
#define PIXY_BUFFERSIZE                      0x104
#define PIXY_CHECKSUM_SYNC                   0xc1af
#define PIXY_NO_CHECKSUM_SYNC                0xc1ae
#define PIXY_SEND_HEADER_SIZE                4
#define PIXY_MAX_PROGNAME                    33

#define PIXY_TYPE_REQUEST_CHANGE_PROG        0x02
#define PIXY_TYPE_REQUEST_RESOLUTION         0x0c
#define PIXY_TYPE_RESPONSE_RESOLUTION        0x0d
#define PIXY_TYPE_REQUEST_VERSION            0x0e
#define PIXY_TYPE_RESPONSE_VERSION           0x0f
#define PIXY_TYPE_RESPONSE_RESULT            0x01
#define PIXY_TYPE_RESPONSE_ERROR             0x03
#define PIXY_TYPE_REQUEST_BRIGHTNESS         0x10
#define PIXY_TYPE_REQUEST_SERVO              0x12
#define PIXY_TYPE_REQUEST_LED                0x14
#define PIXY_TYPE_REQUEST_LAMP               0x16
#define PIXY_TYPE_REQUEST_FPS                0x18

#define PIXY_RESULT_OK                       0
#define PIXY_RESULT_ERROR                    -1
#define PIXY_RESULT_BUSY                     -2
#define PIXY_RESULT_CHECKSUM_ERROR           -3
#define PIXY_RESULT_TIMEOUT                  -4
#define PIXY_RESULT_BUTTON_OVERRIDE          -5
#define PIXY_RESULT_PROG_CHANGING            -6


// Below defines are from Pixy2CCC.h
#define CCC_MAX_SIGNATURE                   7

#define CCC_RESPONSE_BLOCKS                 0x21
#define CCC_REQUEST_BLOCKS                  0x20

#define CCC_SIG1                     1
#define CCC_SIG2                     2
#define CCC_SIG3                     4
#define CCC_SIG4                     8
#define CCC_SIG5                     16
#define CCC_SIG6                     32
#define CCC_SIG7                     64
#define CCC_COLOR_CODES              128

#define CCC_SIG_ALL                  0xff // all bits or'ed together

// cannot store functions inside of c structs ...
typedef struct Version {
	uint16_t hardware;
	uint8_t firmwareMajor;
	uint8_t firmwareMinor;
	uint16_t firmwareBuild;
	char firmwareType[10];
} Version;
void printVersion();

typedef struct Block {
	uint16_t m_signature;
	uint16_t m_x;
	uint16_t m_y;
	uint16_t m_width;
	uint16_t m_height;
	int16_t m_angle;
	uint8_t m_index;
	uint8_t m_age;
} Block;
void printBlock(int n);

typedef struct Pixy2 {
	Version *version;
	uint16_t frameWidth;
	uint16_t frameHeight;

	uint8_t *m_buf;
	uint8_t *m_bufPayload;
	uint8_t m_type;
	uint8_t m_length;
	bool m_cs;

	Block * curr_blocks;
	uint8_t numBlocks;

} Pixy2;

int16_t getSync();
int16_t recvPacket();
int16_t sendPacket();

int8_t getVersion();
int8_t getResolution();
int8_t getBlocks(bool wait, uint8_t sigmap, uint8_t maxBlocks);
int8_t pixy2_init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *port, uint16_t cs_pin);

#endif /* PIXY2_PIXY2_H_ */
