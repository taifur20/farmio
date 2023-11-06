/*
 * soil_sensor.h
 *
 *  Created on: Nov 5, 2023
 *      Author: Md. Khairul Alam
 */

#ifndef SOIL_SENSOR_H_
#define SOIL_SENSOR_H_

/*******************************************************************************
* Macros
*******************************************************************************/
#define SOIL_DATA_BITS_8     8
#define SOIL_STOP_BITS_1     1
#define SOIL_BAUD_RATE       9600
#define SOIL_RX_BUF_SIZE     1

#define DE P13_6 //IO3
#define RE P8_0 //IO4


/*******************************************************************************
* Function Prototypes
*******************************************************************************/
void init_soil_sensor(void);
void read_soil_sensor(void);
float read_ph(void);
float read_ph(void);
float read_temperature(void);
float read_moisture(void);
int read_conductivity(void);
int read_nitrogen(void);
int read_phosphorous(void);
int read_potassium(void);


#endif /* SOIL_SENSOR_H_ */
