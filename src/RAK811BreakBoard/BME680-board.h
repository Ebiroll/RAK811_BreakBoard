#ifndef __BME680_BOARD_H_
#define __BME680_BOARD_H_

int8_t BME680_Init( void );
int8_t BME680_read( int16_t * temprature, uint32_t * pressure, uint32_t * humidity, uint32_t * resistance);

#endif
