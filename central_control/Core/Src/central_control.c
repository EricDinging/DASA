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

// testing
// enum State state = COLLECT;
enum State state = INIT;

enum State prev_state;


uint8_t ball_locked = 0; // from AI
uint8_t ball_not_found = 0; // from AI
uint8_t ball_count = 0; // from IR
uint8_t ball_collected = 0; // from IR

uint8_t station_arrived = 0; // from AI
uint8_t return_near_station = 0;

uint8_t avoid_finished = 0; // from timer
uint8_t avoid_begin = 0;
uint8_t avoid_mode = 0; // 0: reverse, 1: rotate

uint8_t on_off = 0; // on_off button
uint8_t reset = 0; // reset button

uint8_t first_rotor = 0;

uint8_t ir_on = 0;
uint8_t speaker_on = 0;
uint8_t ultrasonic_on = 0;


const uint8_t XL = 120;
const uint8_t XR = 150;
const uint8_t Y_COLLECT = 170;
const uint8_t Y_RETURN = 50;
const float RETURN_STOP_SIZE = (315 * 207) * 3.0 / 4;
const uint8_t Y_FILTER = 70;
const uint8_t RETURN_UROFF_SIZE = (315 * 207) * 1.0 / 4;

void dumb_loop(int time) {
	 for(volatile int j = 0; j < time; ++j)
	  		  for (volatile int i = 0; i < 380; ++i) asm("");//1ms
}

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
		return_near_station = 0;
		avoid_finished = 0;
		avoid_begin = 0;
		reset = 0;
		state = INIT;
		prev_state = state;
		ir_on = 0;
		speaker_on = 0;
		ultrasonic_on = 0;
		avoid_mode = 0;
		first_rotor = 0;
	}

	// disable interrupt ultrasonic, ir
	HAL_NVIC_DisableIRQ(TIM3_IRQn);
	HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
	HAL_NVIC_DisableIRQ(TIM5_IRQn);

	enum State next_state = state;

	switch (state) {
	case INIT:
		printf("Init: \n");
		if (on_off == 1) {
			on_off = 0;
			__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_CC1); // clear ultrasonic interrupt
			next_state = SEARCH;
		}
		break;
	case SEARCH:
		printf("SEARCH: \n");

		if (ball_locked) {
			ball_locked = 0;
			__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_CC1); // clear IR interrupt pending bit
			next_state = SEARCH_TO_COLLECT;
		} else if (ball_not_found) {
			ball_not_found = 0;
			__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_CC1); // clear ultrasonic interrupt
			next_state = RETURN;

		} else {
			ultrasonic_on = 1;
		}
		break;
	case SEARCH_TO_COLLECT: {
		printf("Switching from Search to Collect.\n");

			printf("Actually going to COLLECT\n");
			__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_CC1); // clear IR interrupt pending bit
			first_rotor = 1;
			next_state = COLLECT;

		break;
	}

	case COLLECT_TO_SEARCH: {
		printf("Collect to search\n");
		__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_CC1); // clear ultrasonic interrupt
		next_state = SEARCH;
		break;
	}

	case COLLECT_TO_RETURN: {
		printf("Collect to return\n");
		__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_CC1); // clear ultrasonic interrupt
		next_state = RETURN;
		break;
	}

	case COLLECT: {
		printf("Collect: mode enter\n");

		static collect_enter_count = 0;

		if (ball_collected) {

			speaker_on = 1;

			printf("Collect: Ball_collected, ball_count: %d\n", ball_count);

			if (ball_count < MAXLOAD) {
				next_state = COLLECT_TO_SEARCH;
			} else {
				next_state = COLLECT_TO_RETURN;
			}
			ball_collected = 0; // clear
		} else {
			ir_on = 1;
			ultrasonic_on = 1;

			collect_enter_count++;
			// fail to find a ball
			if (collect_enter_count > 30) {
				__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_CC1); // clear ultrasonic interrupt
				next_state = COLLECT_TO_SEARCH;
				collect_enter_count = 0;
			}
		}
		break;
		}
	case RETURN:
		printf("return\n");

		if (station_arrived) {
			station_arrived = 0;
			reset = 1;
			next_state = INIT;
		} else {
			if (return_near_station == 1) {
				__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_CC1); // clear ultrasonic interrup
				ultrasonic_on = 0;
			} else {
				ultrasonic_on = 1;
			}
		}

		break;
	case AVOID_COLLISION:{

		uint8_t local_avoid_finished = avoid_finished;

		if (avoid_finished) {
			avoid_begin = 0;
			avoid_finished = 0;
			if (avoid_mode == 1) {
				avoid_mode = 0;
				next_state = prev_state;
				if (prev_state == COLLECT) {
					next_state = SEARCH;
				}
			} else {
				avoid_mode = 1;
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
	//	state = SEARCH;

	// Testing arg end

	HAL_NVIC_EnableIRQ(TIM5_IRQn);

	if (ir_on == 1) {
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
		ir_on = 0;
	}

	if (speaker_on == 1) {
		__HAL_TIM_CLEAR_FLAG(&htim5, TIM_SR_UIF);
		TIM5->ARR = 100;
		TIM5->CNT = 0;
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7,  1);
		HAL_TIM_Base_Start_IT(&htim5);
		speaker_on = 0;
	}

	if (ultrasonic_on == 1) {
		HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
		ultrasonic_on = 0;
	}

}

uint8_t get_mode() { // TODO::: ADD IF WE ARE SEARCHING FOR HOME BASE
  int8_t count = getBlocks(true, 1, 0xff);

  static int timecount = 0;
  if(count == 0) {
	  return 3;
  }
  if(count >= 1) {
	  uint16_t best_x = 0;
	  uint32_t best_size = 0;
	  uint16_t best_y = 0;
	  for(int i = 0; i < count; ++i) {
		  uint32_t temp_size = pixy.curr_blocks[i].m_width * pixy.curr_blocks[i].m_height;
//		  printf("temp_x: %d, Temp_size: %d\n", pixy.curr_blocks[i].m_x, temp_size);
		  if(temp_size > best_size && pixy.curr_blocks[i].m_y > Y_FILTER ) {
			  best_size = temp_size;
			  best_x = pixy.curr_blocks[i].m_x;
			  best_y = pixy.curr_blocks[i].m_y;
		  }
	  }

	  if (best_size == 0) {
		  return 3;
	  }

	  timecount++;
	  printf("%d--- Best_x: %d, Best_y: %d\n",timecount, best_x, best_y);

	  if(best_x < XL) {
//		  printf("Best_x : %d, Going to direction: %d\n", best_x, 1);
//		  printf("Best_y : %d \n", best_y);
		  return 1;
	  }
	  if(best_x < XR) {
//		  printf("Best_x : %d, Going to direction: %d\n", best_x, 0);
//		  printf("Best_y : %d \n", best_y);
		  if(best_y >= Y_COLLECT) {
			  printf("---------Collect\n");
			  ball_locked = 1;
		  }
		  return 0;
	  }
	  if(best_x <= 315) {
//		  printf("Best_x : %d, Going to direction: %d\n", best_x, 2);
//		  printf("Best_y : %d \n", best_y);
		  return 2;
	  }

  }
}

uint8_t return_mode() {
	int8_t count = getBlocks(true, 2, 0xff);

	if(count == 0) {
		  return 3;
	  }
	if(count >= 1) {
		uint16_t best_x = 0;
		uint32_t best_size = 0;
		uint16_t best_y = 0;
		for(int i = 0; i < count; ++i) {
			uint32_t temp_size = pixy.curr_blocks[i].m_width * pixy.curr_blocks[i].m_height;
	//		printf("temp_x: %d, Temp_size: %d\n", pixy.curr_blocks[i].m_x, temp_size);
			if(temp_size > best_size) {
				  best_size = temp_size;
				  best_x = pixy.curr_blocks[i].m_x;
				  best_y = pixy.curr_blocks[i].m_y - (pixy.curr_blocks[i].m_height / 2);
			}
		}

		if(best_x < XL) {
	//		  printf("Best_x : %d, Going to direction: %d\n", best_x, 1);
	//		  printf("Best_y : %d \n", best_y);
			  return 1;
		}
		if(best_x < XR) {
	//		  printf("Best_x : %d, Going to direction: %d\n", best_x, 0);
	//		  printf("Best_y : %d \n", best_y);
			if (return_near_station == 0) {
				if (best_size >= RETURN_UROFF_SIZE) {
					return_near_station = 1;
				}
			}

			if(best_size >= RETURN_STOP_SIZE) {
				printf("---------Return\n");
				station_arrived = 1;
			} else {
				station_arrived = 0;
			}
			return 0;
		}
		if(best_x <= 315) {
		//		  printf("Best_x : %d, Going to direction: %d\n", best_x, 2);
		//		  printf("Best_y : %d \n", best_y);
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
//		static uint8_t record[2] = {1, 1};
//
//		for (int i = 0; i < 1; i++) {
//			record[i] = record[i+1];
//		}

//		record[1] =  get_mode();

		uint8_t motor_mode = get_mode();
//		uint8_t motor_mode = 1;

//		if (record[0] == record[1]) {
//			motor_mode = record[1];
//		}
		motor_control(motor_mode);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,  1); // blue led

		break;
	}

	case COLLECT:

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 1); // green led
		rotor_control(0);
		motor_control(6);

		break;
	case RETURN:
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,  1); // white led
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,  1); // blue led
		printf("Green Button pressed\n");
		uint8_t return_motor_mode = return_mode();
		motor_control(return_motor_mode);
		break;
	case AVOID_COLLISION:
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 1); // red led

		if(avoid_mode == 0) {
			rotor_control(0);
			motor_control(4);
		} else {
			motor_control(1);
		}

		if (avoid_begin == 0) {
			__HAL_TIM_CLEAR_FLAG(&htim5, TIM_SR_UIF);
			avoid_begin = 1;
			if (avoid_mode == 0) {
				TIM5->ARR = 1000;
			} else {
				TIM5->ARR = 4000;
			}

			TIM5->CNT = 0;
			printf("Avoid_Collision: timer begin\n");
			HAL_TIM_Base_Start_IT(&htim5);
		}
		break;
	case SEARCH_TO_COLLECT : {
		rotor_control(0);
		motor_control(5);
		dumb_loop(15000);
//		printf("ending search to collect\n");
		break;
	}
	default :
		rotor_control(0);
		motor_control(7);
		dumb_loop(10000);
//		printf("ending search to collect\n");
		break;
	}
}

