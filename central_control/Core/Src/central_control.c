/*
 * central_control.c
 *
 *  Created on: Mar 25, 2023
 *      Author: eric
 */
#include "central_control.h"
#include "Pixy2.h"
#include "stdio.h"


extern Pixy2 pixy;

enum State state;
enum State prev_state;

uint8_t on_off = 0;
uint8_t ball_locked = 0; // from AI
uint8_t ball_not_found = 0; // from AI
uint8_t ball_count = 0; // from IR
uint8_t ball_collected = 0; // from IR
uint8_t station_arrived = 0; // from AI

uint8_t avoid_finished = 0; // from timer
uint8_t avoid_begin = 0;

uint8_t reset = 0; // reset button

// testing
uint32_t count = 0;

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
		avoid_finished = 0;
		avoid_begin = 0;
		reset = 0;
		state = INIT;
		prev_state = state;
		// clear peripheral register
	} else {
		// reading peripheral input, and clear register immediately
	}

	// disable interrupt ultrasonic, ir
	HAL_NVIC_DisableIRQ(TIM3_IRQn);
	HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
	HAL_NVIC_DisableIRQ(TIM5_IRQn);

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
	case COLLECT: {
		printf("Collect: mode enter\n");
		uint32_t local_count;
		local_count = count;

		if (on_off) {
			on_off = 0;
			next_state = RETURN;
		} else if (ball_collected) {

			__HAL_TIM_CLEAR_FLAG(&htim5, TIM_SR_UIF);
			TIM5->ARR = 100;
			TIM5->CNT = 0;
			HAL_TIM_Base_Start_IT(&htim5);
			HAL_NVIC_EnableIRQ(TIM5_IRQn); // speaker
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7,  1);

			printf("Collect: Ball_collected, ball_count: %d\n", ball_count);

			if (ball_count <= MAXLOAD) {
				next_state = SEARCH;
			} else {
				next_state = RETURN;
			}
			ball_collected = 0; // clear
		}
		break;
		}
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
	case AVOID_COLLISION:{
		// only interrupt can cause the robot to enter this state
		// only after certain conditions are met can the robot leave this state
    
		if (on_off) {
			on_off = 0;
			prev_state = RETURN;
		}

		uint8_t local_avoid_finished = avoid_finished;

		if (avoid_finished) {
			avoid_begin = 0;
			avoid_finished = 0;

			HAL_TIM_Base_Stop_IT(&htim5);

			next_state = prev_state;

			if (on_off) {
				on_off = 0;
				next_state = RETURN;
			}
		}
		break;
		}
	}

	state = next_state;
	if (state != AVOID_COLLISION) {
		prev_state = state;
	}

	// Testing arg begin
	state = COLLECT;
	if (state != AVOID_COLLISION) {
		prev_state = state;
	}
	// Testing arg end


	// enable interrupt
	if (state == COLLECT) {
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
//		HAL_NVIC_EnableIRQ(TIM5_IRQn); // speaker
	} else if (state == RETURN || state == SEARCH) {
		HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
	} else if (state == AVOID_COLLISION) {
		if (avoid_begin == 0) {
			__HAL_TIM_CLEAR_FLAG(&htim5, TIM_SR_UIF);
			avoid_begin = 1;
			TIM5->ARR = 5000;
			TIM5->CNT = 0;
			printf("Avoid_Collision: timer begin\n");
			HAL_TIM_Base_Start_IT(&htim5);
		}
		HAL_NVIC_EnableIRQ(TIM5_IRQn);
	}
}

uint8_t get_mode() { // TODO::: ADD IF WE ARE SEARCHING FOR HOME BASE
  int8_t count = getBlocks(true, 1, 0xff);
  //printf("Finding: %i blocks\n", count);
  if(count == 0) {
	  return 3;
  }
  if(count >= 1) {
	  uint16_t best_x = 0;
	  uint32_t best_size = 0;
	  for(int i = 0; i < count; ++i) {
		  uint32_t temp_size = pixy.curr_blocks[i].m_width * pixy.curr_blocks[i].m_height;
		  printf("temp_x: %d, Temp_size: %d\n", pixy.curr_blocks[i].m_x, temp_size);
		  if(temp_size > best_size) {
			  best_size = temp_size;
			  best_x = pixy.curr_blocks[i].m_x;
		  }
	  }

	  if(best_x < 105) {
		  printf("Best_x : %d, Going to direction: %d\n", best_x, 1);
		  return 1;
	  }
	  if(best_x < 210) {
		  printf("Best_x : %d, Going to direction: %d\n", best_x, 0);
		  return 0;
	  }
	  if(best_x <= 315) {
		  printf("Best_x : %d, Going to direction: %d\n", best_x, 2);
		  return 2;
	  }

  }
}


void execute() {
	//default
	rotor_control(1);
	motor_control(7);
	//led
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,  0);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,  0);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 0);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 0);

	switch (state) {
	case INIT:
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,  1); // white led
		break;
	case SEARCH: {
//		uint8_t motor_mode = get_mode();
//		motor_control(motor_mode);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,  1); // blue led

		while (1) {
			static uint8_t local_motor_mode = 0;
			if (local_motor_mode == 8) local_motor_mode = 0;
			printf("Search: motor mode %d\n", local_motor_mode);
			motor_control(local_motor_mode);

//			if (local_motor_mode == 0 || local_motor_mode == 4 || local_motor_mode == 5 || local_motor_mode == 6) {
//				HAL_Delay(1000);
//			} else {
//				HAL_Delay(5000);
//			}

			HAL_Delay(5000);
			local_motor_mode++;
		}

//		motor_control(1);
//		ball_locked;
//		ball_not_found;
		break;
	}
	case COLLECT:

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 1); // green led
		rotor_control(0);
		motor_control(0);

		break;
	case RETURN:

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,  1); // white led
		break;
	case AVOID_COLLISION:

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 1); // red led
		motor_control(1);
		break;
	}
}

