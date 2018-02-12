#ifndef __MY_SHARP_SENSOR__
#define __MY_SHARP_SENSOR__

#define SLEFT   0
#define SSFRONT 1
#define SRIGHT  2
#define SLONG   3

class sensorClass{
  public:
    sensorClass();
    //this function takes as input the value obtained by a short distance sensor,
    //and it should return its respective value in inches
    float shortToInches(int value);

    //this function takes as input the value obtained by the long distance sensor,
    //and it should return its respective value in inches
    float longToInches(int value);


    //the following two functions are meant for combining (taking the mean, median or etc) multiple measurements

    //this function should read and store a new measurement taken with sensor given by the parameter 'sensor'
    void takeNewMeasurement(int sensor, int index);


    //this function should combine the last N measurement taken with sensor 'sensor' and return the result.
    //You are free to chose the value of N
    float getCombinedDistance(int sensor);
  private:
    
};

extern sensorClass sensors;
#endif
