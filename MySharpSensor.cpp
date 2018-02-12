#pragma once
#include "Arduino.h"
#include "MySharpSensor.h"

int Lreadings[10] = {0,0,0,0,0,0,0,0,0,0};
int Rreadings[10] = {0,0,0,0,0,0,0,0,0,0}; 
int Freadings[10] = {0,0,0,0,0,0,0,0,0,0};

int Sensor_i, Sensor_x, med, Min, Sensor_temp;
const int SSensors[3] = {A1, A0, A2};

sensorClass::sensorClass()
{}

//this function takes as input the value obtained by a short distance sensor,
//and it should return its respective value in inches
float sensorClass::shortToInches(int value)
{
  return 10*pow(value * 0.0048828125, -1.15) * 0.394;
}

//this function takes as input the value obtained by the long distance sensor,
//and it should return its respective value in inche
float sensorClass::longToInches(int value)
{
  return 65*pow(value * 0.0048828125, -1.10) * 0.394;
}

//the following two functions are meant for combining (taking the mean, median or etc) multiple measurements
//this function should read and store a new measurement taken with sensor given by the parameter 'sensor'
void sensorClass::takeNewMeasurement(int sensor, int index)
{
  switch(sensor)
  {
    case SLEFT:
      Lreadings[index] = analogRead(SSensors[sensor]);
      break;
    case SRIGHT:
      Rreadings[index] = analogRead(SSensors[sensor]);
      break;
    case SSFRONT:
      Freadings[index] = analogRead(SSensors[sensor]);
      break;
  }
}

int median(int a[10])
{
  for(Sensor_x = 0; Sensor_x < 6; Sensor_x++)
  {
    Min = Sensor_x;
    for(Sensor_i = Sensor_x + 1; Sensor_i < 10; Sensor_i++)
    {
      if(a[Sensor_i] < a[Min])
      {
        Min = Sensor_i;
      }
    }
    Sensor_temp = a[Sensor_x];
    a[Sensor_x] = a[Min];
    a[Min] = Sensor_temp;
  }
  return (a[4] + a[5]) / 2;
}

//this function should combine the last N measurement taken with sensor 'sensor' and return the result.
//You are free to chose the value of N
float sensorClass::getCombinedDistance(int sensor)
{ 
  switch(sensor)
  {
    case SLEFT:
      med = median(Lreadings);
      break;
    case SRIGHT:
      med = median(Rreadings);
      break;
    case SSFRONT:
      med = median(Freadings);
      break;
  }
  
  return shortToInches(med);
}

