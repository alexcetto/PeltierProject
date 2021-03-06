#ifndef __I2C__
#define __I2C__

float readTmp(unsigned char addr);
void I2C_StartCond(void);
void I2C_Stop(void);
void send_ack(void);
unsigned char receive_ack(void);
void sendAddr(unsigned char octet);
unsigned char readBite(void);
unsigned char getByte(void);
void I2C_Write(void);
void I2C_Read(void);

#endif
