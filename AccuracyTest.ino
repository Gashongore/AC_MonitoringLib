#include "AC_MonitorLib.h"         // Include Load_Monitoring Library

//--------------------------------------------------------------------------- Pins assignment/Functions definition-> DON'T CHANGE/MODIFY THESE                               
uint8_t CT1_PIN=A0; // pin where the CT is connected 
const double CT1_Cal = 90.91; // calculated value is 100A:0.05A for transformer / 22 Ohms for resistor = 90.91, or 60.6

Load_Monitor KUMVA_IO;     //Create an instance

/***************************************************************************************************************************************END
*/
  //CT undertest property
   
Load_Monitor::CT_Property_Struct CT_UnderTest={CT1_PIN, CT1_Cal};
  int count = 0;
  float meassurements[20];
  float maximum = 0;
  float minimum = 0;
  float average_addition = 0;
 double CurrentRms=0;

void setup()
{ 
  
 
  Serial.begin(9600);

  delay(50);
  
}

void loop()
{
 
  
  double CurrentRms = KUMVA_IO.calcIrms(CT_UnderTest);  // Calculate Irms only, use this for CT accuracy testing only 
  Serial.print(CurrentRms);  
  Serial.println(' ');
  
  meassurements[count] =CurrentRms;
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
