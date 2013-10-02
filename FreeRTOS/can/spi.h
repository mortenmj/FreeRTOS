/*
 * spi.h
 *
 * Created: 30.09.2013 20:08:38
 *  Author: mortenmj
 */ 


#ifndef SPI_H_
#define SPI_H_

void mcp2515_spi_port_init (void);
void mcp2515_spi_select (void);
void mcp2515_spi_unselect (void);
void mcp2515_spi_transfer (uint8_t dataOut, uint8_t *dataIn);

#endif /* SPI_H_ */