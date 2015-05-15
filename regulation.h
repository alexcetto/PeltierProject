#ifndef __REGULATION__
#define __REGULATION__

void REG_Init();
void REG_Tor(float);
void REG_Pid(float);
void REG_PidClear(void);

void regToR(float consigne);
void regulation(float consigne);

extern float prev_temp;
extern unsigned char marche;
extern unsigned char TorOrPid;
extern unsigned char chauffage;
extern float TConsigne;
extern float erreur;
extern float integrale;
extern float dt;

#endif
