#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
#include "i2c_lmi.h"

using namespace std;

//DEFINIÇÃO DE IP FIXO PARA O NODEMCU
//IPAddress ip(192,168,0,175); //COLOQUE UMA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR. EX: 192.168.1.110 **** ISSO VARIA, NO
//MEU CASO É: 192.168.0.175
//IPAddress gateway(192,168,0,1); //GATEWAY DE CONEXÃO (ALTERE PARA O GATEWAY DO SEU ROTEADOR)
//IPAddress subnet(255,255,255,0); //MASCARA DE REDE

String cmdHexa = "";

MDNSResponder mdns;
ESP8266WebServer server(80);

I2C_LMI COM; 

void enviarHexadecimal(String, int);
String leStringSerial();
String convertHexa(char);

void handleRoot() {
  server.send(200,"text/html", "<html><h1>Voc&ecirc est&aacute conectado!</h1></html>");
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


void setup() {
  
  delay(1500);
  
  COM.I2CBegin(2,0);
  
  Serial.begin(115200);
  
  // Inicialização do WiFiManager
  WiFiManager wifiManager;
  
  // Descomente e excute apenas uma vez, se quer apagar as informações das redes salvas
  //wifiManager.resetSettings();
  
  // configuração fixa da porta de acesso : (IP, GATEWAY, SUBNET)
 // wifiManager.setAPConfig(IPAddress(192,168,0,175), IPAddress(192,168,0,1), IPAddress(255,255,255,0));

  // fetches ssid and pass from eeprom and tries to connect
  // caso não conecte inicia o modo ponto de acesso
  // com o nome "AutoConnectAP", string pode ser modificada
  // então entra num loop até se conectar
  wifiManager.autoConnect("AutoConnectAP");
  // Não é necessário o nome, gera-se um nome por default : ESP + ChipID
  //wifiManager.autoConnect();
  
  // chegando aqui você está conectado com o WiFi
  Serial.println("Conectado.");
  
  server.on("/", handleRoot);
  server.onNotFound(handleQualquer);
  server.begin();
  Serial.println("Server iniciado!");
}


void loop() {
  mdns.update();
  server.handleClient();
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

    for(int i=0; i<12; i++){
      COM.enviaByte(pct[i]);
    }
    
  }
  
  COM.I2CStopTrans();
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
