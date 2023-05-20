
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#define BOT_TOKEN "6096150204:AAG4ldebIbL12Vhp5WtqlfGZFTFSArH10HM"

#define TOPICO_SUB_1 "/uema/gas/0201"

#define TOPICO_PUB_1 "/uema/gas/0201"
#define TOPICO_PUB_2 "/uema/carga/0201"

const char* SSID = "esp";        // Nome da Minha rede Wifi
const char* PASSWORD  = "euseiasenha";

// Use @myidbot (IDBot) to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "1071945597"
bool flag = 1;

bool status_gas = false;
bool old_status_gas = false;


X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void bot_telegram() {
  
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.or

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  bot.sendMessage(CHAT_ID, "Olá, eu sou o Bot de Alerta de Vazamento de Gás! Meu objetivo é garantir que você fique seguro e protegido de vazamentos de gás perigosos. Comigo, você pode ter a tranquilidade de saber que será informado instantaneamente se for detectado qualquer vazamento em sua área. Então, fique tranquilo e deixe que eu cuide da sua segurança.", "");
}

// IP E PORTA DO SERVIDOR MQTT
const char *BROKER_MQTT = "34.229.145.165"; // Endereço do Servidor Broker;
int BROKER_PORT = 1883;       // Porta do Servidor

// USUARIO E SENHA DO SERVIDOR MQTT

const char *User_MQTT = "autohome"; // Usuario MQTT
const char *Pass_MQTT = "comida05"; // Senha MQTT

// PINOS DO RELE
int GAS = 12; // gas
int Rele2 = 15;  // led

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
  pinMode(GAS, INPUT);  // DEFINE O PINO COMO ENTRADA
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
 // bot_telegram();
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
//  mqtt_callback;
  sensor_gas();
  delay(1000);

   
 
}

void sensor_gas( void ){

  status_gas = digitalRead(GAS);

  if(status_gas == old_status_gas ) return;

  old_status_gas = status_gas ;

  if(status_gas == 0){
    Serial.println("Vai Explodir");
    MQTT.publish(TOPICO_PUB_1, "0");
    MQTT.publish(TOPICO_PUB_2, "1");
    bot.sendMessage(CHAT_ID, "Atenção! Existe a possibilidade de vazamento de gás em sua casa, o que pode representar um sério risco para você e sua família. Diante disso, é importante seguir algumas medidas de segurança para garantir a proteção de todos", "");
    bot.sendMessage(CHAT_ID, "1. Não acenda qualquer objeto com chama. 2. Não ligue ou desligue qualquer aparelho elétrico, interruptores de luz, tomadas e nem mesmo telefone celular. 3.  Abra as janelas e portas da casa para permitir a circulação de ar. 4. Desligue a chave geral de energia elétrica. ", "");
  }else{
    Serial.println("Não Vai Explodir");
    MQTT.publish(TOPICO_PUB_1, "1");
    MQTT.publish(TOPICO_PUB_2, "0");
  }

}





