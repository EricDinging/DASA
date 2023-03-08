#include "motor_control.h"

void motor_control (uint16_t mode) {
	// mode 0 straight, 1 left turn, 2 right turn, 3 still-rotation, 4 reverse
	static uint16_t lmotor_pwm_val = 20; // 0 - 99 throttle control
	static uint16_t rmotor_pwm_val = 20;
	static uint16_t lmotor_h_bridge_in1 = 1;
	static uint16_t lmotor_h_bridge_in2 = 0;
	static uint16_t rmotor_h_bridge_in3 = 1;
	static uint16_t rmotor_h_bridge_in4 = 0;
	switch(mode) {
		case 0 :
			lmotor_pwm_val = 50;
			rmotor_pwm_val = 50;
			lmotor_h_bridge_in1 = 1;
			lmotor_h_bridge_in2 = 0;
			rmotor_h_bridge_in3 = 1;
			rmotor_h_bridge_in4 = 0;
			break;
		case 1:
			lmotor_pwm_val = 0;
			rmotor_pwm_val = 30;
			lmotor_h_bridge_in1 = 1;
			lmotor_h_bridge_in2 = 0;
			rmotor_h_bridge_in3 = 1;
			rmotor_h_bridge_in4 = 0;
		case 2:
			lmotor_pwm_val = 30;
			rmotor_pwm_val = 0;
			lmotor_h_bridge_in1 = 1;
			lmotor_h_bridge_in2 = 0;
			rmotor_h_bridge_in3 = 1;
			rmotor_h_bridge_in4 = 0;
		case 3:
			lmotor_pwm_val = 15;
			rmotor_pwm_val = 15;
			lmotor_h_bridge_in1 = 1;
			lmotor_h_bridge_in2 = 0;
			rmotor_h_bridge_in3 = 0;
			rmotor_h_bridge_in4 = 1;
		case 4:
			lmotor_pwm_val = 30;
			rmotor_pwm_val = 30;
			lmotor_h_bridge_in1 = 0;
			lmotor_h_bridge_in2 = 1;
			rmotor_h_bridge_in3 = 0;
			rmotor_h_bridge_in4 = 1;
		default:
			lmotor_pwm_val = 0;
			rmotor_pwm_val = 0;
			lmotor_h_bridge_in1 = 1;
			lmotor_h_bridge_in2 = 0;
			rmotor_h_bridge_in3 = 1;
			rmotor_h_bridge_in4 = 0;
	}

	htim1.Instance->CCR1 = lmotor_pwm_val; // PE9 to EnA
	htim1.Instance->CCR2 = rmotor_pwm_val; // PE11 to EnB
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6,  lmotor_h_bridge_in1);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5,  lmotor_h_bridge_in2);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4,  rmotor_h_bridge_in3);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3,  rmotor_h_bridge_in4);
}
