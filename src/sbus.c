#include "sbus.h"
#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart1;

void SBUS_Send(uint16_t ch[6])
{
    uint8_t frame[25] = {0};

    frame[0] = 0x0F;

    uint16_t c[6];
    for(int i=0;i<6;i++)
        c[i] = ch[i] & 0x07FF;

    frame[1]  = (c[0] & 0xFF);
    frame[2]  = (c[0] >> 8) | ((c[1] & 0x07) << 3);
    frame[3]  = (c[1] >> 5) | ((c[2] & 0x3F) << 6);
    frame[4]  = (c[2] >> 2);
    frame[5]  = (c[2] >>10) | ((c[3] & 0x1FF) <<1);
    frame[6]  = (c[3] >>7) | ((c[4] &0x0F)<<4);
    frame[7]  = (c[4] >>4) | ((c[5] &0x01)<<7);
    frame[8]  = (c[5] >>1);

    frame[23] = 0x00;
    frame[24] = 0x00;

    HAL_UART_Transmit(&huart1, frame, 25, 10);
}
