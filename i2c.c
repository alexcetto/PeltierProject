#include <stdlib.h>
#include <msp430x14x.h>
#include "i2c.h"
#include "uart.h"


/* Temperature reading
 * @brief   : Function that communicate with the tmp101 sensor in order to obtain temp in its registers.
 *
 * @param   : takes the address of the sensor TMP101
 *
 * @return  : temperature read from the sensor in simple precision
 */
float readTemp(unsigned char addr) {
    int temp = 0;
    unsigned char bytes[] = {0x00, 0x00};              // Besoin de 16 bits, le tmp101 envoie 12 bits de données
    char signe;
    int length, tmp;

    I2C_Write();                                        // Write mode activator, the master msp430 send the slave address

    I2C_StartCond();                                    // Start condition send by master, takes controle of the i2c bus

    addr |= 0x01;                                       // Slave address, adding 1 means we want to read
    sendAddr(addr);

    I2C_Read();                                         // Read mode enabled to get ACK confirmation from slave

    if (receive_ack() == 0)                             // Ff ACK ok, we read temp. Stored in bytes
    {
        bytes[0] = getByte();                           // First data

        I2C_Write();                                    // Write mode to confirm to slave good reception (ACK sent)
        send_ack();

        I2C_Read();                                     // Back to read mode for second byte
        bytes[1] = getByte();

        I2C_Write();
        send_ack();

        I2C_Stop();                                     // Stopping communication in i2c bus


        /** Formating temp to send it in float form **/
        tmp = 0;
        temp = 1.0;
        signe = 0x80 & bytes[0];
        bytes[0] &= ~0x80;
        tmp |= (bytes[1] >> 4) | ((int) bytes[0] << 4);
        tmp >>= SHIFT_12;
        if (signe) tmp |= 0xF800;
        temp = tmp;
        temp = temp * INC_12;
        temp /= 10;
    }
    else {
        temp = 1;                                       // Error code
    }

    return temp;
}

/*
 * @ brief : i2c procedure to take control of the bus
 * SCL = BIT6
 * SDA = BIT5
 */
void I2C_StartCond(void) {
    P1OUT |= BIT6;                                      // SDA & SCL to 1 to take control of the bus
    P1OUT |= BIT5;

    P1OUT |= BIT6;                                      // SDA = 0, SCL 1 then SCL O to signal the master is taking control
    P1OUT &= ~BIT5;
    P1OUT &= ~BIT6;
}


/*
 * @ brief : i2c to idle mode
 * SCL = BIT6
 * SDA = BIT5
 */
void I2C_Stop(void) {
    P1OUT |= BIT6;
    P1OUT |= BIT5;
}

/*
 * @ brief : send ACK bit to signal the end of transmission
 * SCL = BIT6
 * SDA = BIT5
 */
void send_ack(void) {
    P1OUT &= ~BIT5;
    P1OUT |= BIT6;
    P1OUT &= ~BIT6;
}

/*
 * @brief  : check if the slave recieved the whole communication
 * The clock is maintained, else the slave stops functionning
 * @return : the ACK bit
 */
unsigned char receive_ack(void) {
    unsigned char acknowledge;
    P1OUT |= BIT6;                                      // Keep the clock going, SCL to 1
    acknowledge = (P1IN & BIT5);                        // Get on SDA line the code for ACK, else error of communication
    P1OUT &= ~BIT6;                                     // SCL to 0
    return acknowledge;
}

/*
 * @brief : Sends on SDA the address of the slave we want to speak to.
 *
 * Algorithm :
 *      After the starting condition
 *      for (MSB to LSB of the address)
 *           we send on SDA the MSB of the address addr and we validate by turning SCL to 1
 */
void sendAddr(unsigned char addr) {
    unsigned int BitCnt;

    for (BitCnt = 0; BitCnt < 8; BitCnt = BitCnt + 1) {
        P1OUT &= ~BIT6;                                 // When SCL back to 0, we send the bit on SDA
        if ((addr << BitCnt) & 0x80)                    // This sends bit after bit, the 0x80 mask let us send 1 or 0, the BitCnt does a shift
        {
            P1OUT |= BIT5;
        }
        else {
            P1OUT &= ~BIT5;
        }
        P1OUT |= BIT6;                                  // Validate the data on SDA by sending 1 on SDA
    }
    P1OUT &= ~BIT6;                                     // SDA et SCL to 0, back to idle mode
    P1OUT &= ~BIT5;
}

/*
 * @Brief : read data on SDA
 * @return : a byte with the bit of data
 */
unsigned char readBit(void) {
    unsigned char bit;
    P1OUT |= BIT6;                                      // SCL to 1
    bit = (P1IN & BIT5);                                // get bit on SDA line
    P1OUT &= ~BIT6;                                     // SCL to 0
    return bit;
}

/*
 * @brief : get the data sent by the slave
 * @return : the byte of data
 */
unsigned char getByte(void) {
    unsigned char bit;
    unsigned char byte = 0;
    int i;

    for (i = 7; i >= 0; i--)                            // The slave sends 8 bits of data
    {
        bit = readBit();
        if (bit)                                        // byte is initialized at 0 at the beginning of the function
        {
            byte |= (1 << i);                           // So we just have to write 1 where it is needed, the rest is 0
        }
    }
    return byte;
}

/*
 * i2c in write mode, the master will communicate with the slave to send the address
 */
void I2C_Write(void) {
    P1DIR |= BIT6;                                      // SCL = 1
    P1DIR |= BIT5;                                      // SDA = 1
}

//On passe en mode lecture, l'esclave va r�pondre en envoyant ACK et la temp�rature
/*
 * @brief : write mode, the slave sends ACK and temperature
 */
void I2C_Read(void) {
    P1DIR |= BIT6;                                  // SCL = 1, clock
    P1DIR &= ~BIT5;                                 // SDA = 0, listenning
}
