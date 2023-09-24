/*
 * central_control.h
 *
 *  Created on: Mar 25, 2023
 *      Author: eric
 */

#ifndef INC_CENTRAL_CONTROL_H_
#define INC_CENTRAL_CONTROL_H_


#include "main.h"
#include "motor_control.h"
#include "rotor_control.h"

extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;


enum State {
	  INIT = 0, // white
	  SEARCH = 1, // blue
	  COLLECT = 2, // green
	  RETURN = 3, // white
	  AVOID_COLLISION = 4, // red
	  SEARCH_TO_COLLECT = 5,
	  COLLECT_TO_SEARCH = 6,
	  COLLECT_TO_RETURN = 7
  };

#define MAXLOAD 4

extern uint8_t on_off; // on_off button
extern uint8_t reset; // reset button

extern uint8_t first_rotor; // ir interrupt will be first rotor


void state_update();

void execute();

#endif /* INC_CENTRAL_CONTROL_H_ */
