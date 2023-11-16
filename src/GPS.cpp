/******************************************************************************
* Include section
******************************************************************************/
#include "GPS.h"
#include <Arduino.h>
#include <assert.h>
#include <sstream>
#include <stdlib.h>

/******************************************************************************
* Defines section
******************************************************************************/

GPS::GPS()
{
    UTC                 = 0;
    latitude            = 0;
    longitude           = 0;
    altitude            = 0;
    numberSatellites    = 0;
    speed               = 0;
    heading             = 0;
    date                = 0;
}

/******************************************************************************
* Forward declarations
******************************************************************************/
vector<string> _splitStringByComma(string input){

    vector<string>  returnVector;
    stringstream    ss(input);
    string          element;

    while(std::getline(ss, element, ',')) {
        returnVector.push_back(element);
    }


    return returnVector;
}
double _degreesToDecimal(int degrees, double minutes)
{
    double returnDouble = 0;

    returnDouble = degrees + minutes/60;

    return returnDouble;
}
double _stringToDouble(string inputString){

    //If string empty, return 0.
    double returnValue = 0;
    std::istringstream istr(inputString);

    istr >> returnValue;

    return (returnValue);

}

double _GpsToDecimalDegrees(string array)
{

    const char * nmeaPos = array.c_str(); 

    double v= 0;
    if(strlen(nmeaPos)>5)
    {
    char integerPart[3+1];
    int digitCount= (nmeaPos[4]=='.' ? 2 : 3);
    memcpy(integerPart, nmeaPos, digitCount);
    integerPart[digitCount]= 0;
    nmeaPos+= digitCount;
    v= atoi(integerPart) + atof(nmeaPos)/60.;

  }
  return v;
}


double _getCoordinates(string array){

    double decimalDegrees = 0;
    string degreeArray;
    string minuteArray;

    // Convert input array into two sub arrays containing the degrees and the minutes
    // Check for correct array length
    if (array.length() > MINUTE_LENGTH){

        degreeArray.assign(array.begin(), array.end() - MINUTE_LENGTH);
        minuteArray.assign(array.end() - MINUTE_LENGTH, array.end());

        // Convert strings into numbers
        int degrees;
        double minutes;
        degrees = atoi(degreeArray.c_str());
        minutes = _stringToDouble(minuteArray);

        // Convert degrees and mintues into decimal
        decimalDegrees = _degreesToDecimal(degrees,minutes);

    }
    return decimalDegrees;

}


bool _isValidGGA(const string GGASentence){
    bool returnBool = true;
    vector<std::string> elementVector = _splitStringByComma(GGASentence);

    if (elementVector[0] != "+GPSRD:$GNGGA")        returnBool = false;
    if (elementVector.size() != 15)                 returnBool = false;
    // if (atoi(elementVector[6].c_str()) == 0)        returnBool = false;
    // if (elementVector[4].length() < MINUTE_LENGTH)  returnBool = false;
    // if (elementVector[2].length() < MINUTE_LENGTH)  returnBool = false;
    // if (atoi(elementVector[7].c_str()) == 0)        returnBool = false;

    return returnBool;
}


/******************************************************************************
* Function Definition Section
******************************************************************************/
uint8_t GPS::SetGGA(string GGASentence)
{
    uint8_t success = 0;
    
    if(_isValidGGA(GGASentence))
    {
        vector<std::string> elementVector = _splitStringByComma(GGASentence);
        // Assert we have a GGA sentence
        assert(elementVector[0] == "+GPSRD:$GNGGA");

        this->UTC                 = atoi(elementVector[1].c_str());
        this->latitude            = _GpsToDecimalDegrees(elementVector[2]);
        if (elementVector[3] == "S") this->latitude  = (this->latitude * (-1.0f));
        this->longitude           = _GpsToDecimalDegrees(elementVector[4]);
        if (elementVector[5] == "W") this->longitude  = (this->longitude * (-1.0f));
        this->altitude            = _stringToDouble(elementVector[9]);
        this->numberSatellites    = atoi(elementVector[7].c_str());

        success = 1;
    }
    else
    {
        /* do nothing */
    }

    return success;
}

double  GPS::GetLatitude()
{
    return this->latitude;
}

double GPS::GetLongitude()
{
    return this->longitude;
}

double GPS::GetSpeed()
{
    return this->speed;
}