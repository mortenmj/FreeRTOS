/*
 * adc.h
 *
 * Created: 05.09.2013 15:03:02
 *  Author: mortenmj
 */ 

#ifndef ADC_H_
#define ADC_H_

typedef enum
{
	adcPORT0,
	adcPORT1,
	adcPORT2,
	adcPORT3,
	adcPORT4,
	adcPORT5,
	adcPORT6,
	adcPORT7		
} eADCPort;

void vAdcInit ( unsigned portBASE_TYPE uxQueueLength );
signed portBASE_TYPE xAdcGetValue ( signed char *pcAdcVal, portTickType xBlockTime );
portBASE_TYPE xAdcGetPort ( void );
void vAdcStartConversion ( void );
portBASE_TYPE xAdcTakeSemaphore ( void );
void vAdcClose ( void );

#endif /* ADC_H_ */