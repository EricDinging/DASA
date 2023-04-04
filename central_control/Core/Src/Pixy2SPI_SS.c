#include "Pixy2SPI_SS.h"

SPI_HandleTypeDef* SPI_obj;
GPIO_TypeDef* GPIO_ss;
uint16_t GPIO_ss_pin;

int8_t open(SPI_HandleTypeDef* hspi,GPIO_TypeDef* port,uint16_t pin) {
	GPIO_ss_pin = pin;
	GPIO_ss = port;
	SPI_obj = hspi;
	return 0;
}

int16_t recv(uint8_t *buf, uint8_t len, uint16_t *checkSumCalculation) { // based on arduino implementation
	uint8_t i = 0;
	if (checkSumCalculation)
		*checkSumCalculation = 0;
	
	HAL_GPIO_WritePin(GPIO_ss, GPIO_ss_pin, GPIO_PIN_RESET);

	for (i = 0; i < len; ++i) {
		uint8_t data = 0;
		HAL_SPI_Receive(SPI_obj, &data, 1, 5000);
		buf[i] = data;
		if (checkSumCalculation)
			*checkSumCalculation += buf[i];
	}

	HAL_GPIO_WritePin(GPIO_ss, GPIO_ss_pin, GPIO_PIN_SET);
	return len;
}

int16_t send(uint8_t *buf, uint8_t len) { // based on arduino implementation
	uint8_t i = 0;
	HAL_GPIO_WritePin(GPIO_ss, GPIO_ss_pin, GPIO_PIN_RESET);


	for (i = 0; i < len; ++i) {
		HAL_SPI_Transmit(SPI_obj, (uint8_t *) (buf + i), 1, 5000);
	}


	HAL_GPIO_WritePin(GPIO_ss, GPIO_ss_pin, GPIO_PIN_SET);

	return len;
}
