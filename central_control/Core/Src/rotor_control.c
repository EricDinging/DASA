/*
 * rotor_control.c
 *
 *  Created on: Apr 2, 2023
 *      Author: ericding
 */

#include "rotor_control.h"


void rotor_control (uint8_t mode) {
	// mode 0 turn, mode 1 stop
	static uint8_t motor_pwm_val = 0; // 0 - 99 throttle control
	static uint8_t motor_h_bridge_in1 = 1;
	static uint8_t motor_h_bridge_in2 = 0;

	switch(mode) {
		case 0 :
			motor_pwm_val = 80;
			motor_h_bridge_in1 = 1;
			motor_h_bridge_in2 = 0;
			break;
		default:
			motor_pwm_val = 0;
			motor_h_bridge_in1 = 1;
			motor_h_bridge_in2 = 0;
			break;
	}

	htim4.Instance->CCR1 = motor_pwm_val; // PD13
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0,  motor_h_bridge_in1);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1,  motor_h_bridge_in2);
}
