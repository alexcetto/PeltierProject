#include <msp430x14x.h>
#include <math.h>
#include "i2c.h"
#include <__cross_studio_io.h>
/*
--- Algo ---
Tous les x millisecondes, faire :
    erreur = consigne - mesure;
    somme_erreurs += erreur;
    variation_erreur = erreur - erreur_précédente;
    commande = Kp * erreur + Ki * somme_erreurs + Kd * variation_erreur;
    erreur_précédente = erreur
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

/* Remise à 0 des valeurs du PID */
void REG_PidClear() {
	prev_erreur = 0;
	integrale = 0;
	dt = 0;
	prev_temp = 0;
}

/* Allumage de la régulation, et du driver Peltier */
void REG_OnOff()
{

/* Le chauffage/refroidissement ne sera déclenché que 
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
	if (abs(consigne) < 499 )
		CCR1 = 500 + consigne;
	else
		if ( consigne > 0 )		
			CCR1 = 990;
		else
			CCR1 = 10;

}


void regulation(float consigne){
    char tmpStr[10];
    float CurrentTemp;
    float Kp, Ki, Kd, derive;

    int sortie;
  
    CurrentTemp = readTmp(0x94);

// Méthode heuristique de ZIEGLER NICHOLS
			
    Kp = 60;
    Ki = 240;
    Kd = 0.5;
    dt = 0.301; //3915.5;
  
    //debug_printf("%f\n", consigne);

    erreur = consigne - CurrentTemp;
    integrale = integrale + erreur*dt;
    derive = (prev_erreur - erreur);

    //debug_printf("erreur : %f\nintegrale : %f\nderive : %f\n", erreur, integrale, derive);
    // Test division par zéro
    if ( fabs(derive) > 10e-4 ) 
            derive = derive/dt;
    else
            derive = 0.0;

    // Evaluation de la consigne via méthode KN
    sortie = ( (Kp*erreur) + (Ki*integrale) + (Kd*derive) );


    debug_printf("%d\n", sortie);
    
    // PWM INIT A REMETTRE APRES
    //REG_OnOff();
    REG_PidClear();
    REG_Consigne(sortie);

    //debug_printf("Sortie : %d\n", sortie);
	
    prev_erreur = erreur;

    return;

}

