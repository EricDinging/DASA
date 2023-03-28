/*
 * central_control.h
 *
 *  Created on: Mar 25, 2023
 *      Author: eric
 */

#ifndef INC_CENTRAL_CONTROL_H_
#define INC_CENTRAL_CONTROL_H_

#include "motor_control.h"
#include "main.h"

enum State {
	  INIT = 0,
	  SEARCH = 1,
	  COLLECT = 2,
	  RETURN = 3,
	  AVOID_COLLISION = 4
  };

enum State state;
enum State prev_state;

uint8_t on_off = 0;
uint8_t ball_locked = 0; // from AI
uint8_t ball_not_found = 0; // from AI
uint8_t ball_count = 0; // from IR
uint8_t ball_collected = 0; // from IR
uint8_t station_arrived = 0; // from AI
uint8_t avoid_end = 0; // from timer
uint8_t reset = 0; // reset button

#define MAXLOAD 6

void state_update();

void execute();

#endif /* INC_CENTRAL_CONTROL_H_ */
