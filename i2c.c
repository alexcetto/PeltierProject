#include <stdlib.h>
#include <msp430x14x.h>
#include "i2c.h"
#include "uart.h"

#define INC_12    0.625
#define SHIFT_12  0

/* Lecture de la température
 * @param:  prend l'adresse du TMP101
 *
 * @return: retourne la température en simple précision
 */
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

    //prise de controle du bus par le maitre (condition de d�part) 
    I2C_StartCond();

    //adresse de l'esclave avec lequel on veut communiquer transmis sur le bus, on pr�cise, que l'on veut lire en mettant le bit de poids faible � 1.
    addr |= 0x01;
    sendAddr(addr);


    // Passez en mode lecture pour recevoir ACK ainsi que les donn�es
    I2C_Read();

    if (receive_ack() == 0)  //si on a recu l'ACK, on r�cup�re le contenu des registres 1 & 2 du registre de temp�rature
    {
        bytes[0] = getByte();

        // Passez en mode ecriture, pour donner l'ACK qu'on a bien lu la donn�e
        I2C_Write();

        send_ack();

        // repasse en mode lecture pour recevoir le prochain octet
        I2C_Read();

        bytes[1] = getByte();

        // Passez en mode ecriture, pour donner l'ACK qu'on a bien lu la donn�e
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
        temp /= 10;
    }
    else
    {
        temp = 1;  //erreur dans la lecture, je prend 1, car dans notre appli on est pas cenc� descendre aussi bas en temp�rature
    }
    
    return temp;
}

void I2C_StartCond(void)
{
    //prise de controle du bus par le maitre (condition de d�part) SCL1 et SDA1
    P1OUT |= BIT6;
    P1OUT |= BIT5;
    //SDA passe � 0, alors que SCL est toujours � 1, puis SCL passe � 0
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
    acknowledge = (P1IN  &BIT5);   //r�cup�re ce que l'esclave a envoy� sur SDA, si c'est un 0, renvoie de l'ACK, sinon erreur
    P1OUT &= ~BIT6;  //continue de faire fonctionner la clock
    return acknowledge;
}

void sendAddr(unsigned char octet)
{/*principe : Apr�s la conditiond de d�part impos�e, on met sur SDA, le bit de poids fort � transmettre, pour valider la donn�e, on passe SCL � 1, puis on recommence*/
    	unsigned int BitCnt;
 
    	for(BitCnt=0;BitCnt < 8;BitCnt = BitCnt + 1)
    	{
        	P1OUT &= ~BIT6;  //lorsque SCL reviens � 0, on peut transmettre notre bit sur SDA
        	if((octet << BitCnt)&0x80)  //regarde le bit de poids fort,s'il vaut 1, envoie1 sinon 0, a chaque tour de boucle on d�cale
        	{
            	P1OUT |= BIT5;
        	}
        	else
        	{
            	P1OUT &= ~BIT5;
        	}
        	P1OUT |= BIT6; // valide la donn� sur SDA, en mettant SCL � 1
   	}
    	P1OUT &= ~BIT6;  //a la fin de l'envoie SCL et SDA repasse � 0
        P1OUT &= ~BIT5;
}

unsigned char readBite(void)
{
    unsigned char bit;
    P1OUT |= BIT6;
    bit = (P1IN  &BIT5);   //r�cupere la valeur du bit sur la ligne SDA
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
        	if(bit)  //si c'est un 1, on l'�crit a l'emplacement, le 0 s'en d�duit automatiquement, la ou on a pas mis de 1
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

//On passe en mode lecture, l'esclave va r�pondre en envoyant ACK et la temp�rature
void I2C_Read(void)
{
    P1DIR |= BIT6; //SCL en sortie (clock)
    P1DIR &= ~BIT5;   //SDA en entr�e, va lire ce que l'esclave envoie
}
