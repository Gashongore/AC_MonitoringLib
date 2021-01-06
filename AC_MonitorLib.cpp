/*
  Emon.cpp - Library for openenergymonitor
  Created by Trystan Lea, April 27 2010
  GNU GPL
  modified to use up to 12 bits ADC resolution (ex. Arduino Due)
  by boredman@boredomprojects.net 26.12.2013
  Low Pass filter for offset removal replaces HP filter 1/1/2015 - RW
*/

/*
  Emon.h - Library for openenergymonitor
  Created by Trystan Lea, April 27 2010
  GNU GPL
  modified to use up to 12 bits ADC resolution (ex. Arduino Due & Spark Core/Photon)
  by boredman@boredomprojects.net 26.12.2013
  Low Pass filter for offset removal replaces HP filter 1/1/2015 - RW
*/

/****************************************************************************************

Modified for multiphase and renamed AC_MOnitorLib
by Prince Gashongore

2020/12/19

****************************************************************************************/

#include "AC_MonitorLib.h"

#include "application.h"

//EnergyMonitor ct1, ct2, ct3, ct4, ct5, ct6; // Create an instance for all CTs 



const uint8_t Vrms=230; //Assume the voltage is constant (we don't have Voltage sensors)
const uint16_t no_of_samples=1662; // To Do, Experiment with an optimal one 


//--------------------------------------------------------------------------------------
// Sets the pins to be used for voltage and current sensors
//--------------------------------------------------------------------------------------
void EnergyMonitor::voltage(unsigned int _inPinV, double _VCAL, double _PHASECAL)
{
   inPinV = _inPinV;
   VCAL = _VCAL;
   PHASECAL = _PHASECAL;
   offsetV = ADC_COUNTS>>1;
}

void EnergyMonitor::current(unsigned int _inPinI, double _ICAL)
{
   inPinI = _inPinI;
   ICAL = _ICAL;
   offsetI = ADC_COUNTS>>1;
}


 ///-------------------------------------------------------------------------------------------------------------------------------------------
//SETUP, call this function in the void setup() function 
//-------------------------------------------------------------------------------------------------------------------------------------------

void AC_Monitor::Monitor_Init(void)
{

 EnergyMonitor ct1, ct2, ct3, ct4, ct5, ct6; // Create an instance for all CTs 

//EnergyMonitor ct1, ct2, ct3, ct4, ct5, ct6; // Create an instance for all CTs 
//__________________________________________________________________________NEW 

const double Ical1=                90.9;                                 // (2000 turns / 22 Ohm burden) = 90.9
const double Ical2=                90.9;                                 // (2000 turns / 22 Ohm burden) = 90.9
const double Ical3=                90.9;                                 // (2000 turns / 22 Ohm burden) = 90.9
const double Ical4=                16.67;                                // (2000 turns / 120 Ohm burden) = 16.67
const double Ical5=                90.9;                                 // (2000 turns / 22 Ohm burden) = 90.9
const double Ical6=                90.9;                                 // (2000 turns / 22 Ohm burden) = 90.9

//http://openenergymonitor.org/emon/buildingblocks/calibration

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN Initialize the AC_Monitoring
  /***** Check available/connected CTs on the carrier board***/

  
  boolean CT_flag[6]={FALSE}; // flag to chech the availability of the CTS

  uint8_t CT_count=0;


//----------------------------Industrial Monitoring 6 CTs Settings---------------------------------------------

   // check to see if CT is connected to CT1 input, if so enable that channel

   // check to see if CT is connected to CT1 input, if so enable that channel
  if (analogRead(A0) > 0) {CT_flag[0]= TRUE;CT_count++;ct1.current(A0, Ical1);} else CT_flag[0]=FALSE;
  if (analogRead(A1) > 0) {CT_flag[1]= TRUE;CT_count++;ct2.current(A1, Ical2);} else CT_flag[1]=FALSE;              // check to see if CT is connected to CT2 input, if so enable that channel
  if (analogRead(A2) > 0) {CT_flag[2]= TRUE;CT_count++;ct3.current(A2, Ical3);} else CT_flag[2]=FALSE;              // check to see if CT is connected to CT3 input, if so enable that channel
  if (analogRead(A3) > 0) {CT_flag[3]= TRUE;CT_count++;ct4.current(A3, Ical4);} else CT_flag[3]=FALSE;              // check to see if CT is connected to CT4 input, if so enable that channel
  if (analogRead(A4) > 0) {CT_flag[4]= TRUE;CT_count++;ct5.current(A4, Ical5);} else CT_flag[4]=FALSE;              // check to see if CT is connected to CT5 input, if so enable that channel
  if (analogRead(A5) > 0) {CT_flag[5]= TRUE;CT_count++;ct6.current(A5, Ical6);} else CT_flag[5]=FALSE;              // check to see if CT is connected to CT6 input, if so enable that channel

  
    if (CT_count==0) {
     // Serial.println("NO CT's detected"); // this can be adapted to Particle API
    } else {
      for(uint8_t i=0;i<=5;i++){
      //Serial.println("CT%d detected on %d",i,CT_count); // 
       Serial.print(CT_flag[i]); // For Device Status report, can use this information in the config function below, (TO DO)
      }
     
    }
}




