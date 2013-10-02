/*
 * spi.h
 *
 * Created: 30.09.2013 20:08:38
 *  Author: mortenmj
 */ 


#ifndef SPI_H_
#define SPI_H_

void spi_init (void);
void spi_transfer (uint8_t dataOut, uint8_t *dataIn);

#endif /* SPI_H_ */