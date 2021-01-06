/*
    -----------------------------------------
  Part of the openenergymonitor.org project
  Licence: GNU GPL V3
*/ 

#include "AC_Monitor.h"                   // Include Emon Library
AC_Monitor Config;
AC_Monitor ct1_test;                     // Create an instance

  int count = 0;
  float meassurements[20];
  float maximum = 0;
  float minimum = 0;
  float average_addition = 0;


void setup()
{ 
  Config.Monitor_Init();
   
  Serial.begin(9600);

  double iA,pA;
 
  ct1_test.SinglePhaseMonitor(0,&iA,&B);             // CT channel, calibration.
  
  delay(50);
  
}

void loop()
{
  ct1_test.Single_Phase_Monitor(0,&iA,&B);
  //ct1.Irms(1662);                 // 
  Serial.print(iA);
  Serial.println(' ');
  
  meassurements[count] =iA;
  count++;
  delay(50);

  if (count == 20){
    maximum = minimum = meassurements[0];
    average_addition = 0;
    for (int i=0; i<20; i++){
      if (meassurements[i] < minimum) minimum = meassurements[i];
      if (meassurements[i] > maximum) maximum = meassurements[i];
      average_addition += meassurements[i];      
    }
    float average = average_addition / 20;
    
    float nominador = 0;
    for(int i =0; i <20; i++){
       nominador += pow ((meassurements[i] - average),2);
    }
    float standard_deviation;
    standard_deviation = sqrt(nominador/20);
    
    Serial.println("Max Min Average Standard-deviation");
    Serial.print(maximum);
    Serial.print(' ');
    Serial.print(minimum);
    Serial.print(' ');
    Serial.print(average);
    Serial.print(' ');
    Serial.print(standard_deviation);
    Serial.println(' ');
    Serial.println(' ');
    
    count = 0;
    delay(10000);
  }
}
