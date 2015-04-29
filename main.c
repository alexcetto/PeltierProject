#include "i2c.h"
#include "pwm.h"
#include <stdio.h>
#include <stdlib.h>
#include <msp430x14x.h>
#include <__cross_studio_io.h>



int consigneInt = 25;
int e = 0;
unsigned short retour;
char conversionStr[50];
int flagEcritureTemp = 0;

void changeClock(void)
{
  volatile unsigned int i;
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog
  /*
    OverClock en changeant de timer
  */  
  BCSCTL1 = 0xC0;                       //XT2 OFF, LFXT1 high frequency
  do 
  {
      IFG1 &= ~OFIFG;                       // Clear OSCFault flag
      for (i = 0xFF; i > 0; i--);           // Time for flag to set
  }
  while ((IFG1 & OFIFG) != 0);          // OSCFault flag still set?                
  BCSCTL2 = 0xC8;                       //MCLK & SMCLK = LFXT1
}

void main(void)
{
        int temp;
	int erreur;

	WDTCTL = WDTPW + WDTHOLD;
        changeClock();
        PWM_Init();

        for(;;)
        {
            temp = readTmp(0x94);   //lit la température du capteur TMP101, avec l'adresse de l'esclave
            erreur = temp;  //distincte l'erreur et la temperature
            if (erreur > 0)
            {
                  debug_printf("%d\n", temp);
                  PWM_heater();
            }
          }
    
}

