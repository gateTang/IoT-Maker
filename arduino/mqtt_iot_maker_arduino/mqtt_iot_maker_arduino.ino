#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define WIFI_NAME WIFI_NAME
#define WIFI_PASS PASSWORD
#define MQTT_SERVER "maqiatto.com"
#define MQTT_PORT 1883
#define MQTT_USERNAME USERNAME
#define MQTT_PASSWORD MQTT PASSWORD
#define MQTT_NAME NAME
#define MQTT_TOPIC_DIGITAL MQTT_TOPIC
#define MQTT_TOPIC_ANALOG MQTT_TOPIC

#define digitalPin D1
#define analogPin 2 //DOUBLE CHECK IF THAT WORKS OR NOT.
WiFiClient espClient;
PubSubClient mqtt(espClient);
String myString;

#define LED_PIN 1

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print ("Message arrived in topic:");
  Serial.println (topic);
  myString = String(topic);
  String msg = "";
  for(int i=0; i<length; i++){
    msg += (char) payload[i];
  }
  Serial.print("Message: ");
  Serial.println (msg);
  if (msg == "1"){
     digitalWrite(LED_BUILTIN, LOW);
     digitalWrite(digitalPin, HIGH);
  }else if(msg=="0"){ //if "off" is published, the builtin LED of the ESP8226 will turn off.
     digitalWrite(LED_BUILTIN, HIGH);
     digitalWrite(digitalPin, LOW);
  }
   int analogValue = msg.toInt();
   analogWrite(analogPin, analogValue);
   delay (1);
  }
void setup() {
  // put your setup code here, to run once:
Serial.begin (115200);

WiFi.begin (WIFI_NAME, WIFI_PASS);
while (WiFi.status() !=WL_CONNECTED){
  delay (500);
  Serial.println (".");
}
Serial.println ("");
Serial.println ("WiFi connected");

mqtt.setServer (MQTT_SERVER, MQTT_PORT);
mqtt.setCallback(callback);
while (!mqtt.connected()){
  if (mqtt.connect (MQTT_NAME, MQTT_USERNAME, MQTT_PASSWORD)){
    mqtt.subscribe (MQTT_TOPIC_DIGITAL);
    mqtt.subscribe (MQTT_TOPIC_ANALOG);
    Serial.println ("MQTT connected");
  } else {
    Serial.print ("Fail, rc ");
    Serial.print(mqtt.state());
    Serial.println (" try again in 5 seconds");
    delay(5000);
  }
}
pinMode(LED_BUILTIN, OUTPUT);
pinMode(digitalPin, OUTPUT);
//pinMode (analogPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
mqtt.loop();
}
