/*
 * central_control.c
 *
 *  Created on: Mar 25, 2023
 *      Author: eric
 */
#include "main.h"
#include "central_control.h"

void state_update() {
	// If on_off button is pressed, the robot will exit INIT state and start execution.
	// If the user press the button during robot's execution, the robot will enter RETURN
	// state immediately.

	// All the buttons should be rising-edge triggered
	

	// read reset button, and set reset bit to 1 only if reset button is pressed
	if (reset) {
		on_off = 0;
		ball_locked = 0;
		ball_not_found = 0;
		ball_count = 0;
		ball_collected = 0;
		station_arrived = 0;
		avoid_end = 0;
		reset = 0;
		state = INIT;
		prev_state = state;
		// clear peripheral register
	} else {
		// reading peripheral input, and clear register immediately
	}

	// Testing arg begin
	state = SEARCH;

	// Testing arg end

	// disable interrupt

	enum State next_state = state;

	switch (state) {
	case INIT:
		if (on_off == 1) {
			on_off = 0;
			next_state = SEARCH;
		}
		break;
	case SEARCH:
		if (on_off) {
			on_off = 0;
			next_state = RETURN;
		} else if (ball_locked) {
			ball_locked = 0;
			next_state = COLLECT;
		} else if (ball_not_found) {
			ball_not_found = 0;
			next_state = RETURN;
		}
		break;
	case COLLECT:
		if (on_off) {
			on_off = 0;
			next_state = RETURN;
		} else if (ball_collected) {
			if (ball_count < MAXLOAD) {
				next_state = SEARCH;
			} else {
				next_state = RETURN;
			}
			ball_collected = 0; // clear
		}
		break;
	case RETURN:
		if (on_off) {
			on_off = 0;
		}

		if (station_arrived) {
			station_arrived = 0;
			reset = 1;
			next_state = INIT;
		}

		break;
	case AVOID_COLLISION:
		// only interrupt can cause the robot to enter this state
		// only after certain conditions are met can the robot leave this state

		// set timer the first time entering this state

		if (on_off) {
			on_off = 0;
			prev_state = RETURN;
		}
		if (avoid_end) {
			avoid_end = 0;
			next_state = prev_state;
		}

		break;
	}

	state = next_state;
	if (state != AVOID_COLLISION) {
		prev_state = state;
	}

	// enable interrupt
}

void execute() {
	switch (state) {
	case INIT:
		break;
	case SEARCH:
		uint8_t mode = 0;

		// read RPI, change mode, set bits
		motor_control(mode);
		ball_locked;
		ball_not_found;
		break;
	case COLLECT:
		break;
	case RETURN:
		break;
	case AVOID_COLLISION:
		break;
	}
}
