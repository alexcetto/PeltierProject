#include <stdlib.h>
#include <msp430x14x.h>
#include "i2c.h"
#include "uart.h"

#define INC_12    0.625
#define SHIFT_12  0

float readTmp(unsigned char addr)
{
    int temp = 0;
    unsigned char bytes[] = {0x00, 0x00};
    char signe;
    int length;
    int tmp;
    int j;
    
    //On passe en mode ecriture, le maitre va communiquer avec l'esclave pour lui envoyer l'adresse
    I2C_Write();

    //prise de controle du bus par le maitre (condition de départ) 
    I2C_StartCond();

    //adresse de l'esclave avec lequel on veut communiquer transmis sur le bus, on précise, que l'on veut lire en mettant le bit de poids faible à 1.
    addr |= 0x01;
    sendAddr(addr);


    // Passez en mode lecture pour recevoir ACK ainsi que les données
    I2C_Read();

    if (receive_ack() == 0)  //si on a recu l'ACK, on récupére le contenu des registres 1 & 2 du registre de température
    {
        bytes[0] = getByte();

        // Passez en mode ecriture, pour donner l'ACK qu'on a bien lu la donnée
        I2C_Write();

        send_ack();

        // repasse en mode lecture pour recevoir le prochain octet
        I2C_Read();

        bytes[1] = getByte();

        // Passez en mode ecriture, pour donner l'ACK qu'on a bien lu la donnée
        I2C_Write();
        send_ack();

        I2C_Stop();

        tmp = 0;
	temp = 1.0;
	signe = 0x80 & bytes[0];
	bytes[0] &= ~0x80;
	tmp |= (bytes[1] >> 4) | ( (int)bytes[0] << 4  ); 			
	tmp >>= SHIFT_12;	
	if ( signe ) tmp |= 0xF800;	
	temp = tmp;
	temp = temp * INC_12;
        //temp /= 10;
    }
    else
    {
        temp = 1;  //erreur dans la lecture, je prend 1, car dans notre appli on est pas cencé descendre aussi bas en température
    }
    
    return temp;
}

void I2C_StartCond(void)
{
    //prise de controle du bus par le maitre (condition de départ) SCL1 et SDA1
    P1OUT |= BIT6;
    P1OUT |= BIT5;
    //SDA passe à 0, alors que SCL est toujours à 1, puis SCL passe à 0
    P1OUT |= BIT6;
    P1OUT &= ~BIT5;
    P1OUT &= ~BIT6;
}

void I2C_Stop(void)
{
    P1OUT |= BIT6;
    P1OUT |= BIT5;
}

void send_ack(void)
{
    P1OUT &= ~BIT5;
    P1OUT |= BIT6;
    P1OUT &= ~BIT6;
}

unsigned char receive_ack(void)
{
    unsigned char acknowledge;
    P1OUT |= BIT6;  //continue de faire fonctionner la clock
    acknowledge = (P1IN  &BIT5);   //récupére ce que l'esclave a envoyé sur SDA, si c'est un 0, renvoie de l'ACK, sinon erreur
    P1OUT &= ~BIT6;  //continue de faire fonctionner la clock
    return acknowledge;
}

void sendAddr(unsigned char octet)
{/*principe : Après la conditiond de départ imposée, on met sur SDA, le bit de poids fort à transmettre, pour valider la donnée, on passe SCL à 1, puis on recommence*/
    	unsigned int BitCnt;
 
    	for(BitCnt=0;BitCnt < 8;BitCnt = BitCnt + 1)
    	{
        	P1OUT &= ~BIT6;  //lorsque SCL reviens à 0, on peut transmettre notre bit sur SDA
        	if((octet << BitCnt)&0x80)  //regarde le bit de poids fort,s'il vaut 1, envoie1 sinon 0, a chaque tour de boucle on décale
        	{
            	P1OUT |= BIT5;
        	}
        	else
        	{
            	P1OUT &= ~BIT5;
        	}
        	P1OUT |= BIT6; // valide la donné sur SDA, en mettant SCL à 1
   	}
    	P1OUT &= ~BIT6;  //a la fin de l'envoie SCL et SDA repasse à 0
        P1OUT &= ~BIT5;
}

unsigned char readBite(void)
{
    unsigned char bit;
    P1OUT |= BIT6;
    bit = (P1IN  &BIT5);   //récupere la valeur du bit sur la ligne SDA
    P1OUT &= ~BIT6;
    return bit;
}

unsigned char getByte(void)
{
    unsigned char bit;
    unsigned char byte = 0;
    int i;

    	for(i = 7;i >=0;i--)  //remplis l'octet lu
    	{
        	bit = readBite();  //lis le bit sur SDA
        	if(bit)  //si c'est un 1, on l'écrit a l'emplacement, le 0 s'en déduit automatiquement, la ou on a pas mis de 1
        	{
                  byte |= (1 << i);
        	}
   	}
    	return byte;  //retourne l'octet lu
}

//On passe en mode ecriture, le maitre va communiquer avec l'esclave pour lui envoyer l'adresse
void I2C_Write(void)
{
    P1DIR |= BIT6; //scl a 1 en sortie
    P1DIR |= BIT5; // sda a 1 en sortie
}

//On passe en mode lecture, l'esclave va répondre en envoyant ACK et la température
void I2C_Read(void)
{
    P1DIR |= BIT6; //SCL en sortie (clock)
    P1DIR &= ~BIT5;   //SDA en entrée, va lire ce que l'esclave envoie
}
