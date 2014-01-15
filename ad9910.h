#ifndef __AD9910_H__
#define __AD9910_H__

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

#define PGA0 PG0
#define PGA1 PG1
#define PGA2 PG2
#define PGA3 PG3

void ad9910_init();
void dds_set_single_tone_frequency(uint16_t amplitude, uint32_t frequency);
void pga_set_gain(uint8_t gain);

#endif

