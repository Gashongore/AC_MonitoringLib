

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif


#define ADC_BITS    12

#define ADC_COUNTS  (1<<ADC_BITS)


class Load_Monitor
{
  public:
     
    //For configuring connected CTs and pass the info to the calcrms();

    typedef struct CT_Data{
    //Get these from calibration.h ??
       int CT_PIN;
       const double ICAL;
      //offsetI = ADC_COUNTS>>1;

      }CT_Property_Struct;

      /* Input: Pin info struct
         Output: Current RMS
      */
    
    double calcIrms(CT_Property_Struct data);


  private:

    //--------------------------------------------------------------------------------------
    // Variable declaration for emon_calc procedure
    //--------------------------------------------------------------------------------------
    //sample_ holds the raw analog read value
    
    int sampleI;

    double filteredI;
    double offsetI;                          //Low-pass filter output

    double sqI,sumI,instP,sumP;              //sq = squared, sum = Sum, inst = instantaneous
};

