#include "stm32f1xx_hal.h"
#include "rf.h"
#include "sbus.h"

SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart1;

uint8_t packet[32];
uint16_t channels[6];

uint32_t crc32(uint8_t *data, int len)
{
    uint32_t crc = 0xFFFFFFFF;
    for(int i=0;i<len;i++)
    {
        crc ^= data[i];
        for(int j=0;j<8;j++)
            crc = (crc>>1) ^ (0xEDB88320 & (-(crc&1)));
    }
    return ~crc;
}

int main(void)
{
    HAL_Init();

    RF_Init();

    while(1)
    {
        if(RF_Receive(packet))
        {
            uint32_t rx_crc;
            memcpy(&rx_crc, &packet[24], 4);

            if(crc32(packet,24) == rx_crc)
            {
                for(int i=0;i<6;i++)
                {
                    channels[i] = packet[i*2] | (packet[i*2+1]<<8);
                }

                SBUS_Send(channels);
            }
        }
    }
}
