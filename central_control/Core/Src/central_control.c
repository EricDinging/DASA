/*
 * central_control.c
 *
 *  Created on: Mar 25, 2023
 *      Author: eric
 */
#include "main.h"
#include "central_control.h"

void central_control() {
	static uint8_t start_button = 0;
	static uint8_t ball_locked = 0;
	static uint8_t ball_count = 0;
	static uint8_t ball_collected = 0;

	switch (state) {
	case INIT:
		break;
	case SEARCH:
		break;
	case COLLECT:
		break;
	case RETURN:
		break;
	case ANTI_COLLISTION:
		break;
	default:
		break;
	}


}

