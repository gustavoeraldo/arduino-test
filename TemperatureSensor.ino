#include <LiquidCrystal.h>

LiquidCrystal lcd(2,3,4,42,40,38,36,34,32,30,28);
double T,T0,v,v0,p1,p2,p3,p4,q1,q2,q3;

void setup() {
  Serial.begin(9600);
  pinMode(A15,INPUT);
  
  T0= -8.7935962;
  v0= -3.4489914*pow(10,-1);
  p1= 2.5678719*10;
  p2= -4.9887904*0.1;
  p3= -4.4705222*0.1;
  p4= -4.4869203*0.01;
  q1= 2.3893439*0.0001;
  q2= -2.0397750*0.01;
  q3= -1.8424107*0.001;
  v = 2.203;
  
  lcd.begin(16,2);
  lcd.print("Gustavo Eraldo");
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  lcd.print(v); 
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
  T = T0 + ((v-v0)*(p1 + (v-v0)*(p2+(v-v0)*(p3+p4*(v-v0)))))/ (1 +(v-v0)*(q1 + (v-v0*(q2 +q2*(v-v0)))));
  v += 0.01;
  Serial.print("Temperatura :");
  Serial.println(T);
}
