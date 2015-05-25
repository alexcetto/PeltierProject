#include <msp430x14x.h>

/*
 * @brief : Initialize the PWM communication by setting the corrects values to the defined pins
 */
void PWM_Init(void) {
    TACTL = TASSEL1 + TACLR;                // Select TimerA and set it to 0
    CCR0 = 1000-1;                          // PWM Period, half is default
    CCTL1 = OUTMOD_2;                       // mode reset/set, reset when in CCR1, set when in CCR0 CCR0
    P1DIR |= BIT2 + BIT3;                   // P1.2 and P1.3 as output
    P1SEL |= BIT2;                          // P1.2 selection

    P1OUT |= BIT3;                          // DRV593 pin shutdown OFF
    TACTL |= MC1 + MC0;                     // Reset counter (MC0) et up mode (MC1)
}

void PWM(int val) {
    CCR1 = val;
}

void PWM_Stop(void) {
    CCR1 = 499;
    P1OUT &= !BIT3;                          // DRV593 pin shutdown OFF
}
