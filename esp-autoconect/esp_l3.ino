#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <string.h>
#include "i2c_lmi.h"

#define TRIGGER 15 // TRIGGER 2, USE ESSA CONFIGURAÇÃO PARA ESP 01 (ESP COM 8 PINOS)
#define TIMEOUT 1000
#define TAM_EEPROM 256
#define ENDE_SSID 0
#define ENDE_PASS 64

using namespace std;

void copiaCString(char *ptr, char *str);

char *ssid = new char[64];
char *password = new char[64];
const char *factorySsid = "ESPAP";
const char *factoryPassword = "123456789";

//DEFINIÇÃO DE IP FIXO PARA O NODEMCU
IPAddress ip(192,168,0,175); //COLOQUE UMA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR. EX: 192.168.1.110 **** ISSO VARIA, NO
//MEU CASO É: 192.168.0.175
IPAddress gateway(192,168,0,1); //GATEWAY DE CONEXÃO (ALTERE PARA O GATEWAY DO SEU ROTEADOR)
IPAddress subnet(255,255,255,0); //MASCARA DE REDE

boolean AP = false;
String cmdHexa = "";
int tamSsid = 0;
int tamPass = 0;

MDNSResponder mdns;
ESP8266WebServer server(80);

I2C_LMI COM; // ***************** FOI COMENTADO ********************

//float getCorrente(void); // ***************** FOI COMENTADO ***********
void enviarHexadecimal(String, int);
String leStringSerial();
String convertHexa(char);

void handleRoot() {
  server.send(200, "text/html", "<h1>Voc&ecirc est&aacute conectado!</h1>");
}


void handleQualquer(){

  
  unsigned long tempo;
  float corrente = 0.0;
  char caractere;
  String comando = server.uri();
  String cmdRecebido = "";
  //while (Serial.available()){char lixo = Serial.read();};
  Serial.println("Recebeu a string da web");
  Serial.println(comando.substring(1));
  
  if (comando.substring(1) != "favicon.ico"){
  enviarHexadecimal(comando.substring(1), comando.length() - 1);
  delayMicroseconds(300);
  
  cmdHexa = "";
  bool continua = COM.I2CStartTrans(127);
  
  for(int i=0; i<12; i++){
    caractere = (int)COM.recebeByte();
    Serial.print("CARACTERE BRUTO: ");
    Serial.println(caractere);
    cmdHexa += convertHexa(caractere);
    Serial.print("CARACTERE convertido: ");
    Serial.println(cmdHexa[i]);
  }
  
  COM.I2CStopTrans();
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/html", "<html><h1>Comando Enviado: " + comando.substring(1) + "<br>Comando Recebido: " + cmdHexa +
  "</h1></html>");
 }
}

void handleConfiguraAP(){

  byte cmdRecebido[12];
  byte numBytesLido;
  String comando = server.uri();
  comando = comando.substring(1);
  
  if (comando != "favicon.ico"){
    int posDiv = comando.indexOf(',');
    
    if(posDiv != -1 && posDiv >= 3 && comando.length() >= 7){
      
      tamSsid = posDiv+1;
      tamPass = (comando.substring(posDiv+1)).length()+1;
      (comando.substring(0,posDiv)).toCharArray(ssid, tamSsid);
      (comando.substring(posDiv+1)).toCharArray(password, tamPass);
      
      Serial.print("Escrevendo EEPROM: ");
      Serial.println(ssid);
      Serial.println(password);
      
      EEPROM.begin(TAM_EEPROM);
      for(int i=0; i<tamSsid; i++)
      EEPROM.write(ENDE_SSID+i, ssid[i]);
      for(int i=0; i<tamPass; i++)
      EEPROM.write(ENDE_PASS+i, password[i]);
      EEPROM.write(130, tamSsid);
      EEPROM.write(140, tamPass);
      //EEPROM.commit();
      
      EEPROM.end();
      AP = false;
      ESP.restart();
      
    } else {
      server.send(200, "text/html", "<h1>Parametros de configuracao invalidos <br>Verifique o comprimento dos campos (usuario e senha) e se"
      " esta usando virgula como separador <br>Ex.: 192.168.4.1/usuario,senha </h1>");
    }
 }
}


