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

int NAV_PVT_PARSE(uint8_t* rxData, float* data) {
	int status = -1;
	if (rxData[1] == 0x62 && rxData[2] == 0x01 && rxData[3] == 0x07) {
		// Ensure a correct header
		status = 0;

		if (rxData[20] > 0) {
			status = 1;
			// Means the satellite got a fix!
		}
	}
	int32_t unscaled_lon = (int32_t)((rxData[24] << 24) | (rxData[25] << 16) | (rxData[26] << 8) | (rxData[27]));
	float lon = unscaled_lon * 1e-7;
	int32_t unscaled_lat = (int32_t)((rxData[28] << 24) | (rxData[29] << 16) | (rxData[30] << 8) | (rxData[31]));
	float lat = unscaled_lat * 1e-7;

	data[0] = lon;
	data[1] = lat;

	return status;

}
