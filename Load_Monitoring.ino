

#include "AC_MonitorLib.h"         // Include Load_Monitoring Library


//--------------------------------------------------------------------------- Pins assignment/Functions definition-> DON'T CHANGE/MODIFY THESE                               
uint8_t CT1_PIN=A0;
uint8_t CT2_PIN=A1;
uint8_t CT3_PIN=A2;
uint8_t CT4_PIN=A3;
uint8_t CT5_PIN=A4;
uint8_t CT6_PIN=A5;

double Vrms=220;    // Standard Utility Voltage , Can be modified for better accuracy

Load_Monitor KUMVA_IO;     //Create an instance

/***************************************************************************************************************************************END
*/



/*------------------------PARAMETERS TO BE ADJUSTED PER DEVICE(i.e., Boron+CarrierBoard+CTs) ---------------------------*/


                               
//Record here calibrations constants for every CT connected and comment out unconnected CT

const double CT1_Cal = 90.91;     // calculated value is 100A:0.05A for transformer / 22 Ohms for resistor = 90.91, or 60.6
const double CT2_Cal = 90.91;     // calculated value is 100A:0.05A for transformer / 22 Ohms for resistor = 90.91, or 60.6
const double CT3_Cal = 90.91;     // calculated value is 100A:0.05A for transformer / 22 Ohms for resistor = 90.91, or 60.6
const double CT4_Cal = 16.67;     // calculated value is 100A:0.05A for transformer / 120 Ohms for resistor
const double CT5_Cal = 90.91;     // calculated value is 100A:0.05A for transformer / 22 Ohms for resistor = 90.91, or 60.6
const double CT6_Cal = 16.67;     // calculated value is 100A:0.05A for transformer / 120 Ohms for resistor
//-----------------------------------------------------------------------------------------------------------



//Assign loads pins here: record sensor type with calibration constant and the pins where its connected

/*examples of Single phase loads
 
 * a load is a single phase when it use only one CT
 * is connected to CTx (Ax)
 * The CT used calibration constant is CTx_Cal: calibration const; 
 * Create a struct as shown below: 
*/

//Lighting_FirstFloor load 
Load_Monitor::CT_Property_Struct Lighting_meter_FirstFloor={CT1_PIN, CT1_Cal};

//A single phase airconditionner
Load_Monitor::CT_Property_Struct AirConditionner={CT2_PIN,CT2_Cal};

//Conference_room load (Single phase)
Load_Monitor::CT_Property_Struct Conference_room={CT6_PIN,CT6_Cal};

/*examples of 3 phase loads 
 * a load is a three phase when it use 3 or 4 CTs
 * is connected to CTx1 (Ax1), CTx2 (Ax2), CTx3 (Ax3), CTx4 (Ax4)
 * Specifit the CTs used calibration constants
 * Create a struct as shown below: 
*/

// water pump
Load_Monitor::CT_Property_Struct WaterPump[3]={  // 3- wires Three phase laod
      {CT3_PIN,CT3_Cal}, // R phase
      {CT4_PIN,CT4_Cal}, // T phase
      {CT5_PIN,CT5_Cal} // S phase 
    };

//Boiler load
Load_Monitor::CT_Property_Struct Boiler_Hotel[4]={   // 4- wires Three phase Load 
      {CT3_PIN,CT3_Cal}, // R phase
      {CT4_PIN,CT4_Cal}, // T phase
      {CT5_PIN,CT5_Cal}, // S phase 
      {CT6_PIN,CT6_Cal} // N phase 
   };
   
//--------------- variables, these variable store the Irms and Aparent Power returned by Single_Phase_Monitor functuction 

//LightingFirstFloor load
 double I_LightingFirstFloor=0;
 double P_LightingFirstFloor=0;

//AirConditionner load
 double I_AirConditionner=0;
 double P_AirConditionner=0; 

//Conference_room load
 double I_Conference_room=0;
 double P_Conference_room=0; 


