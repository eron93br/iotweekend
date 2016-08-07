
#include "DHT.h"
#include <ESP8266WiFi.h> 
#include <PubSubClient.h>


int luminosidade = 0; 
float temperatura =0;
float umidade=0;
char msg[50];

const char* ssid = "CrazyTechLabs";
const char* senha = "#iotweekend";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;            // Objeto para manipular WiFi
PubSubClient mqtt(espClient);    // Objeto para manipulacao de mqtt com WiFi

DHT dht(D4,DHT11);

void setup() 
{
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  Serial.begin(115200);
  dht.begin(); 
  // Setup da Conexao WiFi
  Serial.print("conectando na rede WiFi .....");
  WiFi.begin(ssid,senha); // inicia conexao WiFi 
  while(WiFi.status() != WL_CONNECTED)
  { delay(100);
    Serial.print("..");
  }
  Serial.println("");
  //Printar numero do end IP
  Serial.println(WiFi.localIP()); 
  mqtt.setServer(mqtt_server, 1883);
}

void loop() 
{
  // lo - Aquisicao do sinais de sensores
  umidade = (int) dht.readHumidity();
  temperatura = (int)dht.readTemperature(); 
  luminosidade = analogRead(A0);
  sprintf(msg," %d",luminosidade);
  //sprintf(msg," Luz %d - Temp: %d - Umid: %d", luminosidade, temperatura, umidade);
  Serial.println(msg);
  delay(500);

  if( luminosidade < 500)
  {
    //Serial.println("Luz acessa ");
    digitalWrite(D0,HIGH);
    digitalWrite(D1,LOW);
    delay(500);
    digitalWrite(D0,LOW);
    digitalWrite(D1,HIGH);
  }
  else
  {
        //Serial.println("Luz apagada");
        digitalWrite(D0,HIGH);
  }

  // ultima etapa - comunicacao MQTT
  if(mqtt.connected())
  {
     // caso exista conexao
     mqtt.loop(); // Ping de conexao e atualiza dados com broker 
     Serial.println("Enviando Publish de Mesangem MQTT...");
     Serial.println(msg);
     mqtt.publish("eron93br",msg); 
  } else
  {
    Serial.println("Conectando no broker MQTT.."); 
    if(mqtt.connect("eron93br"))
    {
       // conecta com ID eron93br
       mqtt.loop();
       mqtt.subscribe("eron93Leds"); 
    } else
    {
      Serial.println("Nao foi possivel conectar!"); 
    }
  }
  
  delay(1000);
}
