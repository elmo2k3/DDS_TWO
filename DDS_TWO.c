/*
 * DDS_TWO.c
 *
 * Created: 14.11.2012 12:55:29
 *  Author: Norrin
 */ 

#include <avr/io.h>
#include <util/delay.h>

#define DDS_PORT0 PORTB
#define DDS_CS PB0
#define DDS_SCK PB1
#define DDS_MOSI PB2
#define DDS_MISO PB3
#define DDS_SYNC_CLK PB4
#define DDS_MASTER_RESET PB5
#define DDS_IO_UPDATE PB6
#define DDS_IO_RESET PB7

#define DDS_PORT1 PORTE
#define RX PE0
#define TX PE1
#define DDS_OSK PE2
#define DDS_DROVER PE3
#define DDS_DRCTL PE4
#define DDS_PROFILE0 PE5
#define DDS_PROFILE1 PE6
#define DDS_PROFILE2 PE7


#define DDS_CS_SET() 	DDS_PORT0 |= (1<<DDS_CS);
#define DDS_CS_CLR() 	DDS_PORT0 &= ~(1<<DDS_CS);
#define DDS_DO_RESET()		DDS_PORT0 |= (1<<DDS_MASTER_RESET); DDS_PORT0 &= ~(1<<DDS_MASTER_RESET);
#define DDS_DO_IOUPDATE()	DDS_PORT0 |= (1<<DDS_IO_UPDATE); DDS_PORT0 &= ~(1<<DDS_IO_UPDATE);

#define DDS_REGISTER_CFR1 0x00
#define DDS_REGISTER_CFR1_LENGTH 4
#define DDS_REGISTER_CFR2 0x01
#define DDS_REGISTER_CFR2_LENGTH 4
#define DDS_REGISTER_CFR3 0x02
#define DDS_REGISTER_CFR3_LENGTH 4
#define DDS_REGISTER_FTW 0x07
#define DDS_REGISTER_FTW_LENGTH 4


#define FREQUENCY_FACTOR 10.73741824

static void dds_cmd(uint8_t address, uint8_t read_access, uint8_t *value, uint8_t size);
static void dds_cmd_32(uint8_t address, uint8_t read_access, uint32_t value, uint8_t size);
static void dds_cmd_64(uint8_t address, uint8_t read_access, uint64_t value, uint8_t size);
static void dds_set_single_tone_frequency(uint32_t frequency);
static uint8_t *uint32_to_uint8_array(uint32_t value);

void init(void);

int main(void)
{
	init();
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}


void init(void)
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
	
	// set REFCLK Multiplier to 40 and activate high frequency VCO range, PLL enable
//	dds_cmd_32(DDS_REGISTER_CFR3, 0, 0x3D1F4150, DDS_REGISTER_CFR3_LENGTH);
	dds_cmd_32(DDS_REGISTER_CFR3, 0, 0x17004100, DDS_REGISTER_CFR3_LENGTH);
//	dds_cmd_32(DDS_REGISTER_CFR1, 0, 0x00000002, DDS_REGISTER_CFR1_LENGTH);
	DDS_DO_IOUPDATE();
	
	dds_set_single_tone_frequency(10000);
}


void dds_set_single_tone_frequency(uint32_t frequency)
{
	// set AutoClr Phase Accum
	//dds_cmd_32(DDS_REGISTER_CFR1, 0, 0x00000200, DDS_REGISTER_CFR1_LENGTH);
	//dds_cmd_32(DDS_REGISTER_CFR1, 0, 0x00000000, DDS_REGISTER_CFR1_LENGTH);
	//dds_cmd_32(DDS_REGISTER_FTW,0, frequency, DDS_REGISTER_FTW_LENGTH);
	frequency = frequency*FREQUENCY_FACTOR;
	uint64_t val = 0x1111111100000000 | frequency;
	dds_cmd_64(0x0E,0, val, 8);
	DDS_DO_IOUPDATE();
}

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

static void dds_cmd_64(uint8_t address, uint8_t read_access, uint64_t value, uint8_t size)
{
	uint8_t *values;

	if(size>8) return;

	values = uint64_to_uint8_array(value);
	dds_cmd(address,read_access,values,size);
}

static void dds_cmd_32(uint8_t address, uint8_t read_access, uint32_t value, uint8_t size)
{
	uint8_t *values;

	if(size>4) return;

	values = uint32_to_uint8_array(value);
	dds_cmd(address,read_access,values,size);
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
