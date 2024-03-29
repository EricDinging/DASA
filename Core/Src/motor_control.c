/*
 * motor_control.c
 *
 *  Created on: Mar 12, 2023
 *      Author: ericding
 */
#include "motor_control.h"

void motor_control (uint8_t mode) {
	// mode 0 straight, 1 left turn, 2 right turn, 3 still-rotation, 4 reverse, 5 nitroboost, 6 collect
	static uint8_t lmotor_pwm_val = 20; // 0 - 99 throttle control
	static uint8_t rmotor_pwm_val = 20;
	static uint8_t lmotor_h_bridge_in1 = 1;
	static uint8_t lmotor_h_bridge_in2 = 0;
	static uint8_t rmotor_h_bridge_in3 = 1;
	static uint8_t rmotor_h_bridge_in4 = 0;
	switch(mode) {
		case 0 :
			lmotor_pwm_val = 40;
			rmotor_pwm_val = 40;
			lmotor_h_bridge_in1 = 1;
			lmotor_h_bridge_in2 = 0;
			rmotor_h_bridge_in3 = 1;
			rmotor_h_bridge_in4 = 0;
			break;
		case 1:
			lmotor_pwm_val = 80;
			rmotor_pwm_val = 80;
			lmotor_h_bridge_in1 = 0;
			lmotor_h_bridge_in2 = 1;
			rmotor_h_bridge_in3 = 1;
			rmotor_h_bridge_in4 = 0;
			break;
		case 2:
			lmotor_pwm_val = 80;
			rmotor_pwm_val = 80;
			lmotor_h_bridge_in1 = 1;
			lmotor_h_bridge_in2 = 0;
			rmotor_h_bridge_in3 = 0;
			rmotor_h_bridge_in4 = 1;
			break;
		case 3:
			lmotor_pwm_val = 80;
			rmotor_pwm_val = 80;
			lmotor_h_bridge_in1 = 0;
			lmotor_h_bridge_in2 = 1;
			rmotor_h_bridge_in3 = 1;
			rmotor_h_bridge_in4 = 0;
			break;
		case 4:
			lmotor_pwm_val = 40;
			rmotor_pwm_val = 40;
			lmotor_h_bridge_in1 = 0;
			lmotor_h_bridge_in2 = 1;
			rmotor_h_bridge_in3 = 0;
			rmotor_h_bridge_in4 = 1;
			break;
		case 5:
			lmotor_pwm_val = 20;
			rmotor_pwm_val = 20;
			lmotor_h_bridge_in1 = 0;
			lmotor_h_bridge_in2 = 1;
			rmotor_h_bridge_in3 = 0;
			rmotor_h_bridge_in4 = 1;
			break;
		case 6:
			lmotor_pwm_val = 40;
			rmotor_pwm_val = 40;
			lmotor_h_bridge_in1 = 1;
			lmotor_h_bridge_in2 = 0;
			rmotor_h_bridge_in3 = 1;
			rmotor_h_bridge_in4 = 0;
			break;
		default:
			lmotor_pwm_val = 0;
			rmotor_pwm_val = 0;
			lmotor_h_bridge_in1 = 1;
			lmotor_h_bridge_in2 = 0;
			rmotor_h_bridge_in3 = 1;
			rmotor_h_bridge_in4 = 0;
			break;
	}

	htim4.Instance->CCR3 = lmotor_pwm_val; // PD14 to EnA
	htim4.Instance->CCR4 = rmotor_pwm_val; // PD15 to EnB
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7,  lmotor_h_bridge_in1);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8,  lmotor_h_bridge_in2);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12,  rmotor_h_bridge_in3);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13,  rmotor_h_bridge_in4);
}

