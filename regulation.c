#include <msp430x14x.h>
#include <math.h>
#include "i2c.h"
#include <__cross_studio_io.h>

                                                                    // Globals
unsigned char marche;
unsigned char chauffage;
unsigned char TorOrPid;
float TConsigne;

float prev_erreur;
float integrale;
float dt;
float prev_temp;
float erreur;

/* Reset to 0 the PID values */
void REG_PidClear() {
	prev_erreur = 0;
	integrale = 0;
	dt = 0;
	prev_temp = 0;
}

/*
 * @brief : regulation by All or nothing method.
 */
void regToR(float consigne)
{
  float CurrentTemp;

  CurrentTemp = readTemp(0x94);
  if(CurrentTemp < consigne)
    CCR1 = 700;
  else if(CurrentTemp > consigne)
    CCR1 = 200;
  else
    CCR1 = 500;

  return;
}


/*
 * @ brief : automatic regulation by PID
--- Algo ---
Every x ms do
    erreur = consigne - mesure;
    somme_erreurs += erreur;
    variation_erreur = erreur - erreur_prec;
    commande = Kp * erreur + Ki * somme_erreurs + Kd * variation_erreur;
    erreur_prec = erreur
*/
void regulation(float consigne){
    char tmpStr[10];
    float CurrentTemp;
    float Kp, Ki, Kd, derive;
    int sortie;
  
    CurrentTemp = readTemp(0x94);
                                                                    // Heuristic method of ZIEGLER NICHOLS
    Kp = 35;
    Ki = 1.2;
    Kd = 0.45;
    dt = 0.301;

    erreur = consigne - CurrentTemp;
    integrale = integrale + erreur*dt;
    derive = (prev_erreur - erreur);


    derive = (fabs(derive) > 10e-4) ? derive/dt : 0.0;

    sortie = ( (Kp*erreur) + (Ki*integrale) + (Kd*derive) );

    REG_PidClear();
    REG_Consigne(sortie);
    prev_erreur = erreur;

    return;
}

