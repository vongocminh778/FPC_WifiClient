#include <Arduino.h>
#include <SPI.h>
#include "FPC1020.h"

#define FPC_IRQ 12
#define FPC_RST 13
#define PIN_SPI_SS 5
unsigned char rBuf[36864];
static const int spiClk = 1000000; // 1 MHz

//uninitalised pointers to SPI objects
SPIClass * vspi = NULL;

void FPC1020::init()
{
    vspi = new SPIClass(VSPI);
    // Chip select
    pinMode(PIN_SPI_SS, OUTPUT);
    digitalWrite(PIN_SPI_SS, HIGH);

    // IRQ / data ready
    pinMode(FPC_IRQ, INPUT);
    digitalWrite(FPC_IRQ, LOW);

    // RST
    pinMode(FPC_RST, OUTPUT);

    vspi->begin();
}

void FPC1020::reset()
{
    digitalWrite(FPC_RST, LOW);
    delay(10);
    digitalWrite(FPC_RST, HIGH);
}

void FPC1020::setup()
{
    setup_rev3();
}
/*-------------------------------------------------------------------------------------------*/



void FPC1020::setup_rev3()
{
//---------------------------------------------------------------------------------------------

    Serial.printf("0x%02X\n", transmit8(FPC102X_REG_ACTIVATE_IDLE_MODE, 0x00));
    
    Serial.printf("0x%02X\n", transmit8(FPC102X_REG_FINGER_DRIVE_DLY, 0x09));
  
    Serial.printf("0x%016llX\n", transmit64(FPC102X_REG_SAMPLE_PX_DLY, 0x0808090912121313ULL));// 0x0808090912121313ULL

    Serial.printf("0x%02X\n", transmit8(FPC102X_REG_FINGER_DRIVE_CONF, 0x02));//0x02 || 0x22 || 0x12
    

    Serial.printf("0x%04X\n", transmit16(FPC102X_REG_ADC_SHIFT_GAIN, 0x0a02));//0x0a02  ok

    Serial.printf("0x%04X\n", transmit16(FPC102X_REG_PXL_CTRL, 0x0f1e));//0x0f0a dam || 0x0f1e nhat || 0x0f1b mờ
    

    Serial.printf("0x%02X\n", transmit8(FPC102X_REG_IMAGE_SETUP, 0x03 | 0x08)); //0x03 | 0x08

    Serial.printf("0x%02X\n", transmit8(FPC102X_REG_IMG_RD, 0x0E));//0x0E

    Serial.printf("0x%02X\n", transmit8(FPC1020_REG_FNGR_DET_THRES, 0x50));

    Serial.printf("0x%04X\n", transmit16(FPC1020_REG_FNGR_DET_CNTR, 0x00FF));
    
    Serial.printf("0x%08X\n", transmit32(FPC102X_REG_IMG_CAPT_SIZE, 0x00C000C0));
  
}

uint8_t FPC1020::interrupt(bool clear)
{
    return transmit8(clear ? FPC102X_REG_READ_INTERRUPT_WITH_CLEAR : FPC102X_REG_READ_INTERRUPT, 0);
//    return transmit8(clear ? FPC102X_REG_READ_INTERRUPT : FPC102X_REG_READ_INTERRUPT_WITH_CLEAR, 0);
}

uint8_t FPC1020::error()
{
    return transmit8(FPC102X_REG_READ_ERROR_WITH_CLEAR, 0);
}

uint16_t FPC1020::hardware_id()
{
    return transmit16(FPC102X_REG_HWID, 0);
}

uint16_t FPC1020::finger_present_status()
{
    return transmit16(FPC102X_REG_FINGER_PRESENT_STATUS, 0);
}

uint64_t FPC1020::fpc1020_read_image()
{
    return transmit64(FPC1020X_REG_READ_IMAGE_DATA, 0);
}

void FPC1020::capture_image()
{  
    digitalWrite(FPC_IRQ, LOW);
    digitalWrite(PIN_SPI_SS, LOW);
    vspi->write(0xC4);
    vspi->write(0);
    for(word i=1; i<36865; i++)
        {
          
//          if(i % 922 ==0)
//          Serial.println("");
//          uint8_t status0 = vspi->transfer(0);
//          Serial.print(status0, HEX);
//          if(i == 36863){break;}
//          Serial.print(", ");
//          delay(0.1);
          rBuf[i] = vspi->transfer(0);
        }
    digitalWrite(PIN_SPI_SS, HIGH);
    Serial.println("");
}


void FPC1020::command(fpc1020_reg reg)
{
    digitalWrite(PIN_SPI_SS, LOW);
    vspi->write(reg);
    digitalWrite(PIN_SPI_SS, HIGH);
}


uint8_t FPC1020::transmit8(fpc1020_reg reg, uint8_t val)
{
    digitalWrite(PIN_SPI_SS, LOW);
    vspi->write(reg);
    uint8_t ret = vspi->transfer(val);
    digitalWrite(PIN_SPI_SS, HIGH);
    return ret;
}

uint16_t FPC1020::transmit16(fpc1020_reg reg, uint16_t val)
{
    digitalWrite(PIN_SPI_SS, LOW);
    vspi->write(reg);
    uint16_t ret = vspi->transfer16(val);
    digitalWrite(PIN_SPI_SS, HIGH);
    return ret;
}

uint32_t FPC1020::transmit32(fpc1020_reg reg, uint32_t val)
{
    digitalWrite(PIN_SPI_SS, LOW);

    vspi->write(reg);

    uint32_t out = 0;
    uint8_t *pout = (uint8_t *)&out;
    uint8_t *pin = (uint8_t *)&val;

    for (unsigned int i = 0; i < sizeof(uint32_t); i++)
    {
        pout[sizeof(uint32_t) - i - 1] = vspi->transfer(pin[sizeof(uint32_t) - i - 1]);
        // pout[i] = vspi->transfer(pin[i]);
    }

    digitalWrite(PIN_SPI_SS, HIGH);

    return out;
}

uint64_t FPC1020::transmit64(fpc1020_reg reg, uint64_t val)
{
    digitalWrite(PIN_SPI_SS, LOW);

    vspi->write(reg);

    uint64_t out = 0;
    uint8_t *pout = (uint8_t *)&out;
    uint8_t *pin = (uint8_t *)&val;

    for (unsigned int i = 0; i < sizeof(uint64_t); i++)
    {
        pout[sizeof(uint64_t) - i - 1] = vspi->transfer(pin[sizeof(uint64_t) - i - 1]);
        // pout[i] = vspi->transfer(pin[i]);
    }

    digitalWrite(PIN_SPI_SS, HIGH);

    return out;
}