//--------------------------------------------------------------------------------------
// emon_calc procedure
// Calculates realPower,apparentPower,powerFactor,Vrms,Irms,kWh increment
// From a sample window of the mains AC voltage and current.
// The Sample window length is defined by the number of half wavelengths or crossings we choose to measure.
//--------------------------------------------------------------------------------------
void EnergyMonitor::calcVI(unsigned int crossings, unsigned int timeout)
{
	int SupplyVoltage=3300;

  unsigned int crossCount = 0;                             //Used to measure number of times threshold is crossed.
  unsigned int numberOfSamples = 0;                        //This is now incremented  

  //-------------------------------------------------------------------------------------------------------------------------
  // 1) Waits for the waveform to be close to 'zero' (mid-scale adc) part in sin curve.
  //-------------------------------------------------------------------------------------------------------------------------
  boolean st=false;                                  //an indicator to exit the while loop

  unsigned long start = millis();    //millis()-start makes sure it doesnt get stuck in the loop if there is an error.

  while(st==false)                                   //the while loop...
  {
     startV = analogRead(inPinV);                    //using the voltage waveform
     if ((startV < (ADC_COUNTS*0.55)) && (startV > (ADC_COUNTS*0.45))) st=true;  //check its within range
     if ((millis()-start)>timeout) st = true;
  }
  
  //-------------------------------------------------------------------------------------------------------------------------
  // 2) Main measurement loop
  //------------------------------------------------------------------------------------------------------------------------- 
  start = millis(); 

  while ((crossCount < crossings) && ((millis()-start)<timeout)) 
  {
    numberOfSamples++;                       //Count number of times looped.
    lastFilteredV = filteredV;               //Used for delay/phase compensation  
    
    //-----------------------------------------------------------------------------
    // A) Read in raw voltage and current samples
    //-----------------------------------------------------------------------------
    sampleV = analogRead(inPinV);                 //Read in raw voltage signal
    sampleI = analogRead(inPinI);                 //Read in raw current signal

    //-----------------------------------------------------------------------------
    // B) Apply digital low pass filters to extract the 2.5 V or 1.65 V dc offset,
    //     then subtract this - signal is now centred on 0 counts.
    //-----------------------------------------------------------------------------
    offsetV = offsetV + ((sampleV-offsetV)/1024);
    filteredV = sampleV - offsetV;
    offsetI = offsetI + ((sampleI-offsetI)/1024);
    filteredI = sampleI - offsetI;
   
    //-----------------------------------------------------------------------------
    // C) Root-mean-square method voltage
    //-----------------------------------------------------------------------------  
    sqV= filteredV * filteredV;                 //1) square voltage values
    sumV += sqV;                                //2) sum
    
    //-----------------------------------------------------------------------------
    // D) Root-mean-square method current
    //-----------------------------------------------------------------------------   
    sqI = filteredI * filteredI;                //1) square current values
    sumI += sqI;                                //2) sum 
    
    //-----------------------------------------------------------------------------
    // E) Phase calibration
    //-----------------------------------------------------------------------------
    phaseShiftedV = lastFilteredV + PHASECAL * (filteredV - lastFilteredV); 
    
    //-----------------------------------------------------------------------------
    // F) Instantaneous power calc
    //-----------------------------------------------------------------------------   
    instP = phaseShiftedV * filteredI;          //Instantaneous Power
    sumP +=instP;                               //Sum  
    
    //-----------------------------------------------------------------------------
    // G) Find the number of times the voltage has crossed the initial voltage
    //    - every 2 crosses we will have sampled 1 wavelength 
    //    - so this method allows us to sample an integer number of half wavelengths which increases accuracy
    //-----------------------------------------------------------------------------       
    lastVCross = checkVCross;                     
    if (sampleV > startV) checkVCross = true; 
                     else checkVCross = false;
    if (numberOfSamples==1) lastVCross = checkVCross;                  
                     
    if (lastVCross != checkVCross) crossCount++;
  }
 
  //-------------------------------------------------------------------------------------------------------------------------
  // 3) Post loop calculations
  //------------------------------------------------------------------------------------------------------------------------- 
  //Calculation of the root of the mean of the voltage and current squared (rms)
  //Calibration coefficients applied. 
  
  double V_RATIO = VCAL *((SupplyVoltage/1000.0) / (ADC_COUNTS));
  Vrms = V_RATIO * sqrt(sumV / numberOfSamples); 
  
  double I_RATIO = ICAL *((SupplyVoltage/1000.0) / (ADC_COUNTS));
  Irms = I_RATIO * sqrt(sumI / numberOfSamples); 

  //Calculation power values
  realPower = V_RATIO * I_RATIO * sumP / numberOfSamples;
  apparentPower = Vrms * Irms;
  powerFactor=realPower / apparentPower;

  //Reset accumulators
  sumV = 0;
  sumI = 0;
  sumP = 0;
//--------------------------------------------------------------------------------------       
}

