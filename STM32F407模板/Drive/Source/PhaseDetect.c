#include "stm32f4xx.h"

double Phase_detect(double adc,float freq)
{
	if(freq>10)
		adc*=1.00260;
	
	double phase1=0.0362758139421*adc+0.4190658245613-2.2;
  double phase2=phase1*0.9755616153993+1.7267476938987;
	return phase2;

}
