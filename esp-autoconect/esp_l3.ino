#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <string.h>
#define TIMEOUT 1000

using namespace std;

/* Set these to your desired credentials. */
const char *ssid = "espFirstTest";
const char *password = "12345678";

ESP8266WebServer server(80);

void enviarHexadecimal(String, int);
String leStringSerial();
String convertHexa(char);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
  Serial.print("Voce esta conectado !!!");
  server.send(200, "text/html", "<h1>Voc&ecirc est&aacute conectado!</h1>");
}

void handleQualquer(){
  
  unsigned long tempo;
  char caractere;
  String comando = server.uri();
  String cmdRecebido = "", cmdHexa = "";
  
  while (Serial.available()){char lixo = Serial.read();};
  
  if (comando.substring(1) != "favicon.ico"){
    enviarHexadecimal(comando.substring(1), comando.length() - 1);
    tempo = millis();
    
    while (Serial.available() < 12){
        if ((millis() - tempo) > TIMEOUT){
          cmdHexa = "Time Out";
          break;
        }
    }

    Serial.write(45);
    
    if (Serial.available() >= 12){
      Serial.println("deu certo !!!!!");
      while(Serial.available()){
        caractere = Serial.read();
        cmdHexa += convertHexa(caractere);
        delay(10);     //Aguarda 10ms para o buffer ler próximo caractere
      }
    }
    server.send(200, "text/html", "<html> <script>window.opener.document.getElementById('rb').value = \"" + cmdHexa + "\"; </script><h1>Comando Enviado: " + comando.substring(1) + "<br id = \"resposta\">Comando Recebido: " + cmdHexa + " </h1></html>");
  }
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.print("Configurando ponto de acesso...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.onNotFound(handleQualquer);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  
  server.handleClient();
}

void enviarHexadecimal(String dado, int tamanho){
  char temp = 0;
  String comando = "";

  Serial.print("Tamanho do dado ");
  Serial.println(tamanho);
  
  if (tamanho != 24)
    return;
   
  for(int i=0; i<dado.length(); i++){
    if (dado.charAt(i) >= 48 && dado.charAt(i) < 58){ //verifica se caractere está entre 0 e 9
      temp += dado.charAt(i) - '0'; //converte de ascii para hexa
      if (i % 2 == 0) temp = temp << 4; //desloca 4 bits para esquerda
    } else
    if (dado.charAt(i) >= 65 && dado.charAt(i) < 71){ //verifica se caractere está entre A e F
      temp += dado.charAt(i) - 'A' + 10; //converte de ascii para hexa
      if (i % 2 == 0) temp = temp << 4; //desloca 4 bits para esquerda
    } else
    if (dado.charAt(i) >= 97 && dado.charAt(i) < 103){ //verifica se caractere está entre a e f
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
  
  Serial.write(comando.charAt(0));
  Serial.write(comando.charAt(1));
  Serial.write(comando.charAt(2));
  Serial.write(comando.charAt(3));
  Serial.write(comando.charAt(4));
  Serial.write(comando.charAt(5));
  Serial.write(comando.charAt(6));
  Serial.write(comando.charAt(7));
  Serial.write(comando.charAt(8));
  Serial.write(comando.charAt(9));
  Serial.write(comando.charAt(10));
  Serial.write(comando.charAt(11));
}

String convertHexa(char n)
{
  String dado = "";
  char n1,n2;
  n1 = n;
  n2 = n;
  n1 = (n1 >> 4) & 15;
  if (n1 < 10)
    n1 = n1 + 48;
  else 
    n1 = n1 + 55;
  dado.concat(n1);
  n2 = n2 & 15;
  if (n2 < 10)
    n2 = n2 + 48;
  else 
    n2 = n2 + 55;
  dado.concat(n2);
  return dado;
}
