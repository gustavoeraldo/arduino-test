#ifndef I2C_LMI_H
#define I2C_LMI_H
#define TIME_DELAY 30
#include "Arduino.h"
#include "pins_arduino.h"

class I2C_LMI {

public:
	int SDA, SCL, MY_ADDR, D_ADDR;

	I2C_LMI();
	void I2CBegin(int sda, int scl);
	bool I2CStartTrans(int addr);
	void I2CStopTrans();
	void I2CStartWait();
	void I2CStopWait();
	bool enderecaDestino(unsigned int addr);
	bool confirmaEndereco();
	bool enviaByte(char c);
	char recebeByte();
	void escreveBit(bool bit);
	bool leBit();
	//bool leBitRec();
};

#endif I2C_LMI_H 