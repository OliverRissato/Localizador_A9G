#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <vector>
#include <string>

/******************************************************************************
* Defines section
******************************************************************************/

#define MINUTE_LENGTH 9 // Length of minutes data in NMEA
#define KNOTS_TO_MPS 0.514444444

using namespace std;

/* Typedef used to opaque pointer to GPS_t handler */
typedef struct GPS_struct_t GPS_t;

/******************************************************************************
* Function Prototype Section
******************************************************************************/

/**
 * Function used to initialize the MOTOR controller
 *
 * @param config Structure pointer with configurations
 * @return MOTOR_t handler to be used, NULL if failed
 * */
GPS_t* GPS_Init(void);

uint8_t GPS_SetGGA(GPS_t* gps, string GGASentence);

double GPS_GetLatitude(GPS_t* gps);

double GPS_GetLongitude(GPS_t* gps);

double GPS_GetSpeed(GPS_t* gps);











#endif // GPS_H