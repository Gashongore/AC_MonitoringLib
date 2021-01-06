#include "AC_MonitorLib.h"
AC_Monitor Init;
AC_Monitor Ground_Floor_SinglePhase;
AC_Monitor Boiler_ThreePhase; //to calculate power we just need to add phase power

void setup(){

Init.Monitor_Init();

//Single Phase

int CT_A_Pin=2; // A2
double iA, pA; // current and power

Ground_Floor_SinglePhase.Single_Phase_Monitor(CT_A_Pin,&iA,&pA);

 Serial.print("Current:");Serial.println(iA);
 Serial.print("Power:");Serial.println(pA);

}

void loop(){


}