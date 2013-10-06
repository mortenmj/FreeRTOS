/*
 * spi.h
 *
 * Created: 30.09.2013 20:08:38
 *  Author: mortenmj
 */ 


#ifndef SPI_H_
#define SPI_H_

void spi_init (void);
uint8_t spi_transfer (uint8_t data_out);
void spi_write_block (uint8_t *data_out, uint8_t len);
void spi_readwrite_block (uint8_t *data_out, uint8_t *data_in, uint8_t len);

#endif /* SPI_H_ */