//--------------------------------------------------------------------------------------
double EnergyMonitor::calcIrms(unsigned int Number_of_Samples)
{
  
  int SupplyVoltage=3300;

  for (unsigned int n = 0; n < Number_of_Samples; n++)
  {
    sampleI = analogRead(inPinI);

    // Digital low pass filter extracts the 2.5 V or 1.65 V dc offset, 
  //  then subtract this - signal is now centered on 0 counts.
    offsetI = (offsetI + (sampleI-offsetI)/1024);
  filteredI = sampleI - offsetI;

    // Root-mean-square method current
    // 1) square current values
    sqI = filteredI * filteredI;
    // 2) sum 
    sumI += sqI;
  }

  double I_RATIO = ICAL *((SupplyVoltage/1000.0) / (ADC_COUNTS));
  Irms = I_RATIO * sqrt(sumI / Number_of_Samples); 

  //Reset accumulators
  sumI = 0;
//--------------------------------------------------------------------------------------             
 
  return Irms;
}


 void AC_Monitor::Single_Phase_Monitor(uint8_t CT_ID, double *Current, double *Power){
  
  EnergyMonitor ct1, ct2, ct3, ct4, ct5, ct6; // Create an instance for all CTs 

  // Measured current from CTs
  RMS_Current_CT1=0;
  RMS_Current_CT2=0;
  RMS_Current_CT3=0;
  RMS_Current_CT4=0;
  RMS_Current_CT5=0;
  RMS_Current_CT6=0;


  //Estimated Power from each CTs
   apparentPower_CT1=0;
   apparentPower_CT2=0;
   apparentPower_CT3=0;
   apparentPower_CT4=0;
   apparentPower_CT5=0;
   apparentPower_CT6=0;

    switch(CT_ID){

      case 1 :
   
      RMS_Current_CT1=ct1.calcIrms(no_of_samples);

      apparentPower_CT1=RMS_Current_CT1*Vrms;

      *Current=RMS_Current_CT1;
      *Power=(apparentPower_CT1/1000);

      break;

      case 2 :
   
      RMS_Current_CT2=ct2.calcIrms(no_of_samples);
      
      apparentPower_CT2=RMS_Current_CT2*Vrms;

      *Current=RMS_Current_CT2;
      *Power=(apparentPower_CT2/1000);

      break;

      case 3 :
   
      RMS_Current_CT3=ct3.calcIrms(no_of_samples);
      
      apparentPower_CT3=RMS_Current_CT3*Vrms;

      *Current=RMS_Current_CT3;
      *Power=(apparentPower_CT3/1000);

      break;


      case 4 :
   
      RMS_Current_CT4=ct4.calcIrms(no_of_samples);

      apparentPower_CT4=RMS_Current_CT4*Vrms;

      *Current=RMS_Current_CT4;
      *Power=(apparentPower_CT4/1000);

      break;

      case 5 :
   
      RMS_Current_CT5=ct5.calcIrms(no_of_samples);
      
      apparentPower_CT5=RMS_Current_CT5*Vrms;

      *Current=RMS_Current_CT5;
      *Power=(apparentPower_CT5/1000); // in Kw

      break;

      case 6 :
   
      RMS_Current_CT6=ct6.calcIrms(no_of_samples);
      apparentPower_CT6=RMS_Current_CT6*Vrms;

      *Current=RMS_Current_CT6;
      *Power=(apparentPower_CT6/1000);

      break;

    }

}


void EnergyMonitor::serialprint()
{
    Serial.print(realPower);
    Serial.print(' ');
    Serial.print(apparentPower);
    Serial.print(' ');
    Serial.print(Vrms);
    Serial.print(' ');
    Serial.print(Irms);
    Serial.print(' ');
    Serial.print(powerFactor);
    Serial.println(' ');
    delay(100); 
}
