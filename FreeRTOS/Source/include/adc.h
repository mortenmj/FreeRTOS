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

void xAdcInit ( unsigned portBASE_TYPE uxQueueLength );

#endif /* ADC_H_ */