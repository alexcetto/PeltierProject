#include <msp430x14x.h>
#include <math.h>
#include "i2c.h"
/*
--- Algo ---
Tous les x millisecondes, faire :
    erreur = consigne - mesure;
    somme_erreurs += erreur;
    variation_erreur = erreur - erreur_pr�c�dente;
    commande = Kp * erreur + Ki * somme_erreurs + Kd * variation_erreur;
    erreur_pr�c�dente = erreur
*/
/* Variables globales */
unsigned char marche;
unsigned char chauffage;
unsigned char TorOrPid;
float TConsigne;

float prev_erreur;
float integrale;
float dt;
float prev_temp;
float erreur;

/* Remise � 0 des valeurs du PID */
void REG_PidClear() {
	prev_erreur = 0;
	integrale = 0;
	dt = 0;
	prev_temp = 0;
}

/* Allumage de la r�gulation, et du driver Peltier */
void REG_OnOff()
{

/* Le chauffage/refroidissement ne sera d�clench� que 
si le module est en marche */
	if ( marche == 1 ) {
		P1OUT &= ~BIT1;
		P1OUT |= BIT3;
		REG_PidClear();
	}
	else {	
		P1OUT &= ~BIT3;
		P1OUT |= BIT1;
		REG_PidClear();
	}	

	return;
}

/* Application de la commande */
void REG_Consigne(int consigne)
{	
	if (abs(consigne) < 249 )
		CCR1 = 250 + consigne;
	else
		if ( consigne > 0 )		
			CCR1 = 490;
		else
			CCR1 = 10;

}


void regulation(float consigne){
    char tmpStr[10];
    float CurrentTemp;
    float Kp, Ki, Kd, derive;

    int sortie;
  
    CurrentTemp = readTmp(0x94);

// M�thode heuristique de ZIEGLER NICHOLS
			
    Kp = 27.520;
    Ki = 18.25;
    Kd = 1.131;
    dt = 0.301; //3915.5;

    erreur = consigne - CurrentTemp;
    integrale = integrale + erreur*dt;
    derive = (prev_erreur - erreur);

    // Test division par z�ro
    if ( fabs(derive) > 10e-4 ) 
            derive = derive/dt;
    else
            derive = 0.0;

    // Evaluation de la consigne via m�thode KN
    sortie = ( (Kp*erreur) + (Ki*integrale) + (Kd*derive) );
    
    // PWM INIT A REMETTRE APRES
    REG_OnOff();
    REG_Consigne(sortie);
	
    prev_erreur = erreur;

    return;

}

