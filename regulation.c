#include <msp430x14x.h>
/*
void regulation(float consigne){
  int err;
  int samp;
  int sum_err;
  int var_err;
  int last_err;
  int cmd;

  while(1){
    err = consigne - samp;
    sum_err += err;
    var_err = err - last_err;
    cmd = err * 0.6;
  }
}
*/

void regulation(float cons){
    char tmpStr[10];
    float CurrentTemp;
    float Cp, Ci, Cd, dt, erreur, derive, integrale, prev_erreur;

    int sortie;
  
    CurrentTemp = readTemp(0x94);

// ZIEGLER NICHOLS
// FACTEUR PROPORTIONNEL			
    Cp = 27.520;

// FACTEUR INTEGRAL
    Ci = 18.25;

// FACTEUR DERIVE
    Cd = 1.131;
    

    dt = 0.301; //3915.5;
    erreur = cons - CurrentTemp;
    integrale = integrale + erreur*dt;
    derive = (prev_erreur - erreur);

// NE PAS EFFECTUER DE DIVISION D'UN NOMBRE TROP PETIT
    if ( fabs(derive) > 10e-4 ) 
            derive = derive/dt;
    else
            derive = 0.0;

// CALCUL DE LA CONSIGNE
    sortie = ( (Cp*erreur) + (Ci*integrale) + (Cd*derive) );//*0.3219;
    
// TRANSMISSION CONSIGNE PID
    REG_OnOff();
    REG_Consigne(sortie);

// SAUVEGARDE DE LA VALEUR AVANT LE NOUVEL APPEL DE LA BOUCLE PID	
    prev_erreur = erreur;

    return;

}