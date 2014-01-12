#include <avr/io.h>
#include <util/delay.h>
#include "ad9910.h"

#define DDS_REGISTER_CFR1 0x00
#define DDS_REGISTER_CFR1_LENGTH 4
#define DDS_REGISTER_CFR2 0x01
#define DDS_REGISTER_CFR2_LENGTH 4
#define DDS_REGISTER_CFR3 0x02
#define DDS_REGISTER_CFR3_LENGTH 4
#define DDS_REGISTER_FTW 0x07
#define DDS_REGISTER_FTW_LENGTH 4

#define PLL_ENABLE (1<<8)
#define PFD_RESET (1<<10)
#define REFCLK_IN_DIV_RESET_B (1<<14)
#define REFCLK_IN_DIV_BYPASS (1<<15)
#define PLL_N(X) ((uint32_t)X<<1)
#define PLL_ICP(X) ((uint32_t)X<<19)
#define PLL_VCO_SEL(X) ((uint32_t)X<<24)
#define PLL_DRV0(X) ((uint32_t)X<<28)

#define DDS_CS_SET() 	DDS_PORT0 |= (1<<DDS_CS);
#define DDS_CS_CLR() 	DDS_PORT0 &= ~(1<<DDS_CS);
#define DDS_DO_RESET()		DDS_PORT0 |= (1<<DDS_MASTER_RESET); DDS_PORT0 &= ~(1<<DDS_MASTER_RESET);
#define DDS_DO_IOUPDATE()	DDS_PORT0 |= (1<<DDS_IO_UPDATE); DDS_PORT0 &= ~(1<<DDS_IO_UPDATE);

static uint8_t *uint64_to_uint8_array(uint64_t value)
{
	uint8_t i;
	static uint8_t values[8];

	for(i=0;i<8;i++)
	{
		values[8-i-1] = (value>>((8-i-1)*8))&0xFF;
	}
	return values;
}

static uint8_t *uint32_to_uint8_array(uint32_t value)
{
	uint8_t i;
	static uint8_t values[4];

	for(i=0;i<4;i++)
	{
		values[4-i-1] = (value>>((4-i-1)*8))&0xFF;
	}
	return values;
}

static void dds_cmd(uint8_t address, uint8_t read_access, uint8_t *values, uint8_t size)
{
	uint8_t i;
	uint32_t temp;

	DDS_CS_CLR(); // Start transfer

	SPDR = (address&0x1F) | (read_access<<7);
	while(!(SPSR & (1<<SPIF))); // wait until transmission finished
	
	for(i=0;i<size;i++)
	{
		// Start with upper byte (MSB first)
		temp = values[size-i-1];
		SPDR = temp & 0xFF;
    	while(!(SPSR & (1<<SPIF))); // wait until transmission finished
	}

	DDS_CS_SET(); // End of transfer
	_delay_ms(1);
}

static void dds_cmd_32(uint8_t address, uint8_t read_access, uint32_t value, uint8_t size)
{
	uint8_t *values;

	if(size>4) return;

	values = uint32_to_uint8_array(value);
	dds_cmd(address,read_access,values,size);
}

static void dds_cmd_64(uint8_t address, uint8_t read_access, uint64_t value, uint8_t size)
{
	uint8_t *values;

	if(size>8) return;

	values = uint64_to_uint8_array(value);
	dds_cmd(address,read_access,values,size);
}

void ad9910_init()
{
	DDRB |= (1<<DDS_CS) | (1<<DDS_SCK) | (1<<DDS_MOSI) | (1<<DDS_MASTER_RESET) | (1<<DDS_IO_UPDATE) | (1<<DDS_IO_RESET);
	//DDRB |=  (1<<DDS_SCK) | (1<<DDS_MOSI) | (1<<DDS_MASTER_RESET) | (1<<DDS_IO_UPDATE) | (1<<DDS_IO_RESET);
	PORTB |= (1<<DDS_CS) | (1<<DDS_IO_RESET) | (1<<DDS_MASTER_RESET);
	_delay_us(1);
	PORTB &= ~((1<<DDS_IO_RESET) | (1<<DDS_MASTER_RESET));
	
	DDRE |= (1<<TX) | (1<<DDS_OSK) | (1<<DDS_DRCTL) | (1<<DDS_PROFILE0) | (1<<DDS_PROFILE1) | (1<<DDS_PROFILE2);
	PORTE |= (1<<DDS_OSK);
	PORTE &= ~(1<<DDS_DRCTL);
	
	//SPI
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0)|(0<<CPOL);
	SPSR |= (0<<SPI2X);
	
	DDS_DO_RESET();
	
	dds_cmd_32(DDS_REGISTER_CFR3, 0, PLL_N(40) | PLL_ENABLE | REFCLK_IN_DIV_RESET_B | PLL_DRV0(1) | PLL_VCO_SEL(5) | PLL_ICP(3), DDS_REGISTER_CFR3_LENGTH);
	DDS_DO_IOUPDATE();
	
	//dds_set_single_tone_frequency(10000);
}

void dds_set_single_tone_frequency(uint32_t frequency)
{
	uint64_t val = 0x1111111100000000|(0xFFFFFFFF>>3);
	dds_cmd_64(0x0E,0, val, 8);
	DDS_DO_IOUPDATE();
}
