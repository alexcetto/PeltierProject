#include <msp430x14x.h>

void initUART(void){
  // RX1 = P3DIR = BIT5
  // TX1 = P3DIR = BIT4

    CCTL0 = CCIE;                       // CCR0 interrupt enabled
    CCR0 = 100;
    UCTL0 |= SWRST;                     // RESET UART
    UTCTL0 |= SSEL0;                    // UCLK = ACLK
    

    //Frequence: calcul du ratio 8MHz/9600 bauds
    UBR00 = 0x41;
    UBR10 = 0x03;
    
    UMCTL0 = 0;                         // Error modulation adjust
    UCTL0 = 0x11;                       // 8-bit character
    ME1 |= UTXE0 + URXE0;               // Enabled USART0 TXD/RXD
    UCTL0 &= ~SWRST;                    // release RESET UART
    IE1 |= URXIE0;

    P3SEL |= BIT4 + BIT5;               // P3.4,5 = USART0 TXD/RXD
    P3DIR |= BIT4;
    

  return;
}
