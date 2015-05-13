#include <msp430x14x.h>

void initUART(void){
  // RX1 = P3DIR = BIT5
  // TX1 = P3DIR = BIT4

/*    CCTL0 = CCIE;                       // CCR0 interrupt enabled
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
*/
 P3SEL |= 0x30;                        	// Selectionne les pins d'entrée/sortie de l'UART, PIN3.4,5 = USART0 TXD/RXD
  ME1 |= UTXE0 + URXE0;                 	// Active USART0 TXD/RXD
  UCTL0 |= CHAR;                        	// format des caractères = 8 bits
  UTCTL0 |= SSEL0;                      	// UCLK = ACLK 32.768kHz ,  UART Module avec une clock d'une certaine fréquence
  UBR00 = 0x03;                         	// 32k/9600 - 3.41 (on garde la partie entiere)- Met le générateur de baud a une valeur correcte pour obtenir un baud rate correct depuis la clock source
  UBR10 = 0x00;                         	//
  UMCTL0 = 0x4A;                        	// Modulation - 8 * 0,413 = 3,304 - 3 bits des 8 bits doivent avoir une valeur 1- ce qui donne 01001010 = 0x4A (voir rapport final/datasheet)
  UCTL0 &= ~SWRST;                      	// Initialize USART state machine
  IE1 |= URXIE0;                        	// Enable USART1 RX interrupt
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
    UARTprintTerm("\rBievenue dans la gestion du module peltier\n");
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
