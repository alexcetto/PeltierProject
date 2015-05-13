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
    
    _EINT();

  return;
}


void UARTprintTerm(const char * str)
{
  int i;

    while(*str != '\0' )
    {
   	 while(!(IFG1& UTXIFG0));  // USART0 TX buffer pret?
   	 TXBUF0 = *str++;
    }

    for(i = 0; i < 10000; i = i+1)
    {
    }
}

void UARTmsgAccueil(void)
{
    UARTprintTerm("\n\r===========================================================================\n");
    UARTprintTerm("\rBievenue dans le menu de gestion de la communication avec le microcontrôleur\n");
    UARTprintTerm("\r===========================================================================\n");
    UARTprintTerm("\r===  Menu  ===\n");
    UARTprintTerm("\rTouche 't' : Affichage de la température\n");
    UARTprintTerm("\rTouche 's' : Arrêt de l'affichage de la température\n");
    UARTprintTerm("\rTouche 'c' : Saisi de la consigne de température\n");
    UARTprintTerm("\rTouche 'p' : Affichage de la température de consigne\n");
    UARTprintTerm("\rTouche 'l' : Démarrer la régulation PWM\n");
    UARTprintTerm("\rTouche 'm' : Arrêter la régulation PWM\n");
    UARTprintTerm("\n\rVotre choix ? ");
}
