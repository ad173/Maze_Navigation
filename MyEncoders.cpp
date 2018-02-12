#pragma once
#include "Arduino.h"
#include "MyEncoders.h"
#include <PinChangeInt.h>

#define LeftPin 10
#define RightPin 11

int LCount = 0;
int Encoder_lastTime[2] = {millis(),millis()};
int RCount = 0;
int currentMillis;
float currentSpeeds[2] = {0.0, 0.0};

eclass::eclass(){
  
}

void eclass::resetCounts()
{
  LCount = 0;
  RCount = 0;  
}

void eclass::resetSpeed(int LR)
{
  currentSpeeds[LR] = 0;
}

void eclass::getCounts(int* counts)
{
  counts[0] = LCount;
  counts[1] = RCount;
}

void eclass::getSpeeds(float* speeds)
{
  speeds[0] = currentSpeeds[0];
  speeds[1] = currentSpeeds[1];
}

void LeftRising()
{
  LCount++;
  currentMillis = millis();
  currentSpeeds[0] = (1000.0 / (currentMillis - Encoder_lastTime[0])) / 32.0;
  Encoder_lastTime[0] = currentMillis; 
}

void RightRising()
{
  RCount++;
  currentMillis = millis();
  currentSpeeds[1] = (1000.0 / (currentMillis - Encoder_lastTime[1])) / 32.0;
  Encoder_lastTime[1] = currentMillis; 
}

void eclass::initEncoders(){
  pinMode(LeftPin, INPUT_PULLUP);
  pinMode(RightPin, INPUT_PULLUP);
   
  //PCintPort::attachInterrupt(LeftPin, &LeftRising, RISING);
  //PCintPort::attachInterrupt(RightPin, &RightRising, RISING);
}

