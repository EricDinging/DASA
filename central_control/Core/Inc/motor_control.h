/*
 * motor_control.h
 *
 *  Created on: Mar 12, 2023
 *      Author: ericding
 */

#ifndef INC_MOTOR_CONTROL_H_
#define INC_MOTOR_CONTROL_H_

#include "main.h"

extern TIM_HandleTypeDef htim4;

void motor_control (uint8_t mode);



#endif /* INC_MOTOR_CONTROL_H_ */
