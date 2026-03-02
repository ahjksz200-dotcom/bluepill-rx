#ifndef RF_H
#define RF_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

void RF_Init(void);
uint8_t RF_Receive(uint8_t *data);

#endif
