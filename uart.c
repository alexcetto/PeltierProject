#include <msp430x14x.h>
#include "regulation.h"

void initUART(void){
  
    P3SEL |= 0x30;                        	// Select pin input/output of the UART, PIN3.4 and PIN3.5 = USART0 TXD/RXD 
    ME1 |= UTXE0 + URXE0;                 	// Activate USART0 TXD/RXD
    UCTL0 |= CHAR;                        	// Character format : 8 bits(char)
    UTCTL0 |= SSEL0;                      	// UCLK = ACLK 32.768kHz ,  UART Module with specific clock frequency
    UBR00 = 0x03;                         	// 32k/9600 = 3.41 Set the correct baud rate of the source
    UBR10 = 0x00;                         	//
    UMCTL0 = 0x4A;                        	// Modulation - 8 * 0,413 = 3,304 - 3 bits of the 8 bits have to be 1 so 01001010 = 0x4A
    UCTL0 &= ~SWRST;                      	// Initialize USART state machine
    IE1 |= URXIE0;                        	// Enable USART1 RX interrupt
    
    _EINT();           	                    // Enter interrupt mode
    return;
}

/*
 * @brief : print the str parameter in the PC terminal, accessible via COM port (putty)
 */
void UARTToTerminal(const char * str)
{
    int i;
    
    while(*str != '\0' )                     // Until end of str
    {
        while(!(IFG1& UTXIFG0));             // Checking if the USART0 TX buffer is ready
        TXBUF0 = *str++;                     // Sending the string
    }
    
    for(i = 0; i < 10000; i = i+1) ;         // Waiting time
}

/*
 * Main menu
 */
void UARTmsgAccueil(void)
{
    UARTToTerminal("\n\r=======================================================\n");
    UARTToTerminal("\r\t\t\t===  Menu  ===\n");
    UARTToTerminal("\r't' : Affichage de la temperature\n");
    UARTToTerminal("\r'c' : Saisie de la consigne de temperature\n");
    UARTToTerminal("\r'p' : Affichage de la temperature de consigne\n");
    UARTToTerminal("\r'l' : Regulation PID\n");
    UARTToTerminal("\r'm' : Regulation ToR\n");
    UARTToTerminal("\r'n' : Stopper regulation\n");
    UARTToTerminal("\n\rVotre choix ? \n\r");
}