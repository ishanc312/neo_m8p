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
#include "stddef.h"

#define NEO_CS_PORT GPIOB
#define NEO_CS_PIN GPIO_PIN_6
void NEO_Read(uint8_t* txData, uint8_t txLen, uint8_t* rxData, uint8_t rxLen);
void NEO_Command(uint8_t* txData, uint8_t txLen);
bool NEO_Write(uint8_t* txData, uint8_t txLen);

void pollVersion(uint8_t* rxData);
void loadConfig();
void saveConfig();
void clearConfig();
bool enableGPS();
bool enableAutomotiveMode();
void getSubframe();

#endif /* INC_NEO_H_ */
