#include "stm32l4xx_hal.h"

int8_t open(SPI_HandleTypeDef* hspi, GPIO_TypeDef* port, uint16_t pin); // hspi is spi object ptr, port is GPIO object ptr
int16_t recv(uint8_t *buf, uint8_t len, uint16_t *checkSumCalculation); // from arduino porting guide
int16_t send(uint8_t *buf, uint8_t len); // from arduino porting guide
