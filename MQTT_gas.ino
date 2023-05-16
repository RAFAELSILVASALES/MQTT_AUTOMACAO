#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//****************************************
//  Dispositivo de Controle de Iluminação
//  Sub -> (Comando = 1, Comando = 0) -> Se (Comando == 1) Liga Lampada, Se (Comando == 0) Desliga Lampada
//  Pub -> (Comando = 1, Comando = 0) -> Se (Comando == 1) Respota "On", Se (Comando == 0) Respota "Off"
//****************************************

#define TOPICO_SUB_1 "/uema/gas/0201"

#define TOPICO_PUB_1 "/uema/res/0201"

// MUDAR SENHA WIFI (SSID) PASSWORD

const char *SSID = "";     // Nome da Minha rede Wifi
const char *PASSWORD = ""; // Senha da Minha rede Wifi

// IP E PORTA DO SERVIDOR MQTT
const char *BROKER_MQTT = ""; // Endereço do Servidor Broker;
int BROKER_PORT = 1883;       // Porta do Servidor

// USUARIO E SENHA DO SERVIDOR MQTT

const char *User_MQTT = ""; // Usuario MQTT
const char *Pass_MQTT = ""; // Senha MQTT

// PINOS DO RELE
int Rele1 = 12; // gas
int Rele2 = 3;  // led

int Contagem = 0;

WiFiClient espClient;         // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient

// Inicializar a comunicação wifi
void initWiFi()
{
  delay(10);
  Serial.println("--|Conexao WI-FI--|");
  Serial.print("--|Conectando-se na rede--|: ");
  Serial.println(SSID);
  Serial.println("--Aguarde--");

  reconectWiFi();
}

void reconnectMQTT()
{

  //  String clientName = WiFi.macAddress();

  while (!MQTT.connected())
  {
    Serial.print("  ---Tentando se conectar ao Broker MQTT---");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect("servidor_iot_esp", User_MQTT, Pass_MQTT))
    {
      Serial.println("---Conectado com sucesso ao broker MQTT---");
      MQTT.subscribe(TOPICO_SUB_1);
    }
    else
    {
      Serial.println("---Falha ao reconectar no broker---");
      Serial.println("----Havera nova tentatica de conexao em 2s---");
      delay(2000);
    }
  }
}

// Inicializar a comunicação serial
void InitSerial()
{
  Serial.begin(115200);
}

// Inicializar a comunicação wifi

// Inicializar a comunicação mqtt
void initMQTT()
{
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}

// Inicializar os Pinos
void InitPinos()
{
  pinMode(Rele1, INPUT);  // DEFINE O PINO COMO ENTRADA
  pinMode(Rele2, OUTPUT); // DEFINE O PINO COMO SAÍDA
  digitalWrite(Rele2, 0); // LED INICIA DESLIGADO
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
    Serial.print("-----Aguarde estabelecendo conexão------");
  }

  Serial.println();
  Serial.print("----Conectado com sucesso na rede ------");
  Serial.println(SSID);
  Serial.print("Meu IP: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  // put your setup code here, to run once:
  InitPinos();
  InitSerial();
  initWiFi();
  initMQTT();
}

void mqtt_callback(char *topic, byte *payload, unsigned int length)
{

  String payloadStr = "";
  for (int i = 0; i < length; i++)
  {
    payloadStr += (char)payload[i];
  }
  String topicStr = String(topic);

  Serial.print("----|Topico: ");
  Serial.println(topic);
  Serial.print("--|Mensagem: ");
  Serial.println(payloadStr);

  if (topicStr.equals(TOPICO_SUB_1))
  {
    if (payloadStr.equals("1"))
    {
      digitalWrite(Rele2, 1);
      MQTT.publish(TOPICO_PUB_1, "1");
      Serial.print("Status: Rele2 Ligou");
    }
    else if (payloadStr.equals("0"))
    {
      digitalWrite(Rele2, 0);
      MQTT.publish(TOPICO_PUB_1, "0");
      Serial.print("Status: Rele2 desligado!");
    }
  }
}

void loop()
{

  verificaConexaoWIFIMQTT(); // Verifica Conexão WIFI MQTT
  MQTT.loop();
  mqtt_callback;
  delay(1000);
}