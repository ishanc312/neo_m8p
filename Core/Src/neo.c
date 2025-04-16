/*
 * neo.c
 *
 *  Created on: Apr 6, 2025
 *      Author: ishanchitale
 */

#include "neo.h"
#include "spi.h"

void computeChecksum(uint8_t *payload, uint16_t len, uint8_t *ck_a,
		uint8_t *ck_b) {
	// Start from the CLASS Byte & ID Byte
	*ck_a = 0;
	*ck_b = 0;
	for (size_t i = 2; i < len; i++) {
		*ck_a = (*ck_a + payload[i]) % 0xFF;
		*ck_b = (*ck_a + *ck_b) % 0xFF;
	}
}

int PVT_PARSE(uint8_t *rxData, float *data) {
	int status = -1;
	if (rxData[0] == 0xB5 && rxData[1] == 0x62 && rxData[2] == 0x01
			&& rxData[3] == 0x07) {
		// Ensure a correct header
		status = 0;
		if (rxData[26] > 0) {
			status = 1;
			// Means the satellite got a fix!
		}
	}

	int32_t unscaled_lon =
	    ((uint32_t)rxData[30])       |
	    ((uint32_t)rxData[31] << 8)  |
	    ((uint32_t)rxData[32] << 16) |
	    ((uint32_t)rxData[33] << 24);
	float lon = unscaled_lon * 1e-7;
	int32_t unscaled_lat =
		    ((uint32_t)rxData[34])       |
		    ((uint32_t)rxData[35] << 8)  |
		    ((uint32_t)rxData[36] << 16) |
		    ((uint32_t)rxData[37] << 24);
	float lat = unscaled_lat * 1e-7;
	data[0] = lat;
	data[1] = lon;

	return status;

}
