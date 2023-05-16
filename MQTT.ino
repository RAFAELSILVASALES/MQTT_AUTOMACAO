
void mqtt_callback(char* topic, byte* payload, unsigned int length){

  String payloadStr = "";
  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }
  String topicStr = String(topic);

    Serial.print("----|Topico: ");
    Serial.println(topic);
    Serial.print("--|Mensagem: ");
    Serial.println(payloadStr);







if (topicStr.equals(TOPICO_SUB_1)) {
    if (payloadStr.equals("1")) {
      digitalWrite(Rele1, 1);
      MQTT.publish(TOPICO_PUB_1, "1");
      Serial.print("Status: Rele1 Ligou");

    } else if (payloadStr.equals("0")) {
      digitalWrite(Rele1, 0);
      MQTT.publish(TOPICO_PUB_1, "0");
      Serial.print("Status: Rele1 desligado!");
    }

  }
  
    if (topicStr.equals(TOPICO_SUB_2)) {
      if (payloadStr.equals("1")) {
        digitalWrite(Rele2, 1);
        MQTT.publish(TOPICO_PUB_2, "1");
        Serial.print("Status: Rele2 Ligou");

    } else if (payloadStr.equals("0")) {
      digitalWrite(Rele2, 0);
      MQTT.publish(TOPICO_PUB_2, "0");
      Serial.print("Status: Rele2 desligado!");
    }
   }
  
      if (topicStr.equals(TOPICO_SUB_3)) {
        if (payloadStr.equals("1")) {
        digitalWrite(Rele3, 1);
        MQTT.publish(TOPICO_PUB_3, "1");
        Serial.print("Status: Rele3 Ligou");

    } else if (payloadStr.equals("0")) {
      digitalWrite(Rele3, 0);
      MQTT.publish(TOPICO_PUB_3, "0");
      Serial.print("Status: Rele3 desligado!");
    }
  }

    if (topicStr.equals(TOPICO_SUB_4)) {
      if (payloadStr.equals("1")) {
        digitalWrite(Rele4, 1);
        MQTT.publish(TOPICO_PUB_4, "1");
        Serial.print("Status: Rele4 Ligou");

    } else if (payloadStr.equals("0")) {
      digitalWrite(Rele4, 0);
      MQTT.publish(TOPICO_PUB_4, "0");
      Serial.print("Status: Rele4 desligado!");
    }
  }
 

}
