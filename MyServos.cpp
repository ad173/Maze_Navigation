#pragma once
#include "Arduino.h"
#include "MyServos.h"
#include "MyEncoders.h"

#include <Servo.h>

Servo LServo;  
Servo RServo;

Adafruit_RGBLCDShield dis = Adafruit_RGBLCDShield();
float RCalibratedReverseConversion = .07, RCalibratedForwardConversion = .07;
float LCalibratedReverseConversion = .07, LCalibratedForwardConversion = .07;
int LMicro, RMicro;
float radius, longD, shortD;
float rpsLeft, rpsRight;
  
sclass::sclass(){
}

void sclass::initServos()
{
  LServo.attach(3);  
  RServo.attach(2); 
  encoders.initEncoders(); 
}

void sclass::setSpeeds(int microsLeft, int microsRight){
  LServo.writeMicroseconds(microsLeft + 1500);
  RServo.writeMicroseconds(-microsRight + 1500);
  if(microsLeft == 0)
  {
    encoders.resetSpeed(0);
  }
  if(microsRight == 0)
  {
    encoders.resetSpeed(1);
  }
}

void sclass::setSpeedsRPS(float rpsLeft, float rpsRight)
{
  if(rpsLeft > 0)
  {
    LMicro = (rpsLeft * (.8 / LCalibratedForwardConversion)) * 10;
  }
  else if(rpsLeft < 0)
  {
    LMicro = (rpsLeft * (.8 / LCalibratedReverseConversion)) * 10;
  }
  else
  {
    LMicro = 0;
  }

  if(rpsRight > 0)
  {
    RMicro = (rpsRight * (.8 / RCalibratedForwardConversion)) * 10;
  }
  else if(rpsLeft < 0)
  {
    RMicro = (rpsRight * (.8 / RCalibratedReverseConversion)) * 10;
  }
  else
  {
    RMicro = 0;
  }
  setSpeeds(LMicro, RMicro);
}

void sclass::setSpeedsIPS(float ipsLeft, float ipsRight)
{
  if(ipsLeft > MAX_SPEED)
  {
    ipsLeft = MAX_SPEED;
  }
  if(ipsRight > MAX_SPEED)
  {
    ipsRight = MAX_SPEED;
  }
  rpsLeft = (ipsLeft / MAX_SPEED) * .8;
  rpsRight = (ipsRight  / MAX_SPEED) * .8 ;
  setSpeedsRPS(rpsLeft, rpsRight);
}

void sclass::calibrate();

void sclass::setSpeedSVW(float v, float w)
{
  radius = v / w;
  longD = (radius + 3.95 / 2) * w;
  shortD = (radius - 3.95 / 2) * w;

  if(w != 0)
  {
    setSpeedsIPS(shortD, longD);
  }
  else
  {
    setSpeedsIPS(v, v);
  }
}

