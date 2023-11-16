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

class GPS
{
public:
    GPS();

    uint8_t SetGGA(string GGASentence);

    double GetLatitude();

    double GetLongitude();

    double GetSpeed();

private:
    int     UTC;
    double  latitude;
    double  longitude;
    double  altitude;
    double  speed;
    double  heading;
    int     numberSatellites;
    int     date;
};





#endif // GPS_H