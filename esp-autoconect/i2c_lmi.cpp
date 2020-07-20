#include "i2c_lmi.h"

I2C_LMI::I2C_LMI(void) {
	this->MY_ADDR = 125;
	this->SCL = 2;
	this->SDA = 0;
}


void I2C_LMI::I2CBegin(int sda, int scl) {
	this->SDA = sda;
	this->SCL = scl;
}


bool I2C_LMI::I2CStartTrans(int addr) {
	this->D_ADDR = addr;
	pinMode(SDA, OUTPUT);
	delayMicroseconds(TIME_DELAY);
	pinMode(SCL, OUTPUT);
	delayMicroseconds(TIME_DELAY);

	return this->enderecaDestino(D_ADDR<<1);
}


void I2C_LMI::I2CStopTrans() {
	pinMode(SDA, OUTPUT);
	delayMicroseconds(TIME_DELAY);
	pinMode(SCL, INPUT);
	delayMicroseconds(TIME_DELAY);
	pinMode(SDA, INPUT);
	delayMicroseconds(TIME_DELAY);
}

bool I2C_LMI::enderecaDestino(unsigned int addr) {
 	return (enviaByte(addr)); 
 }

bool I2C_LMI::confirmaEndereco() {
	byte end = recebeByte();
	bool RW = end && 0x01;
	end >>= end;
	
	if(end == MY_ADDR) return true;
		return false; 
}


bool I2C_LMI::enviaByte(char c){
 	for (int bit = 0; bit < 8; bit++) {
		escreveBit(c & 0x80);
		c <<= 1;
	}
 	return !leBit();//NACK/ACK 
}


char I2C_LMI::recebeByte() {
	pinMode(SDA, INPUT);
	char byte = 0;
	char bit;
 	
 	for (bit = 0; bit < 8; bit++) byte = (byte << 1) | leBit();
 		escreveBit(0);
 	return byte;
 }


void I2C_LMI::escreveBit(bool bit) {
	if(bit) pinMode(SDA, INPUT);
	else pinMode(SDA, OUTPUT);
	
	delayMicroseconds(TIME_DELAY);
	pinMode(SCL, INPUT);
	
	while(digitalRead(SCL) == LOW);// Clock stretching
	
	delayMicroseconds(TIME_DELAY);
	pinMode(SCL, OUTPUT);
	delayMicroseconds(TIME_DELAY);
}


bool I2C_LMI::leBit() {
	pinMode(SCL, INPUT);
	
	while(digitalRead(SCL) == LOW);// Clock stretching
	
	delayMicroseconds(TIME_DELAY);
	bool bit = digitalRead(SDA);
	pinMode(SCL, OUTPUT);
	delayMicroseconds(TIME_DELAY);
	
	return bit;
}
