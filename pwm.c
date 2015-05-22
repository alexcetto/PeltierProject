#include <msp430x14x.h>

void PWM_Init(void)
{
  TACTL = TASSEL1 + TACLR;              // S�lection du timer et remise � z�ro
  CCR0 = 1000-1;                         // PWM Periode/2
  CCTL1 = OUTMOD_2;                     // en mode reset/set, reset lorsqu'on arrive a CCR1, set quand on est arriv� � CCR0
  P1DIR |= BIT2 + BIT3;                 // P1.2 et P1.3 en sortie
  P1SEL |= BIT2;                        // P1.2 s�lection

  P1OUT |= BIT3;                               //DRV593 pin shutdown OFF
  TACTL |= MC1 + MC0;                           //reset counter (MC0) et up mode (MC1)
}

void PWM(int val)
{
  CCR1 = val;
}

void PWM_Stop(void)
{
  CCR1 = 499;
  P1OUT &= !BIT3;                               //DRV593 pin shutdown OFF
}
