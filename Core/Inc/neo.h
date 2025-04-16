/*
 * neo.h
 *
 *  Created on: Apr 6, 2025
 *      Author: ishanchitale
 */

#ifndef INC_NEO_H_
#define INC_NEO_H_

#include "stdbool.h"
#include "stdint.h"

#define NEO_CS_PORT GPIOB
#define NEO_CS_PIN GPIO_PIN_6

void computeChecksum(uint8_t* payload, uint16_t len, uint8_t* ck_a, uint8_t* ck_b);
int PVT_PARSE(uint8_t* rxData, float* data);

#endif /* INC_NEO_H_ */
