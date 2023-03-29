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

enum State {
	  INIT = 0,
	  SEARCH = 1,
	  COLLECT = 2,
	  RETURN = 3,
	  AVOID_COLLISION = 4
  };

extern enum State state;
extern enum State prev_state;
#define MAXLOAD 6

void state_update();

void execute();

#endif /* INC_CENTRAL_CONTROL_H_ */
