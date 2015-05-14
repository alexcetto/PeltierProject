#include <msp430x14x.h>

void initUART(void){
  
  P3SEL |= 0x30;                        	// Selectionne les pins d'entr�e/sortie de l'UART, PIN3.4,5 = USART0 TXD/RXD
  ME1 |= UTXE0 + URXE0;                 	// Active USART0 TXD/RXD
  UCTL0 |= CHAR;                        	// format des caract�res = 8 bits
  UTCTL0 |= SSEL0;                      	// UCLK = ACLK 32.768kHz ,  UART Module avec une clock d'une certaine fr�quence
  UBR00 = 0x03;                         	// 32k/9600 - 3.41 (on garde la partie entiere)- Met le g�n�rateur de baud a une valeur correcte pour obtenir un baud rate correct depuis la clock source
  UBR10 = 0x00;                         	//
  UMCTL0 = 0x4A;                        	// Modulation - 8 * 0,413 = 3,304 - 3 bits des 8 bits doivent avoir une valeur 1- ce qui donne 01001010 = 0x4A (voir rapport final/datasheet)
  UCTL0 &= ~SWRST;                      	// Initialize USART state machine
  IE1 |= URXIE0;                        	// Enable USART1 RX interrupt

  _EINT();           	// Entre en mode interruption
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
    UARTprintTerm("\rTouche 't' : Affichage de la temp�rature\n");
    UARTprintTerm("\rTouche 'c' : Saisi de la consigne de temp�rature\n");
    UARTprintTerm("\rTouche 'p' : Affichage de la temp�rature de consigne\n");
    UARTprintTerm("\rTouche 'l' : D�marrer la r�gulation PWM\n");
    UARTprintTerm("\rTouche 'm' : Arr�ter la r�gulation PWM\n");
    UARTprintTerm("\n\rVotre choix ? ");
}
