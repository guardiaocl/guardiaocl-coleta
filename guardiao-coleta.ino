#include <dht.h>
#include <SPI.h>
#include <Ethernet.h>
 
#define DHTPIN 7 //porta conectada ao DHT11
static char apiKey[] = "xxxxxxxx-yyyy-zzzz-wwww-kkkkkkkkkkkk"; //@todo Alterar para a API Key do Projeto.
static char serial[] = "GUC0001"; //@todo Alterar para a Serial do Dispositivo.
 
dht DHT;
EthernetClient client;
 
signed long next;
unsigned long frequencyTime = 60000; //Frequência do Tempo que será lido e enviado os valores(1 min.).
static byte mac[] = { 0xDE, 0xAD, 0x09, 0xEF, 0xFE, 0xED }; // Endereço MAC, mude caso tenha vários
static char serverName[] = "guardiao.cl"; //NÃO ALTERE ESSE VALOR
 
/**
//Caso queira definir o IP manualmente, descomente.
IPAddress ip(10,10,1,2);
IPAddress localDns(10,10,1,1);
IPAddress gateway(10,10,1,1);
IPAddress subnet(255,255,255,0);
**/
 
 
void setup() {
  Serial.begin(9600);
  Serial.println(F("Sistema Inicializado...")); 
  Ethernet.begin(mac); //IP Automático
  //Ethernet.begin(mac, ip, localDns, gateway, subnet); //IP Manual
 
  next = 0;
  Serial.print(F("IP Atual"));
  Serial.println(Ethernet.localIP());
}
 
void loop() {
  if (((signed long)(millis() - next)) > 0)
  {
    Serial.println(F("Coletando...")); 
    next = millis() + frequencyTime;
    char temp[6] PROGMEM;
    char hum[6] PROGMEM;
    int chk = DHT.read11(DHTPIN); //É possível utilizar outros sensores DHT, necessário mudar a chamada.
    if(chk == DHTLIB_OK){  
      dtostrf(DHT.temperature, 1, 2, temp);
      dtostrf(DHT.humidity, 1, 2, hum);
    }
    else{
      temp[0] = '0';
      hum[0] = '0';
    }
    char queryString[128];
    char outBuf[128];
    char host[64];
 
    if (client.connect(serverName, 80)) 
    {
      sprintf(queryString,"apiKey=%s&temperatura=%s&humidade=%s",apiKey, temp,hum);
      sprintf(outBuf,"GET /collect/%s/?%s HTTP/1.1",serial,queryString);
      client.println(outBuf);
      sprintf(host,"Host: %s",serverName);
      client.println(host);
      client.println("User-Agent: arduino-ethernet");
      client.println("Connection: close");
      client.println();
      client.stop();
    }
    else
    {
      Serial.println(F("Falha de conexão"));
    } 
  }
 
}