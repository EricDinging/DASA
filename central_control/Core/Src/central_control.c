/*
 * central_control.c
 *
 *  Created on: Mar 25, 2023
 *      Author: eric
 */
#include "main.h"
#include "central_control.h"

void central_control() {
	// If start_button is pressed, the robot will exit INIT state and start execution.
	// When the robot returns to INIT state, the bit will be cleared automatically.
	// If the user press the button during robot's execution, the robot will enter RETURN
	// state immediately.
	static uint8_t start_button = 0;
	static uint8_t ball_locked = 0;
	static uint8_t ball_count = 0;
	static uint8_t ball_collected = 0;
	static uint8_t clear = 0;

	// reading

	// disable interrupt

	enum State next_state = state;

	switch (state) {
	case INIT:
		if (start_button) {
			next_state = SEARCH;
		}
		break;
	case SEARCH:
		if (!start_button) {
			next_state = RETURN;
		}
		break;
	case COLLECT:
		if (!start_button) {
			next_state = RETURN;
		}
		break;
	case RETURN:
		// doesn't care about start_button bit

		// if reached
		start_button = 0;
		// clear button
		break;
	case ANTI_COLLISION:
		// only interrupt can cause the robot to enter this state
		// only after certain conditions are met can the robot leave this state
		if (clear) {
			next_state = prev_state;
		}

		break;
	default:
		break;
	}

	state = next_state;
	prev_state = state;

	// enable interrupt
}

