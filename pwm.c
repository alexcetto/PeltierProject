#include <msp430x14x.h>

void PWM_Init(void)
{
  TACTL = TASSEL1 + TACLR;              // Sélection du timer et remise à zéro
  CCR0 = 1000-1;                        // PWM Periode/2
  CCTL1 = OUTMOD_2;                     // en mode reset/set, reset lorsqu'on arrive a CCR1, set quand on est arrivé à CCR0
  P1DIR |= 0x0C;                        // P1.2 et P1.3 en sortie
  P1SEL |= 0x04;                        // P1.2 sélection

  P1OUT &= ~0x08;                               //DRV593 pin shutdown OFF
  TACTL |= MC1 + MC0;                           //reset counter (MC0) et up mode (MC1)
}

void PWM_heater(void)
{
  CCR1 = 550;
}

void PWM_cooler(void)
{
  CCR1 = 450;
}
