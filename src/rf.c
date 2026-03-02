#include "rf.h"

extern SPI_HandleTypeDef hspi1;

#define CE_HIGH() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)
#define CE_LOW()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET)
#define CSN_HIGH() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
#define CSN_LOW()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)

static const uint8_t hop_table[8] = {10,22,34,46,58,70,82,94};
static uint8_t hop_index = 0;

static void nrf_write(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg | 0x20, val};
    CSN_LOW();
    HAL_SPI_Transmit(&hspi1, buf, 2, 100);
    CSN_HIGH();
}

static uint8_t nrf_read(uint8_t reg)
{
    uint8_t tx[2] = {reg,0};
    uint8_t rx[2];
    CSN_LOW();
    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, 100);
    CSN_HIGH();
    return rx[1];
}

void RF_Init(void)
{
    HAL_Delay(100);
    CE_LOW();

    nrf_write(0x00, 0x0F);
    nrf_write(0x01, 0x00);
    nrf_write(0x02, 0x01);
    nrf_write(0x03, 0x03);
    nrf_write(0x04, 0x00);
    nrf_write(0x05, hop_table[0]);
    nrf_write(0x11, 32);

    CE_HIGH();
}

uint8_t RF_Receive(uint8_t *data)
{
    uint8_t status = nrf_read(0x07);
    if(status & 0x40)
    {
        CSN_LOW();
        uint8_t cmd = 0x61;
        HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);
        HAL_SPI_Receive(&hspi1, data, 32, 100);
        CSN_HIGH();

        nrf_write(0x07, 0x40);

        hop_index = (hop_index + 1) % 8;
        nrf_write(0x05, hop_table[hop_index]);

        return 1;
    }
    return 0;
}
