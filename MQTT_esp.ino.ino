#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//****************************************
//  Dispositivo de Controle de Iluminação
//  Sub -> (Comando = 1, Comando = 0) -> Se (Comando == 1) Liga Lampada, Se (Comando == 0) Desliga Lampada
//  Pub -> (Comando = 1, Comando = 0) -> Se (Comando == 1) Respota "On", Se (Comando == 0) Respota "Off"
//****************************************

#define TOPICO_SUB_1 "/uema/carga/0201"
#define TOPICO_SUB_2 "/uema/carga/0202"
#define TOPICO_SUB_3 "/uema/carga/0203"
#define TOPICO_SUB_4 "/uema/carga/0204"

#define TOPICO_PUB_1 "/uema/res/0201"
#define TOPICO_PUB_2 "/uema/res/0202"
#define TOPICO_PUB_3 "/uema/res/0203"
#define TOPICO_PUB_4 "/uema/res/0204"

// MUDAR SENHA WIFI (SSID) PASSWORD

const char *SSID = "";     // Nome da Minha rede Wifi
const char *PASSWORD = ""; // Senha da Minha rede Wifi

// IP E PORTA DO SERVIDOR MQTT
const char *BROKER_MQTT = ""; // Endereço do Servidor Broker;
int BROKER_PORT = 1883;                     // Porta do Servidor

// USUARIO E SENHA DO SERVIDOR MQTT

const char *User_MQTT = ""; // Usuario MQTT
const char *Pass_MQTT = ""; // Senha MQTT

// PINOS DO RELE
int Rele1 = 16;
int Rele2 = 15;
int Rele3 = 14;
int Rele4 = 13;

int Contagem = 0;

WiFiClient espClient;         // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient


void setup()
{
  // put your setup code here, to run once:
  InitPinos();
  InitSerial(); 
  initWiFi();   
  initMQTT(); 
}

void loop()
{
  // put your main code here, to run repeatedly:
  // func_Teste();
  verificaConexaoWIFIMQTT(); // Verifica Conexão WIFI MQTT
  MQTT.loop();
  mqtt_callback;
  delay(1000);
}

// Inicializar os Pinos
void InitPinos()
{
  pinMode(Rele1, OUTPUT);
  pinMode(Rele2, OUTPUT);
  pinMode(Rele3, OUTPUT);
  pinMode(Rele4, OUTPUT);
  digitalWrite(Rele1, LOW);
  digitalWrite(Rele2, LOW);
  digitalWrite(Rele3, LOW);
  digitalWrite(Rele4, LOW);
}

// Inicializar a comunicação serial
void InitSerial()
{
  Serial.begin(115200);
}

// Inicializar a comunicação wifi
void initWiFi()
{
  delay(10);
  Serial.println("Conexao WI-FI");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");

  reconectWiFi();
}

// Inicializar a comunicação mqtt
void initMQTT()
{
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}


void mqtt_callback(char* topic, byte* payload, unsigned int length){

  
  String msg;
  
     //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    } 


    Serial.print("Topico ");
    Serial.println(topic);
    Serial.print("Mensagem ");
    Serial.println(msg);


   if (strcmp(topic, TOPICO_SUB_1) == 0)
  {
    if (msg == "0")
    {
      digitalWrite(Rele1, 0);
      MQTT.publish(TOPICO_PUB_1, 0, false, "0");
      Serial.println("Status: Rele1 desligado!");
    }
    else
    {
      digitalWrite(Rele1, 1);
      MQTT.publish(TOPICO_PUB_1, 0, false, "1");
      Serial.println("Status: Rele1 Ligou");
    }
  }
  else if (strcmp(topic, TOPICO_SUB_2) == 0)
  {
    if (msg == "0")
    {
      digitalWrite(Rele2, 0);
      MQTT.publish(TOPICO_PUB_2, 0, false, "0");
      Serial.println("Status: Rele2 desligado");
    }
    else
    {
      digitalWrite(Rele2, 1);
      MQTT.publish(TOPICO_PUB_2, 0, false, "1");
      Serial.println("Status: Rele1 Ligou");
    }
  }
  else if (strcmp(topic, TOPICO_SUB_3) == 0)
  {
    if (msg == "0")
    {
      digitalWrite(Rele3, 0);
      MQTT.publish(TOPICO_PUB_3, 0, false, "0");
      Serial.println("Status: Rele3 desligado");
    }
    else
    {
      digitalWrite(Rele3, 1);
      MQTT.publish(TOPICO_PUB_3, 0, false, "1");
      Serial.println("Status: Rele3 Ligou");
    }
  }
  else if (strcmp(topic, TOPICO_SUB_4) == 0)
  {
    if (msg == "0")
    {
      digitalWrite(Rele4, 0);
      MQTT.publish(TOPICO_PUB_4, 0, false, "0");
      Serial.println("Status: Rele4 desligado");
    }
    else
    {
      digitalWrite(Rele4, 1);
      MQTT.publish(TOPICO_PUB_4, 0, false, "1");
      Serial.println("Status: Rele4 Ligou");
    }
  }

}

void verificaConexaoWIFIMQTT()
{
  reconectWiFi();
  reconnectMQTT(); // verifica conexão Mqtt
}

void reconectWiFi()
{

  // Teste se ja esta conectado;

  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD); // Conectar a rede Wifi

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print("Aguarde estabelecendo conexão");
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.println(SSID);
  Serial.print("Meu IP: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT()
{

  //  String clientName = WiFi.macAddress();

  while (!MQTT.connected())
  {
    Serial.print("  Tentando se conectar ao Broker MQTT");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect("servidor_iot_esp", User_MQTT, Pass_MQTT))
    {
      Serial.println("Conectado com sucesso ao broker MQTT");
      MQTT.subscribe(TOPICO_SUB_1);
      MQTT.subscribe(TOPICO_SUB_2);
      MQTT.subscribe(TOPICO_SUB_3);
      MQTT.subscribe(TOPICO_SUB_4);
    }
    else
    {
      Serial.println("Falha ao reconectar no broker");
      Serial.println("Havera nova tentatica de conexao em 2s");
      delay(2000);
    }
  }
}
