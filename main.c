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
int e = 0;
unsigned short retour;
char conversionStr[50];
int flagEcritureTemp = 0;
int flagRegType = 0;
float temp;

void changeClock(void)
{
  volatile unsigned int i;                
  /*
    OverClock en changeant de timer
  */  
  BCSCTL1 = 0xC0;                       //XT2 OFF, LFXT1 high frequency
  do 
  {
      IFG1 &= ~OFIFG;                   // Clear OSCFault flag
      for (i = 0xFF; i > 0; i--);       // Time for flag to set
  }
  while ((IFG1 & OFIFG) != 0);          // OSCFault flag still set?                
  BCSCTL2 = 0xC8;                       //MCLK & SMCLK = LFXT1

}

void main(void)
{
        
        const char * tempStr;
	int erreur;

	WDTCTL = WDTPW + WDTHOLD;     // Stop watchdog
        //changeClock();

        PWM_Init();

        initUART();
        UARTmsgAccueil();
        //regulation(consigneInt);
        for(;;)
        {
            temp = readTemp(0x94);   //lit la temp�rature du capteur TMP101, avec l'adresse de l'esclave

            if(flagEcritureTemp == 1)
            {
            //\033[2J\033[1;1H
              //sprintf(conversionStr, "Temperature : %f ('s' => Stop)\r", temp);
              sprintf(conversionStr, "\r%f;\n", temp);
              UARTprintTerm(conversionStr);
              //UARTprintTerm("\n");
            }
            if(flagRegType == 0)
            {
              REG_PidClear();
              consigneFloat = consigneInt * 1.0;
              regulation(consigneFloat);
            }
            else if(flagRegType == 1)
            {
              consigneFloat = consigneInt * 1.0;
              regToR(consigneFloat);
            }
            else
            {
              PWM_Stop();
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

  if(indiceConsigne == 1)  //lors de la saisie d'une nouvelle consigne, il y a une interruption par appuie sur une touche pour la saisie, donc on traite separement chaque chiffre
  {
        recupConsigne[indiceConsigne] = RXBUF0;  //conserve dans un tableau la valeur saisie
        indiceConsigne = -1;  //re-initialise la prise d'une consigne, dans le cas plus tard d'une nouvelle saisie de consigne  
        TXBUF0 = RXBUF0; //r�affiche valeur saisi par l'utilisateur
        consigneInt = atoi(recupConsigne);  //transforme la valeur en char en int
        consigneFloat = consigneInt * 1.0;
        UARTprintTerm("\n\r                                                         \r");
        sprintf(conversionStr, "\rLa temp�rature de consigne est d�sormais  de : %f�C\n\n", consigneFloat);  //reaffiche la nouvelle consigne que l'utilisateur vient de saisir
        
        UARTprintTerm(conversionStr);
        UARTmsgAccueil();  //reffiche menu
  }
  if(indiceConsigne == 0)  //si c'est la saisie du premier chiffre de la nouvelle consigne
  {
        recupConsigne[indiceConsigne] = RXBUF0;  //conserve le chiffre
        indiceConsigne = indiceConsigne + 1;   //incrmeente indice tableau
        TXBUF0 = RXBUF0;  //reaffiche a l'utilisateur la valeur saisi
  }


  if(RXBUF0 == 't')   //si c'est l'affichage de la temperature, on active le flag de l'affichage dans la fonction main
  {
      flagEcritureTemp = 1;
  }
  else if(RXBUF0 == 'l') 
  {   
      flagRegType = 0; // Reg PID
      sprintf(conversionStr, "\rDemarrage de la regulation PID\n\n");
      UARTprintTerm(conversionStr);
      UARTmsgAccueil();
  }
   else if(RXBUF0 == 'm')
  {
      flagRegType = 1; // Reg ToR
      sprintf(conversionStr, "\rDemarrage de la regulation Tout ou Rien\n\n");
      UARTprintTerm(conversionStr);
      UARTmsgAccueil();
  }
  else if(RXBUF0 == 'n')
  {
      flagRegType = 2; // Reg ToR
      sprintf(conversionStr, "\rToute regulation stop\n\n");
      UARTprintTerm(conversionStr);
      UARTmsgAccueil();
  }
  else if(RXBUF0 == 's') // Arret affichage temp
  {
      flagEcritureTemp = 0;
      UARTmsgAccueil();
  }
  else if(RXBUF0 == 'c')  //si c'est la prise d'une nouvelle consigne
  {
        UARTprintTerm("\n\r                                                         \r");
        UARTprintTerm("\rSaissisez une valeur pour la consigne : ");   //on affiche la demande de saisi de la consigne, et incremente indice tableau pour conserver la valeur saisi par l'utilisateur
        indiceConsigne = indiceConsigne + 1;
  }
  else if(RXBUF0 == 'p')   //si c'est l'affichage de la consigne, on l'affiche
  {
          sprintf(conversionStr, "\rLa temp�rature de consigne est actuellement  de : %f�C\n\n", consigneFloat);
          UARTprintTerm(conversionStr);     
          UARTmsgAccueil();
  }
}
