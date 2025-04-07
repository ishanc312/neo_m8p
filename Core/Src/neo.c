/*
 * neo.c
 *
 *  Created on: Apr 6, 2025
 *      Author: ishanchitale
 */

#include "neo.h"
#include "spi.h"

void NEO_Read(uint8_t* txData, uint8_t txLen, uint8_t* rxData, uint8_t rxLen) {
	HAL_GPIO_WritePin(NEO_CS_PORT, NEO_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3, txData, txLen, HAL_MAX_DELAY);
	uint8_t rxByte;
	uint8_t SPI_IDLE_BYTE = 0xFF;
	for (size_t k = 0; k < rxLen; k++) {
		HAL_SPI_TransmitReceive(&hspi3, &SPI_IDLE_BYTE, &rxByte, 1, HAL_MAX_DELAY);
		rxData[k] = rxByte;
	}
	HAL_GPIO_WritePin(NEO_CS_PORT, NEO_CS_PIN, GPIO_PIN_SET);
}

void NEO_Command(uint8_t* txData, uint8_t txLen) {
	HAL_GPIO_WritePin(NEO_CS_PORT, NEO_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3, txData, txLen, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(NEO_CS_PORT, NEO_CS_PIN, GPIO_PIN_SET);
}

bool NEO_Write(uint8_t* txData, uint8_t txLen) {
	uint8_t ack[6];
	NEO_Read(txData, txLen, ack, 6);
	if (ack[4] == 0x01) return true;
	return false;
}

void pollVersion(uint8_t* rxData) {
	uint8_t request[4] = {0xB5, 0x62, 0x0A, 0x04};
	NEO_Read(request, 4, rxData, 40);
}

bool enableGPS() {
	uint8_t txData[16] = {0xB5, 0x62, 0x06, 0x3E,
	0, 0, 16, 1,
	0, 16, 16, 0,
	0, 0x01, 0, 0b00000001
	};
	// We set 16 tracking channels for the GPS
	return NEO_Write(txData, 16);
}

bool enableAutomotiveMode() {
	uint8_t txData[40] = {0xB5, 0x62, 0x06, 0x24,
	0b00000100, 0b00000001, 4,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	// The mask here applies the UTC + Dynamic Model Setting
	return NEO_Write(txData, 40);
}

void getSubframe() {
	uint8_t request[4] = {0xB5, 0x62, 0x02, 0x13};
	uint8_t subframe[8+4*10];
	NEO_Read(request, 4, subframe, 8+4*10);
	// Figure out how to parse
}

// We use the same mask for the settings we want to clear/save/load
void clearConfig() {
	uint8_t txData[16] = {0xB5, 0x62, 0x06, 0x09,
	0x00, 0x00, 0b00011111, 0b00011111,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
	};
	NEO_Command(txData, 16);
}

void saveConfig() {
	uint8_t txData[16] = {0xB5, 0x62, 0x06, 0x09,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0b00011111, 0b00011111,
	0x00, 0x00, 0x00, 0x00
	};
	NEO_Command(txData, 16);
}

void loadConfig() {
	uint8_t txData[16] = {0xB5, 0x62, 0x06, 0x09,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0b00011111, 0b00011111
	};
	NEO_Command(txData, 16);
}


