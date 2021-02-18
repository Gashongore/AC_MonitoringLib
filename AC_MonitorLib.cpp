/**---------------------------------------------------------------------------------------------------------------------------------------------------------
 * Project : Electric Load Monitoring
 * Description: Using non-intrusive Current sensors to monitor electric loads and estimate their power
 * Author: Prince Gashongore
 * For: Kumva Insight (https://kumva.io)
 * Date: 18 February 2021
 
 * The library was heavily modified from the EmonLib by openenergymonitor community
 ------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*-------------------------------------------- Description of the modified library
  Emon.cpp - Library for openenergymonitor
  Created by Trystan Lea, April 27 2010
  GNU GPL
  modified to use up to 12 bits ADC resolution (ex. Arduino Due)
  by boredman@boredomprojects.net 26.12.2013
  Low Pass filter for offset removal replaces HP filter 1/1/2015 - RW
*/

// Proboscide99 10/08/2016 - Added ADMUX settings for ATmega1284 e 1284P (644 / 644P also, but not tested) in readVcc function

//#include "WProgram.h" un-comment for use on older versions of Arduino IDE


#include "AC_MonitorLib.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


//--------------------------------------------------------------------------------------
double Load_Monitor::calcIrms(CT_Property_Struct data){
    
    double Irms=0;
    
    offsetI = ADC_COUNTS>>1;
    
    static const unsigned int SupplyVoltage=3300;
   // static const unsigned int NUMBER_OF_SAMPLES=1662;

  for (unsigned int n = 0; n < 1662 /*Number_of_Samples*/; n++)
  {
    sampleI = analogRead(data.CT_PIN);

    // Digital low pass filter extracts the 2.5 V or 1.65 V dc offset,
    //  then subtract this - signal is now centered on 0 counts.
    offsetI = (offsetI + (sampleI-offsetI)/ADC_COUNTS);
    filteredI = sampleI - offsetI;

    // Root-mean-square method current
    // 1) square current values
    sqI = filteredI * filteredI;
    // 2) sum
    sumI += sqI;
  }

  double I_RATIO = data.ICAL*((SupplyVoltage/1000.0) / (ADC_COUNTS));
  Irms = I_RATIO * sqrt(sumI /1662 /*Number_of_Samples*/);

  //Reset accumulators
  sumI = 0;
  //--------------------------------------------------------------------------------------

  return Irms;
}

