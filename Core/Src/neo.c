/*
 * neo.c
 *
 *  Created on: Apr 6, 2025
 *      Author: ishanchitale
 */

#include "neo.h"
#include "spi.h"

void computeChecksum(uint8_t* payload, uint16_t len, uint8_t* ck_a, uint8_t* ck_b) {
	// Start from the CLASS Byte & ID Byte
	*ck_a = 0;
	*ck_b = 0;
	for (size_t i = 2; i < len; i++) {
		*ck_a = (*ck_a + payload[i])%0xFF;
		*ck_b = (*ck_a + *ck_b)%0xFF;
	}
}
