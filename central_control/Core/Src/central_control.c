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
	static uint8_t on_off = 0;
	static uint8_t ball_locked = 0; // from AI
	static uint8_t ball_not_found = 0; // from AI
	static uint8_t ball_count = 0; // from IR
	static uint8_t ball_collected = 0; // from IR
	static uint8_t station_arrived = 0; // from AI
	static uint8_t reset = 0; // reset button

	// read reset button, and set reset bit to 1 only if reset button is pressed
	if (reset) {
		on_off = 0;
		ball_locked = 0;
		ball_not_found = 0;
		ball_count = 0;
		ball_collected = 0;
		station_arrived = 0;
		reset = 0;
		state = INIT;
		prev_state = state;
		// clear peripheral register
	} else {
		// reading peripheral input, and clear register immediately
	}

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
		next_state = prev_state;

		if (on_off) {
			on_off = 0;
			next_state = RETURN;
		}

		break;
	default:
		break;
	}

	state = next_state;
	prev_state = state;

	// enable interrupt

}

