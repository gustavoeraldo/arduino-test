#include <LiquidCrystal.h>

LiquidCrystal lcd(2,3,4,42,40,38,36,34,32,30,28);
double T,T0,v,v0,p1,p2,p3,p4,q1,q2,q3;



void setup() {
  Serial.begin(9600);
  pinMode(A15,INPUT);

  T =0;
  /*T0 = 3.1018976E+02;  
  v0 = 1.2631386E+01;
  p1 = 2.4061949E+01;
  p2 = 4.0158622E+00;
  p3 = 2.6853917E-01;
  p4 = -9.7188544E-03;
  q1 = 1.6995872E-01;
  q2 = 1.1413069E-02;
  q3 = -3.9275155E-04;
  v = 16397/1000;*/
  
  T0= -8.7935962E+00;
  v0= -3.4489914E-01;
  p1= 2.5678719E+01;
  p2= -4.9887904E-01;
  p3= -4.4705222E-01;
  p4= -4.4869203E-02;
  q1= 2.3893439E-04;
  q2= -2.0397750E-02;
  q3= -1.8424107E-03;
  v = 3041/1000;
  
  
  lcd.begin(16,2);
  //lcd.print("Gustavo Eraldo");
  //lcd.setCursor(0,1);
  //lcd.print("Temp: ");
  //lcd.print(v); 
  //lcd.print("O FODA");
}

void loop() {
  //Serial.print("Entrada analogica: ");
  //Serial.println(analogRead(A15));
  
  if(T > -50){
    lcd.setCursor(0,1);
    lcd.clear();
    lcd.print("Temp: ");
    lcd.print(T);
    lcd.print(" C");
  }
  delay(500);
  T = (v-v0)*(p1 + (v-v0)*(p2+ (v-v0)*(p3+p4*(v-v0) )));
  T = T/(1+(v-v0)*(q1 + (v-v0)*(q2 +q3*(v-v0)) ) );
  T +=T0;
  //v += 0.01;
  Serial.print("Temperatura :");
  Serial.println(T);
}