void setup() {
  
  //SSID e PASSWORD de fábrica
  copiaCString(ssid, "TESTE");
  copiaCString(password, "12C1009519");
  delay(1500);
  
  COM.I2CBegin(2,0); //*************** FOI COMENTADO ************
  
  Serial.begin(115200);
  delay(10);
  
  // prepare GPIO
  pinMode(15, INPUT); // pinMode(0, INPUT);
  pinMode(14, INPUT); //pinMode(2, INPUT);
  Serial.println("");
  
  if(digitalRead(TRIGGER) == 0){
    
    Serial.println();
    Serial.print("Configuring access point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(factorySsid, factoryPassword);
    
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    
    server.onNotFound(handleConfiguraAP);
    server.begin();
    
    Serial.println("HTTP server started");
    AP = true;
  }else{
    
    WiFi.mode(WIFI_STA);
    EEPROM.begin(TAM_EEPROM);
    tamSsid = EEPROM.read(130);
    tamPass = EEPROM.read(140);
    String Ssid;
    String Pass;
    
    for(int i=0; i<tamSsid; i++){
      Ssid += (char)EEPROM.read(ENDE_SSID+i);
    }
    
    for(int i=0; i<tamPass; i++){
      Pass += (char)EEPROM.read(ENDE_PASS+i);
    }
    
    EEPROM.end();
    Serial.println("SSID, PASSWORD");
    Serial.print(Ssid.c_str());
    Serial.print(", ");
    Serial.println(Pass.c_str());
    
    WiFi.begin(Ssid.c_str(), Pass.c_str());
    WiFi.config(ip, gateway, subnet); //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI SETAR O IP FIXO NO NODEMCU
    Serial.println("WiFi Ativo");
    
    // Wait for connection
    while ( WiFi.status() != WL_CONNECTED ) {
      delay ( 500 );
      Serial.print ( "." );
    }
    
    Serial.println ( "" );
    Serial.print ( "Connected to " );
    Serial.println ( Ssid.c_str() );
    Serial.print ( "IP address: " );
    Serial.println ( WiFi.localIP() );
    
    if ( mdns.begin ( "esp8266", WiFi.localIP() ) ) {
      Serial.println ( "MDNS resposta iniciada" );
    }
    
    server.on("/", handleRoot);
//    server.onNotFound(handleQualquer);
    server.begin();
    Serial.println("Server iniciado!");
    AP = false;
  }
}


void loop() {
  if(AP){
    server.handleClient();
  }else{
    mdns.update();
    server.handleClient();
  }
  //delay(500);
}

void enviarHexadecimal(String dado, int tamanho){
  char temp = 0;
  String comando = "";
  
  if (tamanho != 24)
    return;
  
  for(int i=0; i<dado.length(); i++){
    if (dado.charAt(i) >= 48 && dado.charAt(i) < 58){ //verifica se caractere está entre 0 e 9
      temp += dado.charAt(i) - '0'; //converte de ascii para hexa
    if (i % 2 == 0) temp = temp << 4; //desloca 4 bits para esquerda
    } else if (dado.charAt(i) >= 65 && dado.charAt(i) < 71){ //verifica se caractere está entre A e F
      temp += dado.charAt(i) - 'A' + 10; //converte de ascii para hexa
    if (i % 2 == 0) temp = temp << 4; //desloca 4 bits para esquerda
    } else if (dado.charAt(i) >= 97 && dado.charAt(i) < 103){ //verifica se caractere está entre a e f
      temp += dado.charAt(i) - 'a' + 10; //converte de ascii para hexa
    if (i % 2 == 0) temp = temp << 4; //desloca 4 bits para esquerda
    } else {
      return;
    } 
    
    if (i % 2 == 1){
      comando += temp;
      temp = 0;
    }
  }
  
  bool continua = COM.I2CStartTrans(127); // *************** FOI COMENTADO *********
  
  if(continua){
    char pct[12];
    
    pct[0] = comando.charAt(0);
    pct[1] = comando.charAt(1);
    pct[2] = comando.charAt(2);
    pct[3] = comando.charAt(3);
    pct[4] = comando.charAt(4);
    pct[5] = comando.charAt(5);
    pct[6] = comando.charAt(6);
    pct[7] = comando.charAt(7);
    pct[8] = comando.charAt(8);
    pct[9] = comando.charAt(9);
    pct[10] = comando.charAt(10);
    pct[11] = comando.charAt(11);

     // *************** FOI COMENTADO *********
    for(int i=0; i<12; i++){
      COM.enviaByte(pct[i]);
    }
    
  
  }
  
  COM.I2CStopTrans(); // *************** FOI COMENTADO ************
}

 
String convertHexa(char n) {
  String dado = "";
  char n1,n2;
  n1 = n;
  n2 = n;
  n1 = (n1 >> 4) & 15;
  
  if (n1 < 10){
    n1 = n1 + 48;
  } else{
    n1 = n1 + 55;
    dado.concat(n1);
    n2 = n2 & 15;
  }if (n2 < 10){
    n2 = n2 + 48;
  } else {
    n2 = n2 + 55;
    dado.concat(n2);
    return dado; 
  }
  
}

/* // ********************* FUNÇÃO COMENTADA ********************
float getCorrente(){
  int valorMax = 0, valorMin = 0, caractere;
  float Vpp, Vrms, valorCorrente = 0.0;
  String resultado;
  cmdHexa = "";
  bool continua = COM.I2CStartTrans(127);
  
  for(int i=0; i<12; i++){
    caractere = (int)COM.recebeByte();
    //Serial.print("CARACTERE BRUTO: ");
    //Serial.println(caractere);
    //cmdHexa += convertHexa('\'');
    //if (i==5) valorMin = caractere;
    //if (i==6){
    //caractere = caractere << 8;
    //valorMin += caractere;
    //}
    
    //if (i==7) valorMax = caractere;
    //if (i==8){
    //caractere = caractere << 8;
    //valorMax += caractere;
    //}
    cmdHexa += convertHexa(caractere);
    //cmdHexa += convertHexa('\'');
    //Serial.print("CARACTERE convertido: ");
    //Serial.println(cmdHexa[i]);
  }
  
  COM.I2CStopTrans();
  Vpp = ((valorMax - valorMin) * 5.0) / 1024.0;
  Vrms = Vpp*0.5*0.707;
  valorCorrente = Vrms/0.1;
  //resultado = String(valorCorrente);
  return valorCorrente;
}
*/

void copiaCString(char *ptr, char* str){
  int tam = strlen(str);
  
  for(int i=0; i<tam; i++){
    ptr[i] = str[i];
  }
  
  ptr[tam] = '\0';
}
