#include "i2c.h"
#include "pwm.h"
#include "uart.h"
#include "regulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <msp430x14x.h>
#include <__cross_studio_io.h>


int consigneInt = 20;
float consigneFloat;
char conversionStr[50];
int flagEcritureTemp = 0;
int flagRegType = 0;
float temp;

void main(void)
{
    const char * tempStr;
	int erreur;

	WDTCTL = WDTPW + WDTHOLD;                                       // Stop watchdog

    PWM_Init();

    initUART();
    UARTmsgAccueil();

    for(;;) {
        temp = readTemp(0x94);                                      // Read the temperature of the module

        if(flagEcritureTemp == 1) {                                 // If flag (set by interrupt in USART0_TX function),
            sprintf(conversionStr, "\r%f;\n", temp);                // sends continuously the temp to the PC terminal
            UARTToTerminal(conversionStr);
        }

        if(flagRegType == 0) {                                      // If flag (set by interrupt in USART0_TX function),
            REG_PidClear();                                         // reset the PID modulation and start PID modulation with the order consigne
            consigneFloat = consigneInt * 1.0;                      // Define as global, set by user by interrupt in USART0_TX function
            regulation(consigneFloat);
        } else if(flagRegType == 1) {                               // If flag (set by interrupt in USART0_TX function),
            consigneFloat = consigneInt * 1.0;                      // regulation is now by "all or nothing"
            regToR(consigneFloat);
        } else {
            PWM_Stop();                                             // Stops all regulation
        }
    }
    
}

void usart0_rx (void) __interrupt[USART0RX_VECTOR]
{
    static char recupConsigne[2];
    int z;
    static int indiceConsigne = -1;
    const char * tempStr;
    //float consigneFloat;
    int choix;

    if(indiceConsigne == 1)                                         // When setting new order, each char is considered individually
    {
        recupConsigne[indiceConsigne] = RXBUF0;
        indiceConsigne = -1;                                        // Reset, for the next order
        TXBUF0 = RXBUF0;                                            // Show the value in terminal
        consigneInt = atoi(recupConsigne);
        consigneFloat = consigneInt * 1.0;
        UARTToTerminal("\n\r                                                         \r");
        sprintf(conversionStr, "\rLa temp�rature de consigne est d�sormais  de : %f�C\n\n", consigneFloat);
                                                                    // Show the order to the user
        UARTToTerminal(conversionStr);
        UARTmsgAccueil();
    }

    if(indiceConsigne == 0)                                         // If it's the first number of the order
    {
        recupConsigne[indiceConsigne] = RXBUF0;                     // Set the number
        indiceConsigne = indiceConsigne + 1;
        TXBUF0 = RXBUF0;                                            // Show the number in terminal
    }


    if(RXBUF0 == 't')                                               // Choice for showing temp, setting the dedicated flag to the correct value
    {
        flagEcritureTemp = 1;
    }
    else if(RXBUF0 == 'l')                                          // PID regulation choice
    {
        flagRegType = 0; // Reg PID
        sprintf(conversionStr, "\rDemarrage de la regulation PID\n\n");
        UARTToTerminal(conversionStr);
        UARTmsgAccueil();
    }
    else if(RXBUF0 == 'm')                                           // All Or Nothing regulation choice
    {
        flagRegType = 1; // Reg ToR
        sprintf(conversionStr, "\rDemarrage de la regulation Tout ou Rien\n\n");
        UARTToTerminal(conversionStr);
        UARTmsgAccueil();
    }
    else if(RXBUF0 == 'n')                                          // No more regulation
    {
        flagRegType = 2; // Reg ToR
        sprintf(conversionStr, "\rToute regulation stop\n\n");
        UARTToTerminal(conversionStr);
        UARTmsgAccueil();
    }
    else if(RXBUF0 == 's')                                          // Stop showing temp
    {
        flagEcritureTemp = 0;
        UARTmsgAccueil();
    }
    else if(RXBUF0 == 'c')                                          // To place a new order
    {
        UARTToTerminal("\n\r                                                         \r");
        UARTToTerminal("\rSaissisez une valeur pour la consigne : ");
        indiceConsigne = indiceConsigne + 1;
    }
    else if(RXBUF0 == 'p')                                          // Show the order
    {
        sprintf(conversionStr, "\rLa temp�rature de consigne est actuellement  de : %f�C\n\n", consigneFloat);
        UARTToTerminal(conversionStr);
        UARTmsgAccueil();
    }
}
