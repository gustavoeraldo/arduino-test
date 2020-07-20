#include "max6675.h" //INCLUSÃO DE BIBLIOTECA
#include <LiquidCrystal.h>

LiquidCrystal lcd(2,3,4,42,40,38,36,34,32,30,28);
int ktcSO = 8; //PINO DIGITAL (SO)
int ktcCS = 9; //PINO DIGITAL (CS)
int ktcCLK = 10; //PINO DIGITAL (CLK / SCK)

MAX6675 ktc(ktcCLK, ktcCS, ktcSO); //CRIA UMA INSTÂNCIA UTILIZANDO OS PINOS (CLK, CS, SO)
  
void setup(){
  Serial.begin(9600); //INICIALIZA A SERIAL
  lcd.begin(16,2);
  pinMode(A15,INPUT);
  delay(500); //INTERVALO DE 500 MILISSEGUNDOS
}

void loop(){
   lcd.setCursor(0,1);
   lcd.clear();
   lcd.print("Temp: ");
   lcd.print(ktc.readCelsius()); 
   Serial.print("Temp: "); 
   Serial.print(ktc.readCelsius()); 
   Serial.println("*C");
   delay(500); 
}