//Water pump laod 

double I_WaterPump[3]={0};
double P_WaterPump[3]={0};


//Boiler laod 

double I_Boiler[4]={0};
double P_Boiler[4]={0}; 

//-----------------------------------------------------------------------------------------------------------End


void setup()
{  
  Serial.begin(9600);
         
}

void loop()
{
 
//LightingFirstFloor load
 Single_Phase_Monitor(Lighting_meter_FirstFloor,&I_LightingFirstFloor,&P_LightingFirstFloor);
  // can be used like this for displaying and storing the data    
  Serial.print(I_LightingFirstFloor); //Irms
  Serial.println(P_LightingFirstFloor);	 // Apparent power	

//AirConditionner load
Single_Phase_Monitor(AirConditionner,&I_AirConditionner,&P_AirConditionner);
 // can be used like this for displaying and storing the data    
Serial.print(I_AirConditionner); //Irms
Serial.println(P_AirConditionner);   // Apparent power 


 //Conference_room load
Single_Phase_Monitor(Conference_room,&I_Conference_room,&P_Conference_room);
 // can be used like this for displaying and storing the data    
Serial.print(I_Conference_room); //Irms
Serial.println(P_Conference_room);   // Apparent power 



Three_Phase_Monitor(3,WaterPump,I_WaterPump,P_WaterPump); // call the function 

//can be used like this for displaying and storing the data    
Serial.print(I_WaterPump[0]); //Irms for R phase
Serial.print(I_WaterPump[1]); //Irms for S phase
Serial.print(I_WaterPump[2]); //Irms for T phase

Serial.print(P_WaterPump[0]); //Power for R phase
Serial.print(P_WaterPump[1]); //Power for S phase
Serial.print(P_WaterPump[2]); //Power for T phase


Three_Phase_Monitor(4,Boiler_Hotel,I_Boiler,P_Boiler); // call the function 

//can be used like this for displaying and storing the data    
Serial.print(I_Boiler[0]); //Irms for R phase
Serial.print(I_Boiler[1]); //Irms for S phase
Serial.print(I_Boiler[2]); //Irms for T phase
Serial.print(I_Boiler[3]); //Irms for N phase

Serial.print(P_Boiler[0]); //Power for R phase
Serial.print(P_Boiler[1]); //Power for S phase
Serial.print(P_Boiler[2]); //Power for T phase

}

/* these functions receive CT Properties in a struct and calculate both Current(RMS) and Aparent power for every phase
 * Ourput: Current(Rms) and Aparent Power per Phase 
 * Single phase: Max is 6
 * 3 wires Three phase: max is 2 
 * 4 wires Three phase: max is 1 
 */
//---------------------------------------------------------------------------------------------Single phase 

void Single_Phase_Monitor(Load_Monitor::CT_Property_Struct Load_Name, double *Current_rms,double *Power){
  
  double i_rms=0;
   i_rms=KUMVA_IO.calcIrms(Load_Name);
  
  *Current_rms=i_rms;
  *Power=((i_rms*Vrms)/1000); //in kW
}

//---------------------------------------------------------------------------------------------Three Phase

// Wires=3 for 3-Wires: R,S,T
// Wires=4 for 4 wires: R,S,T and N

void Three_Phase_Monitor(uint8_t Wires,Load_Monitor::CT_Property_Struct Load_Name[],double *Current_rms_per_Phase,double *Power_rms_per_Phase){
   uint8_t p=0;
   p=Wires;
   double i_rms_per_Phase[p]={0};
   
  for (uint8_t i=0;i<p;i++){

   i_rms_per_Phase[i]=KUMVA_IO.calcIrms(Load_Name[i]);
  
  Current_rms_per_Phase[i]=i_rms_per_Phase[i];
  Power_rms_per_Phase[i]=((i_rms_per_Phase[i]*Vrms)/1000); //in kW
    
  }
  
}